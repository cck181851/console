#!/bin/bash

# Create the mount directory if it doesn't exist
if [ ! -d "mount" ]; then
    echo "Creating mount directory..."
    sudo mkdir -p mount
fi

# Mount the disk image to the mount directory
echo "Mounting the disk image..."
sudo mount -o loop storage_vgc.img mount

# Create a device file for the virtual disk device
echo "Creating device file /dev/vgc_device..."
sudo mknod /dev/vgc_device b 7 1  # Adjust this based on your specific device
sudo chmod 666 /dev/vgc_device

# Create a symbolic link for the device file in the current directory
echo "Creating symbolic link for device file..."
sudo ln -s /dev/vgc_device vgc_device

# Copy game executables from bin to the mounted disk
echo "Copying executables to the mounted disk..."
sudo cp bin/* mount/

# Disk image is mounted and executables copied
echo "Disk image mounted and executables copied successfully."
