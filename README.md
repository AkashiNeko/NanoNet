# NanoNet

C++跨平台网络通信库，封装了Linux和Windows系统原生的网络类型和接口，提供了尽量简单易上手且功能丰富的接口。

库文档👉[nanonet](https://akashi.top/proj/nanonet)

## 构建项目

~~~bash
git clone https://github.com/AkashiNeko/NanoNet.git
cd Nanonet
mkdir build
cd build
cmake .. // Linux
make
~~~

## 使用库

公共头文件位于 `include/nanonet.h`，生成的库文件位于 `lib` 目录下。

使用NanoNet库构建项目时，需要使用C++11及以上标准，选择 `g++` 作为编译器时，需要附加 `-lnanonet` 选项。如果在Windows平台构建，还需使用 `-lws_32` 选项。

