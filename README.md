在开始编译前，请仔细阅读本文件内容!!!

本工程依赖以下开源组件，原本计划把所有的依赖都放在third目录（未来会完成这个事情，但因为时间太匆忙，因此尚未完成）。这些依赖包括：
log4cpp thrift tinyxml jsoncpp mysqlclient curl libevent lua
依赖的意思就是需要正确安装这些第三方库。除了第三方库以外，在工程上还依赖mysql，所以你还得安装mysql，想想挺折磨人的，别急，后面还有更虐心的，
有lee哥在，大家别慌，有事请加lee哥微信（152 0059 9665） or call me when i donot lie in bed。
或者你也可以给lee哥发邮件：lizhiyong4360@gmail.com。

最后该工程是使用cmake编译的，所以请大家安装cmake工具，当然你不要在windows上安装cmake，因为我们的工程全都是在linux下编译和部署的。

编译的命令:
cd src
cmake .
make