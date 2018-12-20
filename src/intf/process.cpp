
#include "process.h"
#include "Logger.h"
#include "interface.h"
#include "brks_socket.h"


#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>


int     brks_argc;
char**  brks_argv;

u32 brks_last_process;
brks_process_t brks_processes[BRKS_MAX_PROCESS_NUM];
u32 brks_current_slot;
brks_pid_t brks_pid;
brks_pid_t brks_parent;

extern Interface* brks_intf;

brks_pid_t brks_spawn_process(char *name, brks_spawn_proc_pt proc)
{
    brks_pid_t pid;

    // find first empty slot
    u32 s = 0;
    for (s = 0; s < BRKS_MAX_PROCESS_NUM; s++)
    {
        if (brks_processes[s].pid == -1)
        {
            break;
        }
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, brks_processes[s].channel) == -1)
    {
        LOG_ERROR("socketpair() failed while spawning \"%s\"", name);
        return BRKS_INVALID_PID;
    }

    if (0 != set_socket_non_block(brks_processes[s].channel[0]))
    {
        LOG_ERROR("failed set to non blocking socket.");
        return BRKS_INVALID_PID;
    }

    if (0 != set_socket_non_block(brks_processes[s].channel[1]))
    {
        LOG_ERROR("failed set to non blocking socket.");
        return BRKS_INVALID_PID;
    }

    brks_current_slot = s;
    pid = fork();

    switch (pid)
    {

    case -1:
        LOG_ERROR("fork() failed while spawning \"%s\"", name);
        brks_close_channel(brks_processes[s].channel);
        return BRKS_INVALID_PID;

    case 0:
        brks_parent = brks_pid;
        brks_pid = getpid();
        proc(s);
        break;

    default:
        break;
    }

    brks_processes[s].proc            = proc;
    brks_processes[s].pid             = brks_pid;
    brks_processes[s].name            = name;
    brks_processes[s].max_connections = BRKS_MAX_CONNECTION_NUM;

    /*
     * if there is an empty slot between 0 and brks_last_process,
     * brks_last_process should not increase
     */
    if (s >= brks_last_process)
        brks_last_process ++;

    return pid;
}

void brks_worker_process_cycle(u32 slot)
{
    LOG_INFO("this is child process pid=%d", getpid());

    /*
     * when entry worker process, first of all, add channel socket to interface.
     */
    brks_intf->add_channel_socket(brks_processes[slot].channel);
    brks_intf->run();
    LOG_ERROR("child process %d exit", getpid());
}

void brks_pass_open_channel(brks_channel_t *ch)
{
    for (u32 i = 0; i < brks_last_process; i++)
    {
        if (i == brks_current_slot
            || brks_processes[i].pid == -1
            || brks_processes[i].channel[0] == -1)
        {
            continue;
        }

        LOG_INFO( "pass channel s:%i pid:%P fd:%d to s:%i pid:%P fd:%d",
                  ch->slot, ch->pid, ch->fd,
                  i, brks_processes[i].pid,
                  brks_processes[i].channel[0]);

        if (RET_OK != brks_write_channel(brks_processes[i].channel[0], ch, CHANNEL_WITHOUT_SPECIAl_SIZE))
        {
            LOG_ERROR("write channel failed with channel[pid=%d]", ch->pid);
        }
    }
}

i32  brks_start_all_worker_service()
{
    brks_channel_t ch;
    ch.command = BRKS_CMD_SERVICE;

    for (u32 i = 0; i < brks_last_process; i++)
    {
        if ((brks_processes[i].pid == -1) || (brks_processes[i].channel[0] == -1))
        {
            continue;
        }

        ch.slot = i;
        ch.pid  = brks_processes[i].pid;
        ch.fd   = brks_processes[i].channel[0];

        LOG_INFO("pass channel s:%i pid:%P fd:%d to s:%i pid:%P fd:%d",
                      ch.slot, ch.pid, ch.fd,
                      i, brks_processes[i].pid,
                      brks_processes[i].channel[0]);

        if (RET_OK != brks_write_channel(brks_processes[i].channel[0], &ch, CHANNEL_WITHOUT_SPECIAl_SIZE))
        {
            LOG_ERROR("write channel failed with channel[pid=%d]", ch.pid);
            return RET_ERROR;
        }
    }

    return RET_OK;
}

i32  brks_start_single_worker_service(u32 slot)
{
    brks_channel_t ch;
    ch.command = BRKS_CMD_SERVICE;
    ch.slot    = slot;
    ch.pid     = brks_processes[slot].pid;
    ch.fd      = brks_processes[slot].channel[0];

    LOG_INFO("pass channel s:%i pid:%P fd:%d to s:%i pid:%P fd:%d",
              ch.slot, ch.pid, ch.fd,
              slot, brks_processes[slot].pid,
              brks_processes[slot].channel[0]);

    if (RET_OK != brks_write_channel(brks_processes[slot].channel[0], &ch, CHANNEL_WITHOUT_SPECIAl_SIZE))
    {
        LOG_ERROR("write channel failed with channel[pid=%d]", ch.pid);
        return RET_ERROR;
    }

    return RET_OK;
}

void brks_master_process_cycle()
{
    pid_t processid;
    LOG_INFO("parent id is %d", getpid());
    LOG_INFO("brks start successful!");

    // if child process exit with -1 then exit parent process.
    // if child process exit with not -1 then restart child process
    int status = 0;
    while (status == 0)
    {
        pid_t pid = wait(&status);  // parent process block here.
        int exitStatus = WEXITSTATUS(status);
        LOG_INFO("parent process wait return with: pid=%d, status=%d, exitStatus=%d.", pid, status, exitStatus);

        if ((WIFEXITED(status) != 0) && (pid > 0))
        {
            if (exitStatus == -1)
            {
                LOG_ERROR("child process %d cannot normal start so parent should be exit too.", pid);
                exit(-1);
            }
            else
            {
                processid = fork();  // restart an child process.
                if (processid == 0) break;  //child process jump out here.
            }
        }

        if (WIFSIGNALED(status)) // child exited on an unhandled signal (maybe a bus error or seg fault)
        {
            processid = fork();  // restart an child process.
            if (processid == 0) break;  //child process jump out here.
        }

fork_worker :
        processid = brks_spawn_process("worker", brks_worker_process_cycle);
        LOG_INFO("fork annother worker %d", processid);
        brks_channel_t channel;
        channel.command = BRKS_CMD_OPEN_CHANNEL;
        channel.pid     = processid;
        channel.len     = 0;
        channel.slot    = brks_current_slot;
        channel.fd      = brks_processes[brks_current_slot].channel[0];
        brks_pass_open_channel(&channel);
        brks_start_single_worker_service(brks_current_slot);
    }
}

