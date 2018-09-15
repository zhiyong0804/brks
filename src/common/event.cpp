
#include "event.h"

iEvent::iEvent(u32 eid, u32 sn) : eid_(eid), sn_(sn)
{
}

iEvent::~iEvent()
{
}

u32 iEvent::generateSeqNo()
{
    static u32 sn = 0;
	return sn++;
}
