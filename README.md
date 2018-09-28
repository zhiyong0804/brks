在开始编译前，请仔细阅读本文件内容!!!

  本工程依赖以下开源组件，原本计划把所有的依赖都放在third目录（未来会完成这个事情，但因为时间太匆忙，因此尚未完成）。
这些依赖包括：
1.log4cpp
  
  
2.thrift
      sudo apt-get install automake bison flex g++ git libboost-all-dev libevent-dev 
      libssl-dev libtool make pkg-config


3.tinyxml
      进入tinyxml解压目录，用文本编辑器打开Makefile文件。
      修改编译目的文件参数。将OUTPUT := xmltest改为OUTPUT := libtinyxml.so。
      删除测试文件。将SRCS := tinyxml.cpp tinyxmlparser.cpp xmltest.cpp tinyxmlerror.cpp tinystr.cpp中的xmltest.cpp删除；
      将xmltest.o: tinyxml.h tinystr.h这一行删除。
      将 ${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}这一行修改为${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS} -fPIC –shared。–shared是用来生成动态库的。-fPIC这个参数最好带上，否则有的.so依赖这个.so时可能会出现问题，提示找不到这个.so什么的。
      在命令行中进入Makefile目录，执行命令：make DEBUG=YES PROFILE=NO TINYXML_USE_STL=NO。DEBUG=YES表示编译的是debug版本，TINYXML_USE_STL=NO表示编译非STL标本。


4.jsoncpp 
安装步骤如下：
            #tar -zxvf scons-2.1.0.tar.gz #cd scons-2.1.0

            #python setup.py install

           #tar -zxvf jsoncpp-src-0.5.0.tar.gz

           #cd jsoncpp-src-0.5.0

           #scons platform=linux-gcc

           #mv libs/linux-gcc-4.4.7/libjson_linux-gcc-4.4.7_libmt.so /lib

           #ln /lib/libjson_linux-gcc-4.4.7_libmt.so /lib/libjson.so

           #mv include/json/ /usr/include/

           #ldconfig

           #./bin/linux-gcc-4.4.7/test_lib_json

5.mysqlclient

6.curl 
    ./configure
    make 
    make install

7.libevent
    ./configure
    make 
    make install

8.lua
    sudo apt-get install lua5.3mysql

9.mysql
    sudo apt-get install docker
    docker pull mysql:5.72

//启动mysql
      docker run -di --name mt_mysql -p 3306:3306 \
      -e MYSQL_ROOT_PASSWORD=123456 mysql:5.72


依赖的意思就是需要正确安装这些第三方库。除了第三方库以外，在工程上还依赖mysql，所以你还得安装mysql，想想挺折磨人的，别急，后面还有更虐心的，
有lee哥在，大家别慌，有事请加lee哥微信（152 0059 9665） or call me when i donot lie in bed。
或者你也可以给lee哥发邮件：lizhiyong4360@gmail.com。

最后该工程是使用cmake编译的，所以请大家安装cmake工具，当然你不要在windows上安装cmake，因为我们的工程全都是在linux下编译和部署的。

编译的命令:
cd src
cmake .
make
