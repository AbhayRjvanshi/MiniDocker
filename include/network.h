#ifndef NETWORK_H
#define NETWORK_H

// Function declarations
int setup_network_namespace(void);
int create_veth_pair(const char *veth_host, const char *veth_container);
int setup_bridge(const char *bridge_name);
int configure_container_network(const char *veth_name, const char *ip_addr);

#endif