# VoIP Contest, Stage 1

## Task

The task of the first round was to create two C++ applications for testing and rating the quality of VoIP calls.

## Project structure

- **opusfile-reader**, **opusfile-reader** - reader/writer for OPUS files
- **tgvoipcall** - app for make calls to relay server
- **tgvoipcall-runner** - script for make calls with tgvoipcall and samples to relay server
- **tgvoiprate** - app for rate calls
- **tgvoiprate-runner** - script for run tgvoiprate, script for recalc and analyze stage 1 results
- **wavefile-reader**, **wavefile-reader** - reader/writer for WAV files

## dev

### deps

```sh
sudo apt-get install libssl-dev libogg-dev libopus-dev libopusfile-dev
```

Build tools:

```sh
sudo apt-get install g++ cmake ninja-build
```

```sh
cd _ext/libopusenc
./configure
make
sudo make install

cd _ext/libtgvoip
./configure --enable-audio-callback CXXFLAGS='-DTGVOIP_LOG_VERBOSITY=1 -DNDEBUG'
make
sudo make install

mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## dev (Windows)

### deps

```sh
vcpkg install openssl ogg opus opusfile opusenc
```

Build tools:
- MSVC
- vcpkg
- cmake

```sh
cd _ext/libtgvoip
gyp --depth=. libtgvoip.gyp
TODO:

md build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_LIBRARY_PATH=../_ext/libtgvoip/x64/Release ..
cmake --build . --target ALL_BUILD --config Release
```

## Result

Issues from judges:

```
The submitted tgvoiprate app got a score of 1359 which means it is less precise than the solution that always returned 3.0 as the result. Unfortunately, this is not good enough to compete for the prize.
```
