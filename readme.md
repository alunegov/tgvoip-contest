## Dev deps

Linux:

```
sudo apt-get install libssl-dev libogg-dev libopus-dev libopusfile-dev
```

Windows:

```
vcpkg install openssl? ogg? opus? opusfile? opusenc?
```

### libtgvoip

```
cd _ext/libtgvoip
./configure --enable-audio-callback CXXFLAGS='-DTGVOIP_LOG_VERBOSITY=1 -DNDEBUG' (duplicate to CFLAGS?)
make
sudo make install
```

### libopusenc (Linux only)

```
cd _ext/libopusenc
./configure
make
sudo make install
```

## Build tgvoipcall

Linux:

```
cd tgvoipcall
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

Windows:

```
cd tgvoipcall
md build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_LIBRARY_PATH=../../../Work3/libtgvoip/x64/Debug ..
cmake --build . --target ALL_BUILD --config Release
```

## Build tgvoiprate

Linux:

```
cd tgvoiprate
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
