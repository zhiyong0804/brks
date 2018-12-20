
#ifndef _BRKS_INTF_PROCESS_H_
#define _BRKS_INTF_PROCESS_H_

#include "glo_def.h"
#include "brks_socket.h"
#include "channel.h"

#include <unistd.h>

typedef pid_t       brks_pid_t;
typedef u64         brks_clientid_t;

#define BRKS_INVALID_PID  -1
#define BRKS_MAX_PROCESS_NUM 1024
#define BRKS_MAX_CONNECTION_NUM  102400

typedef void (*brks_spawn_proc_pt) (i32 slot);

typedef struct {
    char*               name;
    brks_pid_t          pid;
    u32                 status;
    brks_socket_t       channel[2];
    brks_spawn_proc_pt  proc;
    u32                 current_connections;   // 当前客户端连接的数量
    u32                 max_connections;       // 最大连接数量
} brks_process_t;

brks_pid_t brks_spawn_process(char *name, brks_spawn_proc_pt proc);
void brks_worker_process_cycle(u32 slot);
i32  brks_start_all_worker_service();
void brks_pass_open_channel(brks_channel_t* ch);

#endif

