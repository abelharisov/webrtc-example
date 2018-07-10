# build
* fetch submodules:
```
git submodule init
git submodule update
```

* build on linux
```
./webrtc-prepare.sh
mkdir build
cd build
cmake ..
make
```

* build on windows (Visual Studio 2015 Update 3)
```
./webrtc-prepare.bat
mkdir build
cd build
cmake ..
cmake --build .
```
 
# run
```
./webrtc-example 8888
```
and enter `ip:port` of another instance