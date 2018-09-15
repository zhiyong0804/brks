
/*
 * 分发消息服务模块，其实就是把外部收到的消息，转化成内部事件，也就是存在msg->event的解码过程
 * 解码后，再调用每个event handler来处理event，于此每个event handler需要subscribe该event后才会
 * 被调用到.
 */

#ifndef BRK_SERVICE_DISPATCH_EVENT_SERVICE_H_
#define BRK_SERVICE_DISPATCH_EVENT_SERVICE_H_

#include <map>
#include <vector>
#include "event.h"
#include "eventtype.h"
#include "MsgQueue.h"
#include "iEventHandler.h"

class DispatchMsgService
{
public:
    DispatchMsgService();
    virtual ~DispatchMsgService();

    virtual BOOL open();
    virtual void close();

    virtual void subscribe(u32 eid, iEventHandler* handler);
    virtual void unsubscribe(u32 eid, iEventHandler* handler);
    virtual i32 enqueue(iEvent* ev);
	virtual iEvent* process(const iEvent* ev);
    static void* svc(void* argv);
    
protected:
    
    pthread_t tid_;
    
    typedef std::vector<iEventHandler*> T_EventHandlers;
    typedef std::map<u32, T_EventHandlers > T_EventHandlersMap;
    T_EventHandlersMap subscribers_;
    PosixQueue<iEvent> msg_queue_;
    bool svr_exit_;
};


#endif

