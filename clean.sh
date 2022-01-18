set -e

echo "卸载缺页计数模块"
sudo rmmod readpfcount

echo "卸载块设备"
sudo umount /mnt/xfs

echo "卸载xfs文件系统模块"
sudo rmmod hwhxfs

echo "卸载块设备驱动"
sudo rmmod mydriver


echo "环境清理完成"

