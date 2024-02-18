# NanoNet

NanoNet is a cross-platform network communication library that uses C++17. It encapsulates the native network programming methods of the Linux and Windows operating systems and provides a set of simple and easy-to-use network programming interfaces.

Document 👉 [**NanoNet**](https://akashi.top/proj/nanonet) // TODO

## build

NanoNet requires a minimum C++17 standard. You can build it using CMake.

~~~bash
git clone https://github.com/AkashiNeko/NanoNet.git
cd NanoNet
mkdir build && cd build
cmake ..
~~~

## use

The public header file for the library is located at `include/nanonet.h`. When using it on the Windows platform, you need to link the library `ws2_32.lib`.
