#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <linux/limits.h>
#include <sys/types.h>
#include <sys/sysmacros.h>

// Function declarations
int setup_filesystem(const char *rootfs);
int mount_container_fs(void);
int cleanup_filesystem(const char *container_root);

// Helper functions
int setup_rootfs(const char *new_root);
int mount_proc(void);
int mount_sys(void);
int setup_chroot(const char *new_root);
int setup_pivot_root(const char *new_root, const char *old_root);

#endif