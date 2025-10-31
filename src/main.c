#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "container.h"
#include "utils.h"

void print_usage(const char *prog_name) {
    printf("Usage: %s <command> [options]\n", prog_name);
    printf("Commands:\n");
    printf("  run <image> <command>    Run a new container\n");
    printf("  stop <container_id>      Stop a running container\n");
    printf("  ps                       List running containers\n");
    printf("  help                     Show this help message\n");
}

int cmd_run(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: minidocker run <image> <command>\n");
        return 1;
    }

    // Validate arguments
    if (!argv[2] || !argv[3]) {
        fprintf(stderr, "Error: Invalid arguments\n");
        return 1;
    }
    
    container_t container = {0};
    container.image_path = argv[2];
    container.command = argv[3];
    container.args = &argv[3];
    container.cpu_limit = 100;  // Default CPU shares
    container.memory_limit = 128 * 1024 * 1024;  // Default 128MB

    log_message(LOG_INFO, "Creating container with image: %s", container.image_path);
    
    int result = create_container(&container);
    if (result != 0) {
        log_message(LOG_ERROR, "Failed to create container");
    }
    return result;
}

int cmd_stop(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: minidocker stop <container_id>\n");
        return 1;
    }

    if (!argv[2]) {
        fprintf(stderr, "Error: Invalid container ID\n");
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[2]);
    if (pid <= 0) {
        fprintf(stderr, "Error: Invalid PID: %s\n", argv[2]);
        return 1;
    }
    
    log_message(LOG_INFO, "Stopping container with PID: %d", (int)pid);
    
    // TODO: Implement container stopping
    return stop_container(pid);
}

int cmd_ps(void) {
    log_message(LOG_INFO, "Listing running containers");
    
    // TODO: Implement container listing
    return list_containers();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (getuid() != 0) {
        fprintf(stderr, "Error: minidocker must be run as root\n");
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "run") == 0) {
        return cmd_run(argc, argv);
    } else if (strcmp(command, "stop") == 0) {
        return cmd_stop(argc, argv);
    } else if (strcmp(command, "ps") == 0) {
        return cmd_ps();
    } else if (strcmp(command, "help") == 0) {
        print_usage(argv[0]);
        return 0;
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        print_usage(argv[0]);
        return 1;
    }
}