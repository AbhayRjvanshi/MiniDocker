#ifndef CGROUP_H
#define CGROUP_H

#include <sys/types.h>

// Function declarations
int setup_cgroup(const char *cgroup_name);
int set_memory_limit(const char *cgroup_name, long memory_bytes);
int set_cpu_limit(const char *cgroup_name, int cpu_shares);
int add_pid_to_cgroup(const char *cgroup_name, pid_t pid);
int cleanup_cgroup(const char *cgroup_name);

#endif