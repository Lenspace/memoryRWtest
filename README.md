# memoryRWtest
# archives

mkdir build
cd build
cmake ..
make

目前看起来，多线程内部使用mutex，进行读取的效率远远高于共享内存读写

