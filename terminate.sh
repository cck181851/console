#!/bin/bash

# Unmount the mounted disk image
if mount | grep "mount" > /dev/null; then
    echo "Unmounting the disk image..."
    sudo umount mount
else
    echo "Disk image is not mounted."
fi

# Remove the device file and symbolic link if it exists
if [ -L vgc_device ]; then
    echo "Removing symbolic link for the device file..."
    sudo rm vgc_device
else
    echo "Symbolic link for device file not found."
fi

# Optionally, remove the files in the mount directory (not the disk image)
if [ -d mount ]; then
    echo "Cleaning up mount directory..."
    sudo rm -rf mount/*
else
    echo "Mount directory not found."
fi

echo "Termination process completed."
