
#ifndef NS_EVENT_H_
#define NS_EVENT_H_

#include "glo_def.h"
#include <string>


class iEvent
{
public:
	iEvent(u32 eid, u32 sn);
	virtual ~iEvent();
    virtual std::ostream& dump(std::ostream& out) const { return out; }; 
    u32 generateSeqNo();
    u32 get_eid() const { return eid_; };
    u32 get_sn() const { return sn_; };
    void set_eid(u32 eid){ eid_ = eid; };
private:
    u32   eid_;    /* 事件ID */
    u32   sn_;     /* 事件的序列号 */
};

#endif

