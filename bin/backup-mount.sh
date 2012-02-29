. `dirname $0`/backup-common.sh
echo "mount $DISK_DEVICE $DISK_MOUNT_POINT"
mount $DISK_DEVICE $DISK_MOUNT_POINT
if [ ! -f "$IMG" ]; then
	echo "wrong disk, image not found"
	exit 1
fi
echo "losetup $LOOP_DEVICE $IMG"
losetup $LOOP_DEVICE $IMG
echo "cryptsetup create $ENCRYPTED_DEVICE $LOOP_DEVICE"
cryptsetup create $ENCRYPTED_DEVICE $LOOP_DEVICE
echo "mount -t btrfs /dev/mapper/$ENCRYPTED_DEVICE $BTRFS_MOUNT_POINT"
mount -t btrfs /dev/mapper/$ENCRYPTED_DEVICE $BTRFS_MOUNT_POINT
if [ "$?" -eq 0 ]; then
	touch $FLAG
fi
