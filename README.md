# NanoNet

NanoNet is a cross-platform network communication library that uses C++17. It encapsulates the native network programming methods of the Linux and Windows operating systems and provides a set of simple and easy-to-use network programming interfaces.

## build

~~~bash
git clone https://github.com/AkashiNeko/NanoNet.git
cd NanoNet
mkdir build && cd build
cmake ..
make -j
sudo make install
~~~

## use

When using it on the Windows platform, you need to link the Windows library `ws2_32.lib`.

