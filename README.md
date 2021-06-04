# cppgame

# Setup

Add submodules

`git submodule update --init --recursive external/lua`

Create build directory:

`mkdir build; cd build`

Configure and generate build system:

* MinGW: `cmake .. -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"` (or `Release`)

# Run

`cmake --build .\build; .\build\bin\cpp_game.exe`
* Windows: you may need to run powershell/cmd as admin

# Test 

`cmake --build .\build; .\build\bin\cpp_game_test.exe`