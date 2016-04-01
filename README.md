# MilkOS
介绍：
MilkOS是基于i686处理器架构开发的32位操作系统，目前的功能包含段页式寻址，任务并发，多终端切换，软盘I/O，ELF格式文件加载器，
简易shell等，文件系统为FAT12。目前shell提供了浏览软盘文件，运行应用程序，切换终端，清屏，查看内存的功能，另外的功能将在后
续进行开发。该版本的操作系统重点在于内核开发，去掉不必要的GUI开发，因为虚拟机虚拟显卡的原因，目前暂不支持132x43的文本模式
，本系统用的显示模式为80x25文本模式。Github网址:https://github.com/KaimingOuyang/KaiOS.git


系统运行方法：

1.在linux系统下安装qemu,输入(ubuntu系统shell命令):sudo apt-get install qemu.

2.安装好qemu后，可以在我的Github网址中下载源码和软盘镜像，软盘镜像名称为milk.img

3.运行操作系统，命令为qemu-system-i386 -m 256M -vga vmware -fda milk.img



shell功能使用介绍：

ls：浏览软盘内文件

clear:清屏

free:查看当前内存剩余容量

Alt+F1-F10:切换终端，一共可以开启10个终端，可以并发执行10个任务。

./logo:执行logo应用程序，可以展示本操作系统logo.可执行文件后缀名为mk,在ls命令中的type栏显示

