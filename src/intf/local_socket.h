
#ifndef BRKS_INTF_LOCAL_SOCKET_H__
#define BRKS_INTF_LOCAL_SOCKET_H__


#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define EV_RE  1
#define EV_WR  2
#define EV_RM  4//inner use

typedef struct eventreq {
  int      er_type;
  int      er_handle;
  void    *er_data;
  int      er_eventbits;
};

class LocalSocket
{
public:

    int create_and_bind_socket(char *port);

    int set_socket_non_block(int sfd);

    int nio_write(int fd, char* buf, int len);

    int epoll_init();

    int epoll_waitevent(eventreq* req);

    int epoll_setevent(int socket, int event);
private:
    int epollfd_;
    int sfd_;
};

#endif

