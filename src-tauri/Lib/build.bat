rm -rf build
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cmake --build . --config Release
