cmake -G "Visual Studio 16 2019" -A x64 -B build -S . -DOPENRTM_VERSION=1.2.1 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target INSTALL
