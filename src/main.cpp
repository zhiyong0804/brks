
#include "DispatchMsgService.h"
#include "interface.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"
#include "iniconfig.h"
#include "configdef.h"

#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <list>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)
#define DEFAULT_BIN_NAME "brks"

int optreset = 0;
int optind = 1;
int opterr = 1;
int optopt;

static int childNum = 4;
static bool isExit = true;
static std::list<pid_t> listPid;

static int brks_fprintf(FILE *file, const char *fmt,  ...)
{
    if (file == NULL)
        return -1;

    va_list args;
    va_start(args,fmt);
    int result = ::vfprintf(file, fmt, args);
    va_end(args);

    return result;
}

static int optiserr(int argc, char * const *argv, int oint, const char *optstr, int optchr, int err)
{
    if(opterr)
    {
        brks_fprintf(stderr, "Error in argument %d, char %d: ", oint, optchr+1);
        switch(err)
        {
        case OPTERRCOLON:
            brks_fprintf(stderr, ": in flags\n");
            break;
        case OPTERRNF:
            brks_fprintf(stderr, "option not found %c\n", argv[oint][optchr]);
            break;
        case OPTERRARG:
            brks_fprintf(stderr, "no argument for option %c\n", argv[oint][optchr]);
            break;
        default:
            brks_fprintf(stderr, "unknown\n");
            break;
        }
    }
    optopt = argv[oint][optchr];
    return('?');
}

static int getopt(int argc, char* const *argv, const char *optstr)
{
    static int optchr = 0;
    static int dash = 0; /* have already seen the - */

    char *cp;

    if (optreset)
        optreset = optchr = dash = 0;
    if(optind >= argc)
        return(EOF);
    if(!dash && (argv[optind][0] !=  '-'))
        return(EOF);
    if(!dash && (argv[optind][0] ==  '-') && !argv[optind][1])
    {
        /*
         * use to specify stdin. Need to let pgm process this and
         * the following args
         */
        return(EOF);
    }
    if((argv[optind][0] == '-') && (argv[optind][1] == '-'))
    {
        /* -- indicates end of args */
        optind++;
        return(EOF);
    }
    if(!dash)
    {
        assert((argv[optind][0] == '-') && argv[optind][1]);
        dash = 1;
        optchr = 1;
    }

    /* Check if the guy tries to do a -: kind of flag */
    assert(dash);
    if(argv[optind][optchr] == ':')
    {
        dash = 0;
        optind++;
        return(optiserr(argc, argv, optind-1, optstr, optchr, OPTERRCOLON));
    }
    if(!(cp = strchr(optstr, argv[optind][optchr])))
    {
        int errind = optind;
        int errchr = optchr;

        if(!argv[optind][optchr+1])
        {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(optiserr(argc, argv, errind, optstr, errchr, OPTERRNF));
    }
    if(cp[1] == ':')
    {
        dash = 0;
        optind++;
        if(optind == argc)
            return(optiserr(argc, argv, optind-1, optstr, optchr, OPTERRARG));
        optarg = argv[optind++];
        return(*cp);
    }
    else
    {
        if(!argv[optind][optchr+1])
        {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(*cp);
    }
    assert(0);
    return(0);
}

static int daemon(int nochdir, int noclose)
{
    int fd;

    switch (fork())
    {
    case -1:
        return (-1);
    case 0:
        break;
    default:
        _exit(0);
    }

    if (setsid() == -1)
        return (-1);

    if (!nochdir)
        (void)chdir("/");

    if (!noclose && (fd = open("/dev/null", O_RDWR, 0)) != -1) {
        (void)dup2(fd, STDIN_FILENO);
        (void)dup2(fd, STDOUT_FILENO);
        (void)dup2(fd, STDERR_FILENO);
        if (fd > 2)
            (void)close (fd);
    }

    return (0);
}

static void usage()
{
    char* usage_name = DEFAULT_BIN_NAME;
    printf("usage: %s [ -d | -v | -c brks.ini | -l log.conf ]\n", usage_name);
    printf("-d: Run in the foreground\n");
    printf("-c brks.ini: Specify a config file\n");
    printf("-l log.conf: Specify a log config file\n");
}

void waitChild(int signo)
{
    LOG_INFO("signo==[%d]\n", signo);

    pid_t wpid;
    int status; 

    while(1)
    {
        wpid = waitpid(-1, &status, WNOHANG);
        int exitStatus = WEXITSTATUS(status);

        if( (wpid > 0) && (WIFEXITED(status) != 0) ) //有子进程退出
        {
            LOG_ERROR("child exit, wpid==[%d]\n", wpid);
            if (exitStatus == -1) 
            {
                LOG_ERROR("child process %d cannot normal start so parent should be exit too.", pid);
                exit(-1);
            }
            else
            {
                if( WTERMSIG(status) == SIGQUIT )
                {
                    continue;
                }
                else 
                {
                    listPid.remove(wpid);
                    ++childNum;
                }
            }
            if ( WIFSIGNALED(status) ) // child exited on an unhandled signal (maybe a bus error or seg fault)
            {
                LOG_ERROR( "child exited on an unhandled signal: %d ",WTERMSIG(status) );
                if( WTERMSIG(status) == SIGQUIT )
                {
                    continue;
                }
                else 
                {
                    listPid.remove(wpid);
                    ++childNum;
                }   
            }
        }
        else if(wpid==-1) //子进程全部退出了
        {
            LOG_INFO("no child is living, wpid==[%d]\n", wpid);
            break;
        }
        else if(wpid==0) //子进程还存活着
        {
            LOG_INFO("child is living, wpid==[%d]\n", wpid);
            break;
        }
    }
}

void SIGUSR1_ProgramExit(int signo) 
{

    std::list<pid_t>::iterator iter;
    for(iter = listPid.begin(); iter !=listPid.end(); ++iter)
    {
        kill(*iter,SIGQUIT);
    }

    isExit = false; 
}

void SIGQUIT_childExit(int signo)
{
    LOG_INFO("I'll be back!");
    isExit = false;
}



int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        usage();
    }

    char ch;
    bool run_in_foreground = false;
    std::string log_file_path = "";
    std::string config_file_path = "";
    while ((ch = getopt(argc,argv, "vdl:c:")) != EOF)
    {
        switch(ch)
        {
        case 'v':
            usage();
            ::exit(0);
            break;
        case 'd':
            run_in_foreground = true;
            break;
        case 'l':
            assert(optarg != NULL);// this means we didn't declare getopt options correctly or there is a bug in getopt.
            log_file_path = std::string(optarg);
            break;
        case 'c':
            assert(optarg != NULL);// this means we didn't declare getopt options correctly or there is a bug in getopt.
            config_file_path= std::string(optarg);
            break;
        default:
            break;
        }
    }

    if(!Logger::instance()->init(log_file_path))
    {
        printf("init log module with file path = %s failed.\n", log_file_path.c_str());
        return -2;
    }

    Iniconfig config;
    if (!config.loadfile(config_file_path))
    {
        LOG_ERROR("load %s failed.", config_file_path.c_str());
        return -3;
    }
    st_env_config conf_args = config.getconfig();

    if (!run_in_foreground)
    {
        if (0 != daemon(0, 0))
        {
            LOG_ERROR("run in daemon failed with error %s", strerror(errno));
            exit(-1);
        }
    }

    std::shared_ptr<DispatchMsgService> dms(new DispatchMsgService);
    dms->open();

    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    //mysqlconn->Init("127.0.0.1", 3306, "root", "123456", "dongnaobike");
    mysqlconn->Init(conf_args.db_ip.c_str(), conf_args.db_port, \
            conf_args.db_user.c_str(), conf_args.db_pwd.c_str(), conf_args.db_name.c_str());

    BusinessProcessor processor(dms, mysqlconn);
    processor.init();

    std::function< iEvent* (const iEvent*)> fun = std::bind(&DispatchMsgService::process, dms.get(), std::placeholders::_1);

    // create server socket and set to non block
    Interface intf(fun);
    int server_socket = intf.create_and_bind_socket(conf_args.svr_port);
    intf.set_socket_non_block(server_socket);

    //fork multi process
    //阻塞SIGCHLD SIGUSR1 信号
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    //注册SIGCHLD信号的处理函数
    struct sigaction act;
    act.sa_handler= waitChild;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);

    //注册SIGUSR1回调函数
    struct sigaction act1;
    act1.sa_handler = SIGUSR1_ProgramExit;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    sigaction(SIGUSR1, &act1, NULL);

    //注册SIGQUIT回调函数
    struct sigaction act2;
    act2.sa_handler = SIGQUIT_childExit;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    sigaction(SIGQUIT, &act2, NULL);

    pid_t processid = 0;
    for(int i = 0; i < childNum; i++)
    {
        processid = 0;
        processid = fork();
        if(processid < 0)
        {
            LOG_ERROR("fork child task failed.");
            break;
        }
        else if (processid == 0)  // this is child task
        {

            //解除对SIGCHLD SIGUSR1 SIGQUTI 信号的阻塞
            sigprocmask(SIG_UNBLOCK, &mask, NULL);            
            break;
        }
        else
        {
            listPid.push_back(processid);
        }
    }

    if (processid > 0) // this is parent process
    {
        //解除对SIGCHLD SIGUSR1 SIGQUTI  信号的阻塞
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        LOG_INFO("parent id is %d", getpid());
        LOG_INFO("brks start successful!");

        while(isExit)
        {
            while( childNum <= 0 )
            {        
                processid = fork();
                if( processid < 0 )
                {
                    LOG_ERROR("fork child task failed.");
                    break;
                }
                else if (processid == 0)  // this is child task
                {
                    break;
                }
                else 
                {
                    listPid.push_back(processid);
                    --childNum;
                }
            }
        }
    }

    if (processid == 0)
    {
        LOG_INFO("this is child process pid=%d", getpid());
        if (!intf.start(server_socket))
        {
            LOG_ERROR("cannot start child process!");
            exit(-1); // if cannot start child process donnot restart server.
        }
        LOG_ERROR("child process exit");
    }

    while(isExit);

    return 0;
}

