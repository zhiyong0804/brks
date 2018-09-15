#ifndef BRKS_INTF_INTERFACE_H_ 
#define BRKS_INTF_INTERFACE_H_

#include <functional>

#include <thrift/concurrency/Thread.h>



class Interface{

public:
    Interface(std::function< iEvent* (const iEvent*)>  callback) : callback_(callback){};
    BOOL start(int port);
    BOOL close();
private:
    std::function< iEvent* (const iEvent*)> callback_;
    apache::thrift::stdcxx::shared_ptr<apache::thrift::concurrency::Thread> thrift_server_;
};

#endif