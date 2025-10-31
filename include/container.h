#ifndef CONTAINER_H
#define CONTAINER_H

#include <sys/types.h>
#include <sched.h>
#include <signal.h>

// Container configuration
typedef struct {
    char *image_path;
    char *command;
    char **args;
    int cpu_limit;
    int memory_limit;
    pid_t pid;
} container_t;

// Function declarations
int create_container(container_t *container);
int start_container(container_t *container);
int stop_container(pid_t pid);
int list_containers(void);
int container_init(void *arg);

#endif