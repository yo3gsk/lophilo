. `dirname $0`/backup-common.sh
if [ ! -f "$IMG" ]; then
	echo "not ready"
	exit 1
fi
echo "dd if=/dev/zero of=$IMG bs=1M count=500 conv=notrunc oflag=append"
dd if=/dev/zero of=$IMG bs=1M count=500 conv=notrunc oflag=append
sudo cryptsetup resize $ENCRYPTED_DEVICE
echo "btrfs filesystem resize max $BTRFS_MOUNT_POINT"
btrfs filesystem resize max $BTRFS_MOUNT_POINT
