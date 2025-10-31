#ifndef NETWORK_H
#define NETWORK_H

#include <sys/types.h>
#include <linux/limits.h>

// Function declarations
int setup_network_namespace(pid_t pid);
int create_veth_pair(const char *veth_host, const char *veth_container);
int setup_bridge(void);
int configure_container_network(pid_t pid, const char *veth_host, const char *veth_container);
int cleanup_container_network(pid_t pid);

#endif