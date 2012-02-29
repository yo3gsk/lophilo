# http://bellot.net/cyril/contribs/btrfsloop
BASEDIR=`dirname $0`
DISK_DEVICE=/dev/sdb1
DISK_MOUNT_POINT=/home/rngadam/backup/disk
LOOP_DEVICE=/dev/loop1
ENCRYPTED_DEVICE=dsk1
BTRFS_MOUNT_POINT=/home/rngadam/backup/btrfs
IMG=$DISK_MOUNT_POINT/lophilo-dev-station-backup.img
BACKUP_LST=$BASEDIR/backup.lst
FLAG=$BTRFS_MOUNT_POINT/BACKUP-FLAG
