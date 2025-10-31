#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <json-c/json.h>
#include "../include/registry.h"

#define REGISTRY_FILE "/var/lib/minidocker/containers.json"
#define REGISTRY_DIR "/var/lib/minidocker"

static void ensure_registry_dir() {
    mkdir(REGISTRY_DIR, 0755);
}

int registry_add_container(container_t *container) {
    ensure_registry_dir();
    
    struct json_object *root = json_object_new_object();
    struct json_object *containers;
    
    // Read existing registry
    FILE *f = fopen(REGISTRY_FILE, "r");
    if (f) {
        char buffer[4096];
        fread(buffer, 1, sizeof(buffer), f);
        fclose(f);
        root = json_tokener_parse(buffer);
        containers = json_object_object_get(root, "containers");
    } else {
        containers = json_object_new_array();
    }
    
    // Create new container entry
    struct json_object *cont = json_object_new_object();
    json_object_object_add(cont, "pid", json_object_new_int(container->pid));
    json_object_object_add(cont, "image_path", json_object_new_string(container->image_path));
    json_object_object_add(cont, "command", json_object_new_string(container->command));
    json_object_object_add(cont, "created_at", json_object_new_int64(time(NULL)));
    json_object_object_add(cont, "status", json_object_new_string("running"));
    
    // Add to array and save
    json_object_array_add(containers, cont);
    json_object_object_add(root, "containers", containers);
    
    f = fopen(REGISTRY_FILE, "w");
    if (!f) return -1;
    fputs(json_object_to_json_string(root), f);
    fclose(f);
    
    return 0;
}

int registry_update_container_status(pid_t pid, const char *status) {
    ensure_registry_dir();
    
    struct json_object *root;
    FILE *f = fopen(REGISTRY_FILE, "r");
    if (!f) return -1;
    
    char buffer[4096];
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    
    root = json_tokener_parse(buffer);
    struct json_object *containers = json_object_object_get(root, "containers");
    
    int i;
    for (i = 0; i < json_object_array_length(containers); i++) {
        struct json_object *cont = json_object_array_get_idx(containers, i);
        struct json_object *cont_pid;
        json_object_object_get_ex(cont, "pid", &cont_pid);
        
        if (json_object_get_int(cont_pid) == pid) {
            json_object_object_add(cont, "status", json_object_new_string(status));
            break;
        }
    }
    
    f = fopen(REGISTRY_FILE, "w");
    if (!f) return -1;
    fputs(json_object_to_json_string(root), f);
    fclose(f);
    
    return 0;
}

void registry_list_containers() {
    FILE *f = fopen(REGISTRY_FILE, "r");
    if (!f) {
        printf("No containers found\n");
        return;
    }
    
    char buffer[4096];
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    
    struct json_object *root = json_tokener_parse(buffer);
    struct json_object *containers = json_object_object_get(root, "containers");
    
    printf("CONTAINER ID\tSTATUS\t\tCOMMAND\n");
    
    int i;
    for (i = 0; i < json_object_array_length(containers); i++) {
        struct json_object *cont = json_object_array_get_idx(containers, i);
        struct json_object *pid, *status, *command;
        
        json_object_object_get_ex(cont, "pid", &pid);
        json_object_object_get_ex(cont, "status", &status);
        json_object_object_get_ex(cont, "command", &command);
        
        printf("%d\t\t%s\t\t%s\n",
               json_object_get_int(pid),
               json_object_get_string(status),
               json_object_get_string(command));
    }
}