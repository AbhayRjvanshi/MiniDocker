#ifndef CONTAINER_H
#define CONTAINER_H

#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <time.h>

// Container configuration
typedef struct {
    char *image_path;    // Path to container root filesystem
    char *command;       // Command to run in container
    char **args;        // Command arguments
    int cpu_limit;      // CPU limit in shares (relative weight)
    int memory_limit;   // Memory limit in bytes
    pid_t pid;         // Container process ID
    char *id;          // Container unique identifier
    time_t created_at; // Creation timestamp
    char *status;      // Container status (created, running, stopped, exited)
} container_t;

// Function declarations
int create_container(container_t *container);
int start_container(container_t *container);
int stop_container(pid_t pid);
int list_containers(void);
int container_init(void *arg);

// Internal helper functions
static int cleanup_container_resources(pid_t pid);

#endif