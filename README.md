# Mini Docker

A lightweight container runtime written in C that helps beginners understand the core concepts behind Docker and container technology. This educational project implements basic container features using Linux namespaces, making it easier to grasp how Docker works under the hood.

## Overview

Mini Docker is designed as a learning tool that demonstrates:
- How containers achieve process isolation using Linux namespaces
- How resource limits are implemented using cgroups
- How container networking works with network namespaces
- Basic container lifecycle management
- Fundamental Docker-like commands

This project is intentionally simplified compared to Docker to focus on core concepts that are essential for understanding container technology.

## Features

### Core Features
1. **Container Isolation** 
   - Process isolation using Linux namespaces (PID, UTS, Mount, IPC)
   - Resource limits with cgroups v2 (CPU and memory)
   - Filesystem isolation using chroot/pivot_root
   - Network namespace isolation

2. **Container Management**
   - Create and run containers (`run` command)
   - List running containers (`ps` command)
   - Stop containers gracefully (`stop` command)
   - Clean container lifecycle management

3. **Resource Control**
   - CPU usage limits
   - Memory limits
   - Process isolation
   - Basic network isolation

4. **Container Registry**
   - Track running containers
   - Persist container information
   - Display container status and metadata

### Commands
1. `run [PATH] [COMMAND]`
   - Creates and starts a new container
   - Example: `sudo ./minidocker run /bin/bash /bin/bash`
   - Options for CPU and memory limits

2. `ps`
   - Lists all running containers
   - Shows container ID, PID, status, and command
   - Displays resource usage statistics

3. `stop [CONTAINER_PID]`
   - Gracefully stops a running container
   - Handles cleanup of resources
   - Example: `sudo ./minidocker stop 1234`

4. `help`
   - Displays usage information
   - Lists available commands
   - Shows command options

### Future Scope
As this is an educational tool for Docker beginners, future enhancements could include:
- Enhanced visualization of container states
- Step-by-step container creation process display
- Resource usage monitoring and graphs
- Interactive tutorial mode
- Better error messages with learning points
- Documentation improvements for learning

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

#### ✅ Completed Features
- [x] Complete CLI interface (run, stop, ps, help commands)
- [x] Project structure and build system
- [x] Namespace creation (PID, UTS, Mount, IPC, Network)
- [x] Cgroups v2 integration (memory and CPU limits)
- [x] Basic container process management
- [x] Process isolation with clone() syscall
- [x] Logging and utility functions
- [x] Root privilege validation

####  Partially Implemented
- [~] Container stopping (basic signal handling implemented)
- [~] Filesystem isolation (structure exists, chroot/pivot_root not integrated)
- [~] Network namespace setup (basic structure, no veth/bridge config)

####  Not Yet Implemented
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
