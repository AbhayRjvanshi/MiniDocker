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
    
    // TODO: Setup filesystem isolation (chroot/pivot_root)
    log_message(LOG_DEBUG, "Setting up filesystem isolation");
    
    // TODO: Mount /proc and /sys
    log_message(LOG_DEBUG, "Mounting proc and sys");
    
    // TODO: Execute the container command
    log_message(LOG_INFO, "Executing command: %s", container->command);
    
    execvp(container->command, container->args);
    die("execvp failed");
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
    
    // TODO: Add PID to cgroup
    // TODO: Wait for container or return immediately based on requirements
    
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
    // TODO: Send SIGTERM, wait, then SIGKILL if necessary
    log_message(LOG_INFO, "Stopping container with PID: %d", (int)pid);
    
    if (kill(pid, SIGTERM) == -1) {
        log_message(LOG_ERROR, "Failed to send SIGTERM to PID %d", (int)pid);
        return -1;
    }
    
    // Wait for graceful shutdown
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        log_message(LOG_DEBUG, "Container still running, waiting...");
        // TODO: Implement timeout and force kill if needed
    } else if (result == -1) {
        log_message(LOG_ERROR, "Failed to wait for container PID %d", (int)pid);
        return -1;
    }
    
    return 0;
}

int list_containers(void) {
    // TODO: Read from /proc or maintain container registry
    printf("CONTAINER ID\tIMAGE\t\tCOMMAND\t\tSTATUS\n");
    printf("(No containers running)\n");
    return 0;
}