
#ifndef BRKS_BUS_RUNNER_BIKE_HANDLER_H_
#define BRKS_BUS_RUNNER_BIKE_HANDLER_H_

#include "glo_def.h"
#include "iEventHandler.h"
#include "sqlconnection.h"
#include "events_def.h"
#include "DispatchMsgService.h"

#include <string>
#include <map>
#include <memory>

class RunnerEventHandler : public iEventHandler
{
public:
	RunnerEventHandler(std::shared_ptr<DispatchMsgService> dms, std::shared_ptr<MysqlConnection> conn);
	virtual ~RunnerEventHandler();
	virtual iEvent* handle(const iEvent* ev);

private:
    CommonRspEv* handle_unlock_req(UnlockEv* ev);
    LockResultRspEv* handle_lock_req(LockEv* ev);
    ListTravelRecordsRspEv* handle_list_travel_records_req(ListTravelRecordsReqEv* ev);

private:
    std::shared_ptr<MysqlConnection> sqlconn_;
    std::shared_ptr<DispatchMsgService> dms_;

};

#endif

