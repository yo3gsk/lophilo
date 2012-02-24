. `dirname $0`/backup-common.sh
if [ ! -f "$FLAG" ]; then
	echo "Backup disk not mounted, exiting"
	exit
fi
echo "rsync -av --delete-excluded --include-from=$BACKUP_LST $BTRFS_MOUNT_POINT /"
rsync -av --delete-excluded --include-from=$BACKUP_LST --exclude=backup/ / $BTRFS_MOUNT_POINT
touch $FLAG
