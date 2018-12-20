
#include "DispatchMsgService.h"
#include "interface.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"
#include "iniconfig.h"
#include "configdef.h"
#include "consistency_hash_cluster.h"
#include "process.h"
#include "brks_socket.h"


#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/wait.h>

#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)
#define DEFAULT_BIN_NAME "brks"

int optreset = 0;
int optind = 1;
int opterr = 1;
int optopt;

Interface* brks_intf = NULL;
extern u32 brks_last_process;
extern u32 brks_current_slot;
extern brks_process_t brks_processes[BRKS_MAX_PROCESS_NUM];

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
    printf("-c brks.ini: Specify a parameter config file\n");
    printf("-l log.conf: Specify a log config file\n");
}

int main(int argc, char** argv)
{
    if (argc <= 1) usage();

    char ch;
    bool run_in_foreground = false;
    std::string log_file_path = "";
    std::string config_file_path = "";

    consistency_hash_cluster_t process_hash;

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

    brks_intf = new Interface(fun);
    brks_socket_t server_socket = create_and_bind_socket(conf_args.svr_port);  // 9090 port
    set_socket_non_block(server_socket);

    //fork multi process
    brks_pid_t processid = 0;
    brks_last_process = 0;
    brks_channel_t channel;
    channel.command = BRKS_CMD_OPEN_CHANNEL;
    /*
     * TODO : 4 is magic number, we need to config by conf file.
     */
    for (i32 i = 0; i < 4; i++)
    {
        processid = brks_spawn_process("worker", brks_worker_process_cycle);

        channel.pid  = processid;
        channel.len  = 0;
        channel.slot = brks_current_slot;
        channel.fd   = brks_processes[brks_current_slot].channel[0];
        brks_pass_open_channel(&channel);
    }

    /*
     * TODO : set worker cpu affinity and priority.
     */

    /* all worker start to service, that meams worker add all service socket to epoll */
    brks_start_all_worker_service();

    brks_master_process_cycle();

    for(;;);

    return 0;
}


