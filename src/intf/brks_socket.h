
#ifndef _BRKS_INTF_SOCKET_H_
#define _BRKS_INTF_SOCKET_H_

#include "glo_def.h"

//typedef int   brks_fd_t;
typedef i32         brks_socket_t;


brks_socket_t create_and_bind_socket(u16 port);

i32 set_socket_non_block(brks_socket_t sfd);

i32 nio_recv(brks_socket_t sockfd, i8* buffer, i32 length, i32* ret);

i32 nio_write(brks_socket_t fd, i8* buf, i32 len);


#endif

