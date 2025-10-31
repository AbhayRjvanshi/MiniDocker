#include "container.h"
#include "filesystem.h"
#include "cgroup.h"
#include "utils.h"
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>

#define STACK_SIZE (1024 * 1024)

int container_init(void *arg) {
    container_t *container = (container_t *)arg;
    
    if (!container || !container->command) {
        log_message(LOG_ERROR, "Invalid container configuration");
        return 1;
    }
    
    // Setup filesystem isolation
    log_message(LOG_DEBUG, "Setting up filesystem isolation");
    if (setup_filesystem(container->image_path) != 0) {
        log_message(LOG_ERROR, "Failed to setup filesystem isolation");
        return 1;
    }
    
    // Set hostname
    char hostname[256];
    snprintf(hostname, sizeof(hostname), "minidocker-%d", (int)getpid());
    if (sethostname(hostname, strlen(hostname)) != 0) {
        log_message(LOG_WARN, "Failed to set hostname");
    }
    
    // Drop capabilities (optional, uncomment if needed)
    // if (drop_capabilities() != 0) {
    //     log_message(LOG_WARN, "Failed to drop capabilities");
    // }
    
    log_message(LOG_INFO, "Executing command: %s", container->command);
    execvp(container->command, container->args);
    
    log_message(LOG_ERROR, "execvp failed: %s", strerror(errno));
    return 1;
}

int create_container(container_t *container) {
    log_message(LOG_INFO, "Creating new container");
    
    // TODO: Setup cgroups for resource limits
    char cgroup_name[256];
    int ret = snprintf(cgroup_name, sizeof(cgroup_name), "minidocker_%d", (int)getpid());
    if (ret >= (int)sizeof(cgroup_name) || ret < 0) {
        log_message(LOG_ERROR, "Cgroup name too long");
        return -1;
    }
    
    // TODO: Create namespaces (PID, UTS, Mount, IPC, Network)
    int flags = CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | 
                CLONE_NEWIPC | CLONE_NEWNET;
    
    // Setup cgroups first
    if (setup_cgroup(cgroup_name) != 0) {
        log_message(LOG_ERROR, "Failed to setup cgroup");
        return -1;
    }
    
    // Setup network bridge
    if (setup_bridge() != 0) {
        log_message(LOG_ERROR, "Failed to setup network bridge");
        cleanup_cgroup(cgroup_name);
        return -1;
    }
    
    // Allocate stack for child process
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        log_message(LOG_ERROR, "Failed to allocate stack memory");
        cleanup_cgroup(cgroup_name);
        return -1;
    }
    
    // Create child process with namespaces
    pid_t pid = clone(container_init, stack + STACK_SIZE, flags | SIGCHLD, container);
    if (pid == -1) {
        log_message(LOG_ERROR, "Failed to create container process");
        free(stack);
        cleanup_cgroup(cgroup_name);
        return -1;
    }
    
    // Add PID to cgroup
    if (add_pid_to_cgroup(cgroup_name, pid) != 0) {
        log_message(LOG_WARN, "Failed to add PID to cgroup");
    }
    
    // Set resource limits
    if (container->memory_limit > 0) {
        set_memory_limit(cgroup_name, container->memory_limit);
    }
    if (container->cpu_limit > 0) {
        set_cpu_limit(cgroup_name, container->cpu_limit);
    }
    
    // Wait for child to start before returning
    int status;
    pid_t wait_result = waitpid(pid, &status, WNOHANG);
    if (wait_result == -1) {
        log_message(LOG_WARN, "Failed to check child process status");
    }
    
    // Note: Stack memory will be cleaned up when child exits
    // In production, implement proper resource tracking
    
    container->pid = pid;
    log_message(LOG_INFO, "Container created with PID: %d", (int)pid);
    
    // Setup network for container
    if (setup_network_namespace(pid) == 0) {
        char veth_host[32], veth_container[32];
        snprintf(veth_host, sizeof(veth_host), "veth%dh", pid);
        snprintf(veth_container, sizeof(veth_container), "veth%dc", pid);
        
        if (create_veth_pair(veth_host, veth_container) == 0) {
            if (configure_container_network(pid, veth_host, veth_container) != 0) {
                log_message(LOG_WARN, "Failed to configure container network");
            }
        } else {
            log_message(LOG_WARN, "Failed to create veth pair");
        }
    } else {
        log_message(LOG_WARN, "Failed to setup network namespace");
    }
    
    // Add container to registry
    if (registry_add_container(container) != 0) {
        log_message(LOG_WARN, "Failed to add container to registry");
    }
    
    return 0;
}

int start_container(container_t *container) {
    // TODO: Implement container start logic
    if (!container) {
        log_message(LOG_ERROR, "Invalid container");
        return -1;
    }
    log_message(LOG_INFO, "Starting container with PID: %d", (int)container->pid);
    return 0;
}

int stop_container(pid_t pid) {
    log_message(LOG_INFO, "Stopping container with PID: %d", (int)pid);
    
    // First try graceful shutdown with SIGTERM
    if (kill(pid, SIGTERM) == -1) {
        log_message(LOG_ERROR, "Failed to send SIGTERM to PID %d", (int)pid);
        return -1;
    }
    
    // Wait up to 10 seconds for graceful shutdown
    time_t start_time = time(NULL);
    int status;
    while (time(NULL) - start_time < 10) {
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) {
            log_message(LOG_INFO, "Container stopped gracefully");
            registry_update_container_status(pid, "stopped");
            cleanup_container_resources(pid);
            return 0;
        } else if (result == -1) {
            log_message(LOG_ERROR, "Failed to wait for container PID %d", (int)pid);
            return -1;
        }
        usleep(100000); // Sleep for 100ms
    }
    
    // If still running after timeout, force kill
    log_message(LOG_WARN, "Container didn't stop gracefully, forcing kill");
    if (kill(pid, SIGKILL) == -1) {
        log_message(LOG_ERROR, "Failed to send SIGKILL to PID %d", (int)pid);
        return -1;
    }
    
    // Wait for the forced kill
    if (waitpid(pid, &status, 0) == -1) {
        log_message(LOG_ERROR, "Failed to wait for container after SIGKILL");
        return -1;
    }
    
    registry_update_container_status(pid, "stopped");
    cleanup_container_resources(pid);
    return 0;
}

static int cleanup_container_resources(pid_t pid) {
    char cgroup_path[256];
    char container_root[PATH_MAX];
    
    // Clean up cgroups
    snprintf(cgroup_path, sizeof(cgroup_path), "minidocker_%d", (int)pid);
    cleanup_cgroup(cgroup_path);
    
    // Clean up network
    cleanup_container_network(pid);
    
    // Clean up filesystem
    snprintf(container_root, sizeof(container_root),
             "/var/lib/minidocker/containers/%d", (int)pid);
    cleanup_filesystem(container_root);
    
    return 0;
}

int list_containers(void) {
    registry_list_containers();
    return 0;
}