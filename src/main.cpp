
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


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("please input brks <log file config> <config file path>!\n");
        return -1;
    }

    if(!Logger::instance()->init(std::string(argv[1])))
    {
        printf("init log module failed.\n");
        return -2;
    }
    else
    {
        printf("init log module success!");
    }

    Iniconfig config;

    if (!config.loadfile(std::string(argv[2])))
    {
        LOG_ERROR("load %s failed.", argv[2]);
        return -3;
    }
    st_env_config conf_args = config.getconfig();

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
    pid_t processid = 0;
    for(int i = 0; i < 4; i++)
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
            break;
        }
    }

    if (processid > 0) // this is parent process
    {
        LOG_INFO("parent id is %d", getpid());
        LOG_INFO("brks start successful!");
    }
    else if (processid == 0)
    {
        LOG_INFO("this is child process pid=%d", getpid());
        intf.start(server_socket);
    }

    for(;;);

    return 0;
}
