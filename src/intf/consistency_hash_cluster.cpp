
#include "consistency_hash_cluster.h"
#include "MurmurHash2.h"

#include <sstream>
#include <string>
#include <string.h>


consistency_hash_cluster_t::consistency_hash_cluster_t()
{

}

consistency_hash_cluster_t::~consistency_hash_cluster_t()
{
    server_nodes_.clear();
}

pid_t consistency_hash_cluster_t::get(const std::string& key)
{
    u64 partition = MurmurHash64A(key.c_str(), strlen(key.c_str()), seed_);
    //沿环的顺时针找到一个大于等于key的虚拟节点
    std::map<u64, pid_t>::iterator it = server_nodes_.lower_bound(partition);
    if(it == server_nodes_.end())
    {
        return server_nodes_.begin()->second;
    }

    return it->second;
}

void consistency_hash_cluster_t::delete_node(const pid_t key)
{
    for(uint64_t j=0; j<virtual_node_num_; ++j)
    {
        std::stringstream node_key;
        node_key<<"SHARD-"<<key<<"-NODE-"<<j;
        u64 partition = MurmurHash64A(node_key.str().c_str(), strlen(node_key.str().c_str()), seed_);

        std::map<u64, pid_t>::iterator it = server_nodes_.find(partition);
        if(it != server_nodes_.end())
        {
            server_nodes_.erase(it);
        }
    }
}

void consistency_hash_cluster_t::add_node(const pid_t key)
{
    for(uint64_t j=0; j < virtual_node_num_; ++j)
    {
        std::stringstream node_key;
        node_key<<"SHARD-"<<key<<"-NODE-"<<j;
        u64 partition = MurmurHash64A(node_key.str().c_str(), strlen(node_key.str().c_str()), seed_);
        server_nodes_.insert(std::pair<u64, pid_t>(partition, key));
    }
}

