
#include "DispatchMsgService.h"
#include "interface.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"

#include <functional>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("please input brks <log file config>!\n");
        return -1;
    }
    
    if(!Logger::instance()->init(std::string(argv[1])))
    {
        printf("init log module failed.\n");
        return -1;
    }
    else
    {
        printf("init log module success!");
    }

    std::shared_ptr<DispatchMsgService> dms(new DispatchMsgService);
    dms->open();
    
    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    mysqlconn->Init("127.0.0.1", 3306, "root", "123456", "dongnaobike");
    
    BusinessProcessor processor(dms, mysqlconn);
    processor.init();

    std::function< iEvent* (const iEvent*)> fun = std::bind(&DispatchMsgService::process, dms.get(), std::placeholders::_1);
    
    Interface intf(fun);
    intf.start(9090);

    LOG_INFO("brks start successful!");

    for(;;);
    
    return 0;
}