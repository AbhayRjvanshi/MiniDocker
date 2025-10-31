#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// Function declarations
int setup_rootfs(const char *new_root);
int mount_proc(void);
int mount_sys(void);
int setup_chroot(const char *new_root);
int setup_pivot_root(const char *new_root, const char *old_root);

#endif