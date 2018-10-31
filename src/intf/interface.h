#ifndef BRKS_INTF_INTERFACE_H_
#define BRKS_INTF_INTERFACE_H_

#include <functional>



class Interface
{

public:
    Interface(std::function< iEvent* (const iEvent*)>  callback) : callback_(callback){};
    bool start(int socket);
    bool close();

    int create_and_bind_socket(unsigned short port);
    int set_socket_non_block(int sfd);
    int nio_write(int fd, char* buf, int len);

private:
    std::function< iEvent* (const iEvent*)> callback_;
};

#endif