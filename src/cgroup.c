#include "cgroup.h"
#include "utils.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define CGROUP_ROOT "/sys/fs/cgroup"

int setup_cgroup(const char *cgroup_name) {
    // TODO: Create cgroup directory structure
    if (!cgroup_name || strlen(cgroup_name) == 0 || strchr(cgroup_name, '/') || strstr(cgroup_name, "..")) {
        log_message(LOG_ERROR, "Invalid cgroup name");
        return -1;
    }
    log_message(LOG_DEBUG, "Setting up cgroup: %s", cgroup_name);
    
    char path[512];
    int ret = snprintf(path, sizeof(path), "%s/%s", CGROUP_ROOT, cgroup_name);
    if (ret >= (int)sizeof(path) || ret < 0) {
        log_message(LOG_ERROR, "Cgroup path too long");
        return -1;
    }
    
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        log_message(LOG_ERROR, "Failed to create cgroup directory: %s", path);
        return -1;
    }
    
    return 0;
}

int set_memory_limit(const char *cgroup_name, long memory_bytes) {
    // TODO: Set memory limit in cgroup
    if (!cgroup_name || memory_bytes <= 0 || strchr(cgroup_name, '/') || strstr(cgroup_name, "..")) {
        log_message(LOG_ERROR, "Invalid parameters for memory limit");
        return -1;
    }
    log_message(LOG_DEBUG, "Setting memory limit: %ld bytes", memory_bytes);
    
    char path[512];
    int ret = snprintf(path, sizeof(path), "%s/%s/memory.max", CGROUP_ROOT, cgroup_name);
    if (ret >= (int)sizeof(path) || ret < 0) {
        log_message(LOG_ERROR, "Memory limit path too long");
        return -1;
    }
    
    FILE *file = fopen(path, "w");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open memory.max file: %s", path);
        return -1;
    }
    
    fprintf(file, "%ld", memory_bytes);
    fclose(file);
    
    return 0;
}

int set_cpu_limit(const char *cgroup_name, int cpu_shares) {
    // TODO: Set CPU limit in cgroup
    if (!cgroup_name || cpu_shares <= 0 || strchr(cgroup_name, '/') || strstr(cgroup_name, "..")) {
        log_message(LOG_ERROR, "Invalid parameters for CPU limit");
        return -1;
    }
    log_message(LOG_DEBUG, "Setting CPU shares: %d", cpu_shares);
    
    char path[512];
    int ret = snprintf(path, sizeof(path), "%s/%s/cpu.weight", CGROUP_ROOT, cgroup_name);
    if (ret >= (int)sizeof(path) || ret < 0) {
        log_message(LOG_ERROR, "CPU limit path too long");
        return -1;
    }
    
    FILE *file = fopen(path, "w");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open cpu.weight file: %s", path);
        return -1;
    }
    
    fprintf(file, "%d", cpu_shares);
    fclose(file);
    
    return 0;
}

int add_pid_to_cgroup(const char *cgroup_name, pid_t pid) {
    // TODO: Add process PID to cgroup
    if (!cgroup_name || pid <= 0 || strchr(cgroup_name, '/') || strstr(cgroup_name, "..")) {
        log_message(LOG_ERROR, "Invalid parameters for adding PID to cgroup");
        return -1;
    }
    log_message(LOG_DEBUG, "Adding PID %d to cgroup %s", (int)pid, cgroup_name);
    
    char path[512];
    int ret = snprintf(path, sizeof(path), "%s/%s/cgroup.procs", CGROUP_ROOT, cgroup_name);
    if (ret >= (int)sizeof(path) || ret < 0) {
        log_message(LOG_ERROR, "Cgroup procs path too long");
        return -1;
    }
    
    FILE *file = fopen(path, "w");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open cgroup.procs file: %s", path);
        return -1;
    }
    
    fprintf(file, "%d", (int)pid);
    fclose(file);
    
    return 0;
}

int cleanup_cgroup(const char *cgroup_name) {
    // TODO: Remove cgroup directory
    if (!cgroup_name || strchr(cgroup_name, '/') || strstr(cgroup_name, "..")) {
        log_message(LOG_ERROR, "Invalid cgroup name for cleanup");
        return -1;
    }
    log_message(LOG_DEBUG, "Cleaning up cgroup: %s", cgroup_name);
    
    char path[512];
    int ret = snprintf(path, sizeof(path), "%s/%s", CGROUP_ROOT, cgroup_name);
    if (ret >= (int)sizeof(path) || ret < 0) {
        log_message(LOG_ERROR, "Cleanup path too long");
        return -1;
    }
    
    if (rmdir(path) == -1) {
        log_message(LOG_ERROR, "Failed to remove cgroup directory: %s", path);
        return -1;
    }
    
    return 0;
}