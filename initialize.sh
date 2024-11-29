#!/bin/bash

# Remove existing disk image if it exists
echo "Removing existing disk image..."
sudo rm -f storage_vgc.img

# Create the disk image file (52MB size, adjust if needed)
echo "Creating disk image..."
dd if=/dev/zero of=storage_vgc.img bs=1M count=52 status=progress

# Format the disk image as ext4
echo "Formatting disk image as ext4..."
sudo mke2fs -t ext4 storage_vgc.img

# Create the mount directory if it doesn't exist
echo "Creating mount directory..."
sudo mkdir -p mount

# Compile the game and main-screen executables and place them in the bin directory
echo "Compiling source files..."
gcc -o bin/game_1 src/src1.c
gcc -o bin/game_2 src/src2.c
gcc -o bin/game_3 src/src3.c
gcc -o bin/main-screen src/main-screen.c

# Create a symbolic link for the device file
echo "Creating symbolic link for device file..."
sudo ln -s /dev/vgc_device vgc_device

# Disk image setup is complete
echo "Disk image created and executables compiled successfully."
