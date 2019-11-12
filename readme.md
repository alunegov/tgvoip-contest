# Linux

## Dev deps

```
sudo apt-get install libssl-dev libogg-dev libopus-dev libopusfile-dev
```

Build tools:

```
sudo apt-get install g++ cmake ninja
```

### libtgvoip

```
cd _ext/libtgvoip
./configure --enable-audio-callback CXXFLAGS='-DTGVOIP_LOG_VERBOSITY=1 -DNDEBUG'
make
sudo make install
```

### libopusenc

```
cd _ext/libopusenc
./configure
make
sudo make install
```

## Build tgvoipcall

```
cd tgvoipcall
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Build tgvoiprate

```
cd tgvoiprate
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

# Windows

## Dev deps

```
vcpkg install openssl ogg opus opusfile opusenc
```

Build tools:
- MSVC
- vcpkg
- cmake

### libtgvoip

```
cd _ext/libtgvoip
gyp --depth=. libtgvoip.gyp
TODO:
```

## Build tgvoipcall

```
cd tgvoipcall
md build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_LIBRARY_PATH=../_ext/libtgvoip/x64/Release ..
cmake --build . --target ALL_BUILD --config Release
```

## Build tgvoiprate

```
cd tgvoiprate
md build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static ..
cmake --build . --target ALL_BUILD --config Release
```
