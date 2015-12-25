# juCi++ Installation Guide

- Linux
  - [Debian/Ubuntu 15](#debianubuntu-15)
  - [Ubuntu 14/Linux Mint 17](#ubuntu-14linux-mint-17)
  - [Arch Linux](#arch-linux)
- OS X
  - [Homebrew](#os-x-with-homebrew-httpbrewsh)
- Windows
  - [MSYS 2](#windows-with-msys2-httpsmsys2githubio)

## Debian/Ubuntu 15
Install dependencies:
```sh
sudo apt-get install git cmake make g++ libclang-dev pkg-config libboost-system-dev libboost-thread-dev libboost-filesystem-dev libboost-log-dev libboost-regex-dev libpython-dev python libgtksourceviewmm-3.0-dev aspell-en libaspell-dev
sudo apt-get install clang-format-3.6 || sudo apt-get install clang-format-3.5
```

Get juCi++ source, compile and install:
```sh
git clone --recursive https://github.com/cppit/jucipp
mkdir jucipp/build
cd jucipp/build
cmake ..
make
sudo make install
```

## Ubuntu 14/Linux Mint 17
Install dependencies:
```sh
sudo apt-get install git cmake make g++ libclang-3.6-dev clang-format-3.6 pkg-config libboost-system-dev libboost-thread-dev libboost-filesystem-dev libboost-log-dev libboost-regex-dev python3 libpython3-dev libgtksourceviewmm-3.0-dev aspell-en libaspell-dev
<!-- TODO  check this -->
```
Get juCi++ source, compile and install:
```sh
git clone --recursive https://github.com/cppit/jucipp
mkdir jucipp/build
cd jucipp/build
cmake ..
make
sudo make install
```

##Arch Linux
Package available in the Arch User Repository:
https://aur.archlinux.org/packages/jucipp-git/

Alternatively, follow the instructions below.

Install dependencies:
```sh
#as root
pacman -S git cmake make clang gtksourceviewmm boost aspell aspell-en
```

Get juCi++ source, compile and install:
```sh
git clone --recursive https://github.com/cppit/jucipp
mkdir jucipp/build
cd jucipp/build
cmake ..
make
# as root
make install
```

## OS X with Homebrew (http://brew.sh/)
Install dependencies (installing llvm may take some time):
```sh
brew install cmake --with-clang llvm pkg-config boost homebrew/x11/gtksourceviewmm3 aspell clang-format
```

Get juCi++ source, compile and install:
```sh
git clone --recursive https://github.com/cppit/jucipp
mkdir jucipp/build
cd jucipp/build
cmake ..
make
make install
```

##Windows with MSYS2 (https://msys2.github.io/)
Install dependencies (replace `x86_64` with `i686` for 32-bit MSYS2 installs):
```sh
pacman -S git mingw-w64-x86_64-cmake make mingw-w64-x86_64-toolchain mingw-w64-x86_64-clang mingw-w64-x86_64-gtkmm3 mingw-w64-x86_64-gtksourceviewmm3 mingw-w64-x86_64-boost mingw-w64-x86_64-aspell mingw-w64-x86_64-aspell-en
```

Get juCi++ source, compile and install (replace `mingw64` with `mingw32` for 32-bit MSYS2 installs):
```sh
git clone --recursive https://github.com/cppit/jucipp
mkdir jucipp/build
cd jucipp/build
cmake -G"MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw64 ..
make
make install
```

<!--
## Windows with Cygwin (https://www.cygwin.com/)
**Make sure the PATH environment variable does not include paths to non-Cygwin cmake, make and g++.**

Select and install the following packages from the Cygwin-installer:
```
pkg-config libboost-devel libgtkmm3.0-devel libgtksourceviewmm3.0-devel xinit
```
Then run the following in the Cygwin Terminal:
```sh
git clone https://github.com/cppit/jucipp.git
cd jucipp
cmake .
make
make install
```

Note that we are currently working on a Windows-version without the need of an X-server.
-->

## Run
```sh
juci
```

<!--
#### Windows
```sh
startxwin /usr/local/bin/juci
```
-->
