
#include "channel.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "Logger.h"

i32 brks_write_channel(brks_socket_t s, brks_channel_t *ch, size_t size)
{
    ssize_t         n;
    i32             err;
    struct iovec    iov[1];
    struct msghdr   msg;

    union {
        struct cmsghdr  cm;
        char            space[CMSG_SPACE(sizeof ch->data)];
    } cmsg;

    if (ch->len == 0) {
        msg.msg_control    = NULL;
        msg.msg_controllen = 0;

    } else {
        msg.msg_control    = (caddr_t) &cmsg.space;
        msg.msg_controllen = ch->len;

        memset(&cmsg, 0, sizeof(cmsg));
        cmsg.cm.cmsg_len   = CMSG_LEN(ch->len);
        cmsg.cm.cmsg_level = SOL_SOCKET;
        if ((ch->command == BRKS_CMD_OPEN_CHANNEL) || (ch->command == BRKS_CMD_MOVEFD))
        {
            cmsg.cm.cmsg_type  = SCM_RIGHTS;
        }

        /*
         * We have to use ngx_memcpy() instead of simple
         *   *(int *) CMSG_DATA(&cmsg.cm) = ch->fd;
         * because some gcc 4.4 with -O2/3/s optimization issues the warning:
         *   dereferencing type-punned pointer will break strict-aliasing rules
         *
         * Fortunately, gcc with -O1 compiles this ngx_memcpy()
         * in the same simple assignment as in the code above
         */
        memcpy(CMSG_DATA(&cmsg.cm), &ch->data, ch->len);
    }

    msg.msg_flags = 0;

    iov[0].iov_base = (char *) ch;
    iov[0].iov_len  = size;

    msg.msg_name    = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov     = iov;
    msg.msg_iovlen  = 1;

    n = sendmsg(s, &msg, 0);

    if (n == -1) {
        err = errno;
        if (err == EAGAIN) {
            return EAGAIN;
        }

        LOG_ERROR("sendmsg() failed");
        return RET_ERROR;
    }

    return RET_OK;
}

i32 brks_read_channel(brks_socket_t s, brks_channel_t *ch, size_t size)
{
    ssize_t        n;
    i32            err;
    struct iovec   iov[1];
    struct msghdr  msg;

    union {
        struct cmsghdr  cm;
        char            space[CMSG_SPACE(sizeof ch->data)];
    } cmsg;

    iov[0].iov_base = (char *) ch;
    iov[0].iov_len  = size;

    msg.msg_name    = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov     = iov;
    msg.msg_iovlen  = 1;

    msg.msg_control    = (caddr_t) &cmsg.space;
    msg.msg_controllen = sizeof(ch->data);

    n = recvmsg(s, &msg, 0);

    if (n == -1) {
        err = errno;
        if (err == EAGAIN) {
            return EAGAIN;
        }

        LOG_ERROR("recvmsg() failed");
        return RET_ERROR;
    }

    if (n == 0) {
        LOG_ERROR("recvmsg() returned zero");
        return RET_ERROR;
    }

    if ((ch->command == BRKS_CMD_OPEN_CHANNEL) || (ch->command == BRKS_CMD_MOVEFD))
    {
        if (cmsg.cm.cmsg_len < (socklen_t) CMSG_LEN(sizeof(int))) {
            LOG_ERROR("recvmsg() returned too small ancillary data");
            return RET_ERROR;
        }

        if (cmsg.cm.cmsg_level != SOL_SOCKET || cmsg.cm.cmsg_type != SCM_RIGHTS)
        {
            LOG_ERROR("recvmsg() returned invalid ancillary data "
                          "level %d or type %d",
                          cmsg.cm.cmsg_level, cmsg.cm.cmsg_type);
            return RET_ERROR;
        }

        memcpy(&ch->fd, CMSG_DATA(&cmsg.space), cmsg.cm.cmsg_len);
    }

    if (msg.msg_flags & (MSG_TRUNC|MSG_CTRUNC)) {
        LOG_ERROR("recvmsg() truncated data");
    }

    return n;
}


void brks_close_channel(brks_socket_t* fd)
{
    if (close(fd[0]) == -1)
    {
        LOG_ERROR("close() channel failed");
    }

    if (close(fd[1]) == -1)
    {
        LOG_ERROR("close() channel failed");
    }
}



