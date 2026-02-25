cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build/tests --output-on-failure
./build/examples/example
