#include "DispatchMsgService.h"
#include "Logger.h"

#include <algorithm>

#define MAX_ITEM_IN_MSG_QUEUE 65536

DispatchMsgService::DispatchMsgService() : msg_queue_( PosixQueue<iEvent>(MAX_ITEM_IN_MSG_QUEUE) )
{
}

DispatchMsgService::~DispatchMsgService()
{
}

BOOL DispatchMsgService::open()
{
    svr_exit_ = FALSE;

#if 0
    /* 异步处理则开启一个线程 */
    if (0 != pthread_create(tid_, NULL, svc, this)
    {
        LOG_ERROR("open failed since create thread failed.")
        return FALSE;
    }
#endif

    return TRUE;
}

void DispatchMsgService::close()
{
    //delete msg_queue_;
    svr_exit_ = TRUE;
}

void DispatchMsgService::subscribe(u32 eid, iEventHandler* handler)
{
    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if (iter != subscribers_.end())
    {
        T_EventHandlers::iterator hdl_iter = std::find(iter->second.begin(), iter->second.end(), handler);
        if ( hdl_iter == iter->second.end())
        {
           iter->second.push_back(handler);
        }
    }
    else
    {
        subscribers_[eid].push_back(handler);
    }
}

void DispatchMsgService::unsubscribe(u32 eid, iEventHandler* handler)
{
    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if (iter != subscribers_.end())
    {
        T_EventHandlers::iterator hdl_iter = std::find(iter->second.begin(), iter->second.end(), handler);
        if ( hdl_iter != iter->second.end())
        {
           iter->second.erase(hdl_iter);
        }
    }
}

i32 DispatchMsgService::enqueue(iEvent* ev)
{
    if (NULL == ev)
    {
        return -1;
    }

    return msg_queue_.enqueue(ev, 0);
}

iEvent* DispatchMsgService::process(const iEvent* ev)
{
    if (NULL == ev)
	  {
        return NULL;
	  }

    u32 eid = ev->get_eid();

    if (eid == EEVENTID_UNKOWN)
    {
        LOG_WARN("DispatchMsgService : unknow evend id %d", eid);
        return NULL;
    }

	LOG_DEBUG("dispatch ev : %d", ev->get_eid());

    T_EventHandlersMap::iterator handlers = subscribers_.find(eid);
    if (handlers == subscribers_.end())
    {
        LOG_WARN("DispatchMsgService : no any event handler subscribed %d", eid);
        return NULL;
    }

    iEvent* rsp = NULL;
    for (auto iter = handlers->second.begin(); iter != handlers->second.end(); iter++)
    {
        iEventHandler* handler = *iter;
        rsp = handler->handle(ev);
    }

	  return rsp;
}

void* DispatchMsgService::svc(void* argv)
{
    DispatchMsgService* dms = (DispatchMsgService*)argv;
    while(!dms->svr_exit_)
    {
        iEvent* ev = NULL;
        /* wait only 1 ms to dequeue */
        if (-1 == dms->msg_queue_.dequeue(ev, 1))
        {
            continue;
        }

        iEvent* rsp = dms->process(ev);
		delete ev;
    }

	dms->subscribers_.clear();

    return NULL;
}


