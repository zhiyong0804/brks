
#ifndef _BRKS_INTF_CONSISTENCY_HASH_CLUSTER_H
#define _BRKS_INTF_CONSISTENCY_HASH_CLUSTER_H

#include <map>
#include <string>
#include <unistd.h>
#include <map>

#include "glo_def.h"

class consistency_hash_cluster_t
{
public:
	consistency_hash_cluster_t();
    ~consistency_hash_cluster_t();
    pid_t get(const std::string& key);   // 获取key在环上的最近的进程ID
    void delete_node(const pid_t key);
    void add_node(const pid_t key);

private:
    std::map<u64, pid_t> server_nodes_; //虚拟节点,key是哈希值，value是进程ID
    const u64 virtual_node_num_ = 512; //每个进程节点关联的虚拟节点个数
    const u64 seed_ = 0x1234ABCD;
};


#endif

