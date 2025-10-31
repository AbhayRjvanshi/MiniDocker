#include "network.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <errno.h>
#include <sys/stat.h>

#define BRIDGE_NAME "minidocker0"
#define CONTAINER_NETNS_PATH "/var/run/netns"
static int ip_counter = 2; // Start from 172.17.0.2

int setup_network_namespace(pid_t pid) {
    char netns_path[PATH_MAX];
    char pid_str[16];
    
    log_message(LOG_DEBUG, "Setting up network namespace for PID %d", pid);
    
    // Ensure netns directory exists
    mkdir(CONTAINER_NETNS_PATH, 0755);
    
    // Create netns symlink
    snprintf(netns_path, sizeof(netns_path), "%s/%d", CONTAINER_NETNS_PATH, pid);
    snprintf(pid_str, sizeof(pid_str), "/proc/%d/ns/net", pid);
    
    if (symlink(pid_str, netns_path) == -1 && errno != EEXIST) {
        log_message(LOG_ERROR, "Failed to create netns symlink");
        return -1;
    }
    
    return 0;
}

int create_veth_pair(const char *veth_host, const char *veth_container) {
    char cmd[256];
    
    if (!veth_host || !veth_container) {
        log_message(LOG_ERROR, "Invalid veth pair names");
        return -1;
    }
    
    log_message(LOG_DEBUG, "Creating veth pair: %s <-> %s", veth_host, veth_container);
    
    // Create veth pair using ip command
    snprintf(cmd, sizeof(cmd), 
             "ip link add %s type veth peer name %s", veth_host, veth_container);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to create veth pair");
        return -1;
    }
    
    return 0;
}

int setup_bridge() {
    char cmd[256];
    
    log_message(LOG_DEBUG, "Setting up bridge: %s", BRIDGE_NAME);
    
    // Create bridge if it doesn't exist
    snprintf(cmd, sizeof(cmd), "ip link show %s 2>/dev/null", BRIDGE_NAME);
    if (system(cmd) != 0) {
        // Create bridge
        snprintf(cmd, sizeof(cmd), "ip link add name %s type bridge", BRIDGE_NAME);
        if (system(cmd) != 0) {
            log_message(LOG_ERROR, "Failed to create bridge");
            return -1;
        }
        
        // Set bridge IP
        snprintf(cmd, sizeof(cmd), 
                 "ip addr add 172.17.0.1/16 dev %s", BRIDGE_NAME);
        if (system(cmd) != 0) {
            log_message(LOG_ERROR, "Failed to set bridge IP");
            return -1;
        }
        
        // Enable bridge
        snprintf(cmd, sizeof(cmd), "ip link set %s up", BRIDGE_NAME);
        if (system(cmd) != 0) {
            log_message(LOG_ERROR, "Failed to enable bridge");
            return -1;
        }
        
        // Enable IP forwarding
        if (system("echo 1 > /proc/sys/net/ipv4/ip_forward") != 0) {
            log_message(LOG_WARN, "Failed to enable IP forwarding");
        }
        
        // Setup NAT
        snprintf(cmd, sizeof(cmd), 
                 "iptables -t nat -A POSTROUTING -s 172.17.0.0/16 ! -o %s -j MASQUERADE",
                 BRIDGE_NAME);
        if (system(cmd) != 0) {
            log_message(LOG_WARN, "Failed to setup NAT");
        }
    }
    
    return 0;
}

int configure_container_network(pid_t pid, const char *veth_host, const char *veth_container) {
    char cmd[256];
    
    log_message(LOG_DEBUG, "Configuring container network for PID %d", pid);
    
    // Move container veth to container namespace
    snprintf(cmd, sizeof(cmd), 
             "ip link set %s netns %d", veth_container, pid);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to move veth to container namespace");
        return -1;
    }
    
    // Connect host veth to bridge
    snprintf(cmd, sizeof(cmd), 
             "ip link set %s master %s", veth_host, BRIDGE_NAME);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to connect veth to bridge");
        return -1;
    }
    
    // Set host veth up
    snprintf(cmd, sizeof(cmd), "ip link set %s up", veth_host);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to enable host veth");
        return -1;
    }
    
    // Configure container network
    snprintf(cmd, sizeof(cmd), 
             "ip netns exec %d ip addr add 172.17.0.%d/16 dev %s",
             pid, ip_counter++, veth_container);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to set container IP");
        return -1;
    }
    
    // Set container interface up
    snprintf(cmd, sizeof(cmd), 
             "ip netns exec %d ip link set %s up", pid, veth_container);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to enable container interface");
        return -1;
    }
    
    // Set container loopback up
    snprintf(cmd, sizeof(cmd), 
             "ip netns exec %d ip link set lo up", pid);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to enable container loopback");
        return -1;
    }
    
    // Add default route in container
    snprintf(cmd, sizeof(cmd), 
             "ip netns exec %d ip route add default via 172.17.0.1", pid);
    if (system(cmd) != 0) {
        log_message(LOG_ERROR, "Failed to set container default route");
        return -1;
    }
    
    return 0;
}

int cleanup_container_network(pid_t pid) {
    char netns_path[PATH_MAX];
    char cmd[256];
    
    log_message(LOG_DEBUG, "Cleaning up network for PID %d", pid);
    
    // Remove veth pair (will be auto-removed when namespace is deleted)
    snprintf(cmd, sizeof(cmd), 
             "ip link delete veth%d 2>/dev/null", pid);
    system(cmd); // Ignore errors as it might already be deleted
    
    // Remove netns symlink
    snprintf(netns_path, sizeof(netns_path), "%s/%d", CONTAINER_NETNS_PATH, pid);
    unlink(netns_path); // Ignore errors
    
    return 0;
}