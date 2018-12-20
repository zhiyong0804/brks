
#include "brks_socket.h"
#include "Logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


i32 create_and_bind_socket(u16 port)
{
    i32 sfd;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        LOG_ERROR("create socket failed");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
    {
	    LOG_ERROR("bind to port = %d failed.", port);
	    return -1;
    }

    // set SO_REUSEADDR
    i32 opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    return sfd;
}

i32 set_socket_non_block(brks_socket_t sfd)
{
    i32 flags, res;

    flags = fcntl(sfd, F_GETFL);
    if (flags == -1)
    {
        LOG_ERROR("cannot get socket flags!\n");
        return -1;
    }

    flags |= O_NONBLOCK;
    res    = fcntl(sfd, F_SETFL, flags);
    if (res == -1)
    {
        LOG_ERROR("cannot set socket flags!\n");
        return -1;
    }

    return 0;
}

i32 nio_write(i32 fd, i8* buf, i32 len)
{
    i32 write_pos = 0;
    i32 left_len = len;

    while (left_len > 0)
    {
        i32 writed_len = 0;
        if ((writed_len = write(fd, buf + write_pos, left_len)) <= 0)
        {
            if (errno == EAGAIN)
            {
               writed_len = 0;
            }
            else return -1; //表示写失�?
        }
        left_len -= writed_len;
        write_pos += writed_len;
    }

    return len;
}

i32 nio_recv(i32 sockfd, i8* buffer, i32 length, i32* ret)
{
	i32 idx = 0;

	while (1)
    {
		i32 count = recv(sockfd, buffer+idx, length - idx, 0);
		if (count == 0)
        {
			*ret = -1;
			::close(sockfd);
			break;
		}
        else if (count == -1)
		{
			*ret = 0;
			break;
		}
        else
        {
			idx += count;
			if (idx == length) break;
		}
	}

	return idx;
}



