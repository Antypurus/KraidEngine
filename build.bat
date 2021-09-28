@echo off

mkdir build
cd build
mkdir msvc
cd msvc

cmake ../..
cmake --build .
