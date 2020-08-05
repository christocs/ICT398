<div align=center>
  <img src="https://i.imgur.com/ltfhqiW.png" alt="ict397-logo" width="200">
  <p>
    ICT398 Assignment
  </p>
</div>

## Table of Contents
* [1&nbsp;&nbsp;Building](#building)
  * [1.1&nbsp;&nbsp;Unix](#unix)
  * [1.2&nbsp;&nbsp;Windows](#windows)
* [2&nbsp;&nbsp;Contributing](#contributing)
* [3&nbsp;&nbsp;Meta](#meta)
  * [3.1&nbsp;&nbsp;License](#license)
  * [3.2&nbsp;&nbsp;Built With](#built-with)

## Building
### Unix
Clone repository:
```
git clone https://github.com/opeik/ICT398.git
git submodule update --init --recursive
```

Install build tools:

```
# macOS
brew install cmake ninja

# Linux
apt install build-essential clang ninja-build clang-9 \
  libstdc++-9-dev ninja-build libgl1-mesa-dev libx11-dev \
  libxrandr-dev libudev-dev libfreetype6-dev \
  libopenal-dev libflac++-dev libvorbis-dev \
  libxinerama-dev libxcursor-dev libxi-dev
```

Generate build files:
```
# Debug
CXX=clang++ cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Debug
# Release
CXX=clang++ cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=RelWithDebInfo
```

Compile:
```
cmake --build .
```

### VSCode
* Follow [Unix](#unix) instructions for project cloning and dependency installation
* Install [CMake tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* Open the command palette, run:
  * CMake: Select a kit
  * CMake: Select variant
  * CMake: Build

### Windows
Enable developer mode:
* Open Settings
* Navitgate to Update & Security → For developers
* Enable Developer Mode

Install build tools:
* [Visual Studio Community][2] (2019 or newer)
  * Install the "C++ CMake tools for Windows" component
  * Install the "Clang compiler for Windows" component

Clone repository:
```
git clone https://github.com/opeik/ICT398.git
git submodule update --init --recursive --depth 1
```

Compile:
* Open Visual Studio
* Select Open → CMake
* Select `CMakeLists.txt`
* Set the startup item to `afk.exe`

### Documentation
Generate doxygen:
```
doxygen .doxyconf
```

## Contributing
Please see the [`CONTRIBUTING.md`](CONTRIBUTING.md) file for instructions.

## Meta
### License
This project is licensed under the ISC license. Please see the [`LICENSE.md`](LICENSE.md)
file for details. Individual authors can be found inside the [`AUTHORS.md`](AUTHORS.md) file.

### Built With
TODO

[1]: https://github.com/microsoft/vcpkg#quick-start
[2]: https://visualstudio.microsoft.com/downloads/
