. `dirname $0`/backup-common.sh
umount $BTRFS_MOUNT_POINT
cryptsetup remove $ENCRYPTED_DEVICE
losetup -d $LOOP_DEVICE
umount $DISK_MOUNT_POINT
