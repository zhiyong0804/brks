
#include "local_socket.h"
#include "Logger.h"

int LocalSocket::create_and_bind_socket(char *port)
{
    struct addrinfo hint, *result;
    int res, sfd;
#if 0
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags    = AI_PASSIVE;

    res = getaddrinfo(NULL, port, &hint, &result);
    if (res == -1)
    {
        LOG_ERROR("can not get address info\n");
        exit(1);
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1)
    {
        LOG_ERROR("can not get socket descriptor!\n");
        exit(1);
    }

    res = bind(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1)
    {
        LOG_ERROR("can not bind the socket!\n");
        exit(1);
    }

    unsigned value = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

    freeaddrinfo(result);

    sfd_ = sfd;
#endif
    return sfd;
}

int LocalSocket::set_socket_non_block(int sfd)
{
    int flags, res;
#if 0
    flags = fcntl(sfd, F_GETFL);
    if (flags == -1)
    {
        LOG_ERROR("cannot get socket flags!\n");
        exit(1);
    }

    flags |= O_NONBLOCK;
    res    = fcntl(sfd, F_SETFL, flags);
    if (res == -1)
    {
        LOG_ERROR("cannot set socket flags!\n");
        exit(1);
    }
#endif
    return 0;
}

int LocalSocket::nio_write(int fd, char* buf, int len)
{
    int write_pos = 0;
    int left_len = len;
#if 0
    while (left_len > 0)
    {
        int writed_len = 0;
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
#endif
    return 0;
}

int LocalSocket::epoll_waitevent(eventreq* req)
{
#if 0
    struct epoll_event events[MAX_EVENTS] = {0};

    int cnt = epoll_wait(epoll, events, MAX_EVENTS, -1);
    for (i = 0; i < cnt; i++)
    {
        if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)
            || !(events[i].events & EPOLLIN))
        {
            LOG_ERROR("socket fd=%d error.", events[i].data.fd);
            close(events[i].data.fd);
            continue;
        }
        else if (events[i].data.fd == sfd)
        {
            struct sockaddr client_addr;
            int addrlen = sizeof(struct sockaddr);

            int sd = accept(sfd, &client_addr, &addrlen);
            if (sd == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                        break;
                }
                else
                {
                    perror("error : cannot accept new socket!\n");
                    continue;
                }
            }

                    res = set_socket_non_block(sd);
                    if (res == -1)
                    {
                        perror("error : cannot set flags!\n");
                        exit(1);
                    }

                    /* 添加socket到epoll�?*/
                    event.data.fd = sd;
                    event.events  = EPOLLET | EPOLLIN;
                    res = epoll_ctl(epoll, EPOLL_CTL_ADD, sd, &event);
                    if (res == -1)
                    {
                        perror("error : cannot add to epoll!\n");
                        exit(1);
                    }
            }
            else
            {
                int cnt;
                char buf[BUF_SIZE] = {0};
                //while (1)
                {
                    memset(buf, 0, sizeof(buf));
                    cnt = read(events[i].data.fd,  buf, BUF_SIZE);
                    if (cnt == -1)
                    {
                        if (errno == EAGAIN)
                        {
                            break;
                        }

                        perror("error : read error!\n");
                        exit(1);

                    }
                    else if (cnt == 0)
                    {
                        close(events[i].data.fd);
                    }

                    printf("receive client data : %s\n", buf);
                    nio_write(events[i].data.fd, buf, strlen(buf));
                    break;
                }
            }
        }
#endif
    return 0;
}

int LocalSocket::epoll_init()
{
    epollfd_ = 0;// epoll_create(MAX_EPOLL_FD);

	return epollfd_;
}

int LocalSocket::epoll_setevent(int socket, int event)
{
#if 0
    int ret;
    struct epoll_event ev;

    if(event == EV_RE)
    {
        ev.data.fd = socket;
        ev.events = EPOLLIN|EPOLLHUP|EPOLLERR;//level triggle
        ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, socket, &ev);
    }
    else if(event == EV_WR)
    {
        ev.data.fd = socket;
        ev.events = EPOLLOUT;//level triggle
        ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, socket, &ev);
    }
    else if(event == EV_RM)
    {
        ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, socket, &ev);
    }
    else
    {
        //do nonthing
    }
#endif
    return 0;
}



