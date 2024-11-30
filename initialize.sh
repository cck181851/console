#!/bin/bash

# Remove existing disk image if it exists
echo "Checking for existing disk image..."
if [ -f storage_vgc.img ]; then
    echo "Existing disk image found. Removing it..."
    sudo rm -f storage_vgc.img
else
    echo "No existing disk image found."
fi

# Create the disk image file (52MB size)
echo "Creating a new disk image (52MB)..."
dd if=/dev/zero of=storage_vgc.img bs=1M count=52 status=progress

# Format the disk image as ext4
echo "Formatting the new disk image as ext4..."
sudo mke2fs -t ext4 storage_vgc.img

# Create the mount directory if it doesn't exist
echo "Ensuring the mount directory exists..."
sudo mkdir -p mount

# Create the bin directory
echo "Ensuring the bin directory exists..."
sudo mkdir -p bin

# Compile the source files into executables and place them in the bin directory
echo "Compiling source files into executables..."
sudo gcc -o bin/game_snake src/src1.c
sudo gcc -o bin/game_sudoku src/src2.c
sudo gcc -o bin/game_save_the_princess src/src3.c
sudo gcc -o bin/main-screen src/main-screen.c

# Create a symbolic link for the device file
echo "Creating a symbolic link for the virtual device file..."
sudo ln -sf /dev/vgc_device vgc_device

# Display completion message
echo "Disk image has been created and executables compiled successfully."
