
#include "runer_bike_handler.h"
#include "Logger.h"
#include "bike_service.h"

RunnerEventHandler::RunnerEventHandler(std::shared_ptr<DispatchMsgService> dms, std::shared_ptr<MysqlConnection> conn)
    : dms_(dms), sqlconn_(conn)
{
    dms_->subscribe(EEVENTID_LOCK_REQ, this);
    dms_->subscribe(EEVENTID_UNLOCK_REQ, this);
    dms_->subscribe(EEVENTID_LIST_TRAVELS_REQ, this);
}

RunnerEventHandler::~RunnerEventHandler()
{
    dms_->unsubscribe(EEVENTID_LOCK_REQ, this);
    dms_->unsubscribe(EEVENTID_UNLOCK_REQ, this);
    dms_->unsubscribe(EEVENTID_LIST_TRAVELS_REQ, this);
}

iEvent* RunnerEventHandler::handle(const iEvent* ev)
{
    if (ev == NULL)
    {
        LOG_ERROR("input ev is NULL");
        return NULL;
    }

    u32 eid = ev->get_eid();

    if (eid == EEVENTID_UNLOCK_REQ)
    {
        return handle_unlock_req((UnlockReqEv*) ev);
    }
    else if (eid == EEVENTID_LOCK_REQ)
    {
        return handle_lock_req((LockReqEv*) ev);
    }
    else if (eid == EEVENTID_LIST_TRAVELS_REQ)
    {
        return handle_list_travel_records_req((ListTravelRecordsReqEv*) ev);
    }

    return NULL;
}

CommonRspEv* RunnerEventHandler::handle_unlock_req(UnlockReqEv* ev)
{
    LOG_DEBUG("enter handle_unlock_req");

    std::string mobile = ev->get_mobile();
    std::string bike_code = ev->get_bike_code();

    int bike_code_num = atoi(bike_code.c_str());
    BikeService bs(sqlconn_);
    std::string runner_mobile;
    Bike bk(bike_code_num, mobile);
    if (bs.get_bike(bike_code_num, bk))
    {
        if(bk.st_ == BIKE_ST_UNLOCK)
        {
            if (bk.mobile_.compare(mobile) != 0)
            {
                return new CommonRspEv(ERRO_BIKE_IS_TOOK, "annother one take the bike.", "");
            }
            else
            {
                return new CommonRspEv(ERRO_BIKE_IS_RUNNING, "success", "");
            }
        }
        else if (bk.st_ == BIKE_ST_DAMANGE)
        {
            return new CommonRspEv(ERRO_BIKE_IS_DAMAGED, "annother one take the bike.", "");
        }
        else if (bk.st_ == BIKE_ST_LOCK)   //如果是锁住状态，则解锁
        {
            bk.mobile_ = mobile;    // 设置该单车被mobile用户解锁使用
            if (!bs.unlock(bk))
            {
                return new CommonRspEv(ERRO_PROCCESS_FAILED, "unlock failed.", "");
            }
        }
        else
        {
            // do nothing
        }
    }

    return new CommonRspEv(200, "success", "");
}

LockRspEv* RunnerEventHandler::handle_lock_req(LockReqEv* ev)
{
    std::string mobile = ev->get_mobile();
    std::string bike_code = ev->get_bike_code();
    int bike_code_num = atoi(bike_code.c_str());

    BikeService bs(sqlconn_);
    TravelInfo travel;
    if (bs.lock(mobile, bike_code_num, travel))
    {
        return new LockRspEv(ERRC_SUCCESS, "success", "", travel);
    }
    else
    {
        return new LockRspEv(ERRO_PROCCESS_FAILED, "lock bike failed", "", travel);
    }

    std::vector<TravelRecord> record;
    TravelInfo trave(0, 0, 0, record);
    return new LockRspEv(ERRC_INVALID_DATA, "cannot find bike", "", trave);
}

ListTravelRecordsRspEv* RunnerEventHandler::handle_list_travel_records_req(ListTravelRecordsReqEv* ev)
{
    std::string mobile = ev->get_mobile();
    BikeService bs(sqlconn_);

    TravelInfo travel;
    if (!bs.list_travel_records(mobile, travel))
    {
        return new ListTravelRecordsRspEv(ERRO_PROCCESS_FAILED, "failed", "", travel);
    }

    return new ListTravelRecordsRspEv(ERRC_SUCCESS, "success", "", travel);
}


