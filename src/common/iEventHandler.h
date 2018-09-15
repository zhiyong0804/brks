
#ifndef NS_IEVENT_HANDLER_H_
#define NS_IEVENT_HANDLER_H_

#include "event.h"
#include "eventtype.h"

class iEventHandler
{
public:
    virtual iEvent* handle(const iEvent* ev){ return NULL; };
    virtual ~iEventHandler() {};
    iEventHandler(){};
};

#endif

