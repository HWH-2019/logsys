set -e
logsysPath="./logsys.o"
xfsPath="/mnt/xfs"
pagefaultPath="$xfsPath/miss.log"
timePath="$xfsPath/time.log"

echo "安装块设备驱动"
sudo insmod ./driver/mydriver.ko

echo "安装xfs文件系统模块"
sudo modprobe libcrc32c
sudo insmod ./xfs/hwhxfs.ko


echo "格式化块设备为xfs"
sudo mkfs.xfs /dev/sblkdev0

if [ ! -d "$xfsPath" ]; then

sudo mkdir "$xfsPath"

fi

echo "挂载xfs文件系统到块设备中"
sudo mount -t hwhxfs /dev/sblkdev0 /mnt/xfs


echo "创建日志文件"
if [ ! -f "$pagefaultPath" ]; then

sudo touch "$pagefaultPath"

fi

if [ ! -f "$timePath" ]; then

sudo touch "$timePath"

fi

echo "安装缺页统计模块"

sudo insmod ./pagefault/readpfcount.ko path="$pagefaultPath"

echo "环境准备完成"


echo "创建logsys主进程"
if [ ! -f "$xfsPath" ]; then

g++ main.cpp -o logsys.o

fi

echo "创建配置文件"
if [ ! -f "$xfsPath/.config" ]; then
echo "正在创建..."
sudo touch $xfsPath/.config
echo "misspath=$pagefaultPath" | sudo tee -a $xfsPath/.config
echo "timepath=$timePath" | sudo tee -a $xfsPath/.config
echo "创建完成"
fi

option="y"
read -p "创建成功，是否立即启动[y/n]" option

if [ "$option" = "y" ];then
echo "启动中..."
sudo ./logsys.o
else
echo -e "准备就绪\n\t1.执行 ./logsys.o 启动 logsys\n\t2.执行 ./clean.sh 清理环境"
fi



