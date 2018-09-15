
#include "BusProcessor.h"
#include "SqlTables.h"

BusinessProcessor::BusinessProcessor(std::shared_ptr<DispatchMsgService> dms, std::shared_ptr<MysqlConnection> conn)
    : dms_(dms), mysqlconn_(conn), ueh_(new UserEventHandler(dms, conn)), runer_(new RunnerEventHandler(dms, conn))
{

}

bool BusinessProcessor::init()
{
    SqlTables tables(mysqlconn_);
    tables.CreateUserInfo();
    tables.CreateBikeTable();
    tables.CreateAccountRecordsTable();
    tables.CreateTravelRecordsInfo();

    return true;
}

BusinessProcessor::~BusinessProcessor()
{
    ueh_.reset();
}

