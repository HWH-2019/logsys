# logsys
- This is a comprehensive exercise project for linux kernel modification, including adding system calls, modifying the xfs file system, adding block device drivers, timer usage and page fault interrupt statistics. 
- We have combined these five parts into a log management system, of course the effect is very simple, but I believe it is enough for an exercise. 
- In addition to providing the source code of the final effect, a detailed tutorial on the operation of different modules is also specially prepared. Of course, in order to integrate it, we finally made some adjustments, but starting from the simplest, I believe you can understand the linux kernel better.

- 这是linux内核修改的一个综合练习项目，包含了添加系统调用、修改xfs文件系统、添加块设备驱动、定时器使用和缺页中断数统计。
- 我们将这五个部分融合成了一个日志管理系统，当然效果非常简陋，不过我相信用来作为练习已经够用了。
- 除了提供了最终效果的源码外，还专门准备了不同模块操作的详细教程，当然为了融合起来我们最终做了一些调整，但是从最简单的开始我相信你能更加理解linux内核。

# Structure
- **driver** -- Contains a custom block device driver module
- **pagefault** -- Contains a custom page fault interrupt statistics module
- **syscall** -- Contains system call test files
- **xfs** -- Most of the content in this folder comes from the linux kernel, we just modified part of the files
- **main.cpp** -- The main program, including the use of system calls and user logic processing
- **start.sh** -- Startup scripts, including preparation of various modules
- **clean.sh** -- Clean up scripts, remove related modules

# Tutorials
- [Adding system calls(添加系统调用)](https://blog.csdn.net/h2763246823/article/details/122534644?spm=1001.2014.3001.5501)

- [Modifying the xfs file system(修改xfs文件系统)](https://blog.csdn.net/h2763246823/article/details/122538992?spm=1001.2014.3001.5501)

- [Adding block device drivers(添加块设备驱动)](https://blog.csdn.net/h2763246823/article/details/122548221?spm=1001.2014.3001.5501)

- [Timer usage(定时器使用)]()(待完善)

- [Page fault interrupt statistics(缺页中断数统计)](https://blog.csdn.net/h2763246823/article/details/122559742?spm=1001.2014.3001.5501)

# Related
- [Install and create a virtual machine(安装并创建虚拟机)](https://blog.csdn.net/h2763246823/article/details/122522464)
- [Install Ubuntu under VMware virtual machine(VMware虚拟机下安装Ubuntu)](https://blog.csdn.net/h2763246823/article/details/122523076)
- [The latest tutorial on kernel compilation + summary of common problems(内核编译最新教程+常见问题总结)](https://blog.csdn.net/h2763246823/article/details/122522326)
