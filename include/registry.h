#ifndef REGISTRY_H
#define REGISTRY_H

#include "container.h"
#include <sys/types.h>

// Function declarations
int registry_add_container(container_t *container);
int registry_update_container_status(pid_t pid, const char *status);
void registry_list_containers(void);

#endif