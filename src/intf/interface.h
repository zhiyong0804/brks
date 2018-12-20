#ifndef BRKS_INTF_INTERFACE_H_
#define BRKS_INTF_INTERFACE_H_

#include <functional>
#include <vector>

#include "protocol_head.h"
#include "protocol_codec.h"
#include "json_protocol_codec.h"
#include "brks_socket.h"

class Interface
{
public:
    Interface(std::function< iEvent* (const iEvent*)>  callback);

    bool add_server_socket(brks_socket_t socket);
    bool add_channel_socket(brks_socket_t *fd);
    bool close();
    void run();

private:
    std::function< iEvent* (const iEvent*)>  callback_;
    std::vector<brks_socket_t> server_sockets_;
    brks_socket_t  epoll_fd_;
    brks_socket_t  channel_fd_[2];

    protocol_codec_t* codecs_[4]; // there is only 4 protocol codec.

private:
    bool add_epoll_event(brks_socket_t efd, brks_socket_t socket, int events);
    bool accept_client(brks_socket_t efd, brks_socket_t sfd);
    unsigned int read_client_data(brks_socket_t fd, char* buffer, int size);
};

#endif