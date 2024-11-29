#!/bin/bash

# Run terminate.sh to clean up everything
./terminate.sh

# Delete the disk image file
if [ -f storage_vgc.img ]; then
    echo "Removing the disk image..."
    sudo rm storage_vgc.img
else
    echo "Disk image not found."
fi

echo "All files and the disk image have been purged."
