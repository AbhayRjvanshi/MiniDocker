# Mini Docker
A project emphasizing the need to understand the basics of technology like Desktop Docker(which is much complex to learn as a beginner). The project emphasizes the learning of basics commands of Docker(via namespaces).
A lightweight container runtime written in C, designed to run entirely in the Linux terminal.

## Features

### Working Features
- **Process Isolation**: Uses Linux namespaces (PID, UTS, Mount, IPC, Network)
- **Resource Limits**: CPU and memory control with cgroups v2
- **Container Lifecycle**: Create and run containers with process isolation
- **CLI Interface**: Complete terminal-based commands (run, stop, ps, help)
- **Logging System**: Structured logging with different levels
- **Security**: Root privilege validation and input sanitization

### In Development
- **Filesystem Isolation**: Basic structure implemented, chroot integration pending
- **Container Management**: Stop command with signal handling
- **Network Isolation**: Namespace creation working, configuration pending

### Planned Features
- **Container Registry**: Persistent container tracking and listing
- **Advanced Networking**: veth pairs, bridge configuration, IP assignment
- **Image Management**: Layered filesystem support
- **Enhanced Lifecycle**: Pause, restart, and advanced container operations

## Prerequisites

### System Requirements
- Linux kernel ≥ 5.x (WSL 2 Ubuntu supported)
- Root privileges required for namespace operations

### Dependencies
```bash
sudo apt update
sudo apt install -y build-essential libcap-dev libseccomp-dev pkg-config git
```

## Building

```bash
# Build the project
make

# Clean build artifacts
make clean

# Build with debug symbols
make debug
```

## Usage

### Run a Container
```bash
sudo ./minidocker run /bin/bash /bin/bash
```

### List Running Containers
```bash
sudo ./minidocker ps
```

### Stop a Container
```bash
sudo ./minidocker stop <container_pid>
```

### Help
```bash
./minidocker help
```

## Development

### Project Structure
```
mini docker/
├── src/                 # Source files
│   ├── main.c          # CLI entry point
│   ├── container.c     # Container lifecycle management
│   ├── filesystem.c    # Filesystem isolation
│   ├── cgroup.c        # Resource limits
│   ├── network.c       # Network namespace setup
│   └── utils.c         # Logging and utilities
├── include/            # Header files
├── Makefile           # Build configuration
└── .vscode/           # VS Code tasks
```

### Implementation Status

#### Completed Features
- [x] Complete CLI interface (run, stop, ps, help commands)
- [x] Project structure and build system
- [x] Namespace creation (PID, UTS, Mount, IPC, Network)
- [x] Cgroups v2 integration (memory and CPU limits)
- [x] Basic container process management
- [x] Process isolation with clone() syscall
- [x] Logging and utility functions
- [x] Root privilege validation

#### Partially Implemented
- [~] Container stopping (basic signal handling implemented)
- [~] Filesystem isolation (structure exists, chroot/pivot_root not integrated)
- [~] Network namespace setup (basic structure, no veth/bridge config)

#### Not Yet Implemented
- [ ] Container registry/listing (shows placeholder)
- [ ] Complete filesystem isolation (proc/sys mounting)
- [ ] Network configuration (veth pairs, bridges, IP assignment)
- [ ] Container persistence and state management
- [ ] Advanced container lifecycle (pause, restart)
- [ ] Image management and layered filesystems

### Testing in WSL 2

1. Open WSL 2 terminal
2. Navigate to project directory
3. Build: `make`
4. Test: `sudo ./minidocker run /bin/bash /bin/bash`

### Current Limitations

- **Container Listing**: `ps` command shows placeholder output
- **Filesystem Isolation**: Containers share host filesystem (chroot not integrated)
- **Network Configuration**: No network connectivity setup for containers
- **Container Persistence**: No state tracking between runs
- **Error Handling**: Limited error recovery and cleanup

### Known Issues

- Container processes may not be properly cleaned up on exit
- Memory limits require cgroups v2 to be enabled on the system
- Network namespace isolation is created but not configured
- Stop command may not work reliably with all container states

## Security Notes

- Requires root privileges for namespace operations
- Uses Linux capabilities for privilege management
- Implements secure filesystem isolation
- Resource limits prevent container resource abuse

## License

MIT License - see LICENSE file for details
