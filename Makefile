# Export MM so it's available to sub-makes
export MM

# Default target to build everything
all: bootloader kernel userland image

# Build the bootloader component
bootloader:
	cd Bootloader; make all

# Build the kernel with memory manager configuration
kernel:
	cd Kernel; make all MM=$(MM)

# Build userland applications
userland:
	cd Userland; make all

# Create system image after building all components
image: kernel bootloader userland
	cd Image; make all

# Clean all build artifacts
clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean