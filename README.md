# NanoNet

C++跨平台网络通信库，封装了Linux和Windows系统原生的网络类型和接口，提供了简单易用且功能丰富的接口。

库文档👉[**NanoNet**](https://akashi.top/proj/nanonet)

## 生成库

NanoNet库使用的最低C++标准为C++11。

~~~bash
git clone https://github.com/AkashiNeko/NanoNet.git
cd NanoNet
mkdir build && cd build
cmake ..
~~~

在Windows下，`cmake` 将生成 `nanonet.sln`，Linux则生成 `Makefile` 文件。

## 使用库

库的公共头文件位于 `include/nanonet.h`。使用 `g++` 进行编译链接时，需要附加 `-lnanonet` 选项。如果在Windows平台使用 `g++`，还需附加 `-lws2_32` 选项。
