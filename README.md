# cppgame

# Setup

Add submodules

`git submodule update --init --recursive external/lua`

Create build directory:

`mkdir build; cd build`

Configure and generate build system:

* MinGW: `cmake .. -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"` (or `Release`)

# Run

`cmake --build .\build --target cpp_game; .\build\bin\cpp_game.exe`
* Windows: you may need to run powershell/cmd as admin

# Test 

`cmake --build .\build --target cpp_game_test; .\build\bin\cpp_game_test.exe`

# love -> raylib/p5.js

love.math.newTransform() look at https://github.com/raysan5/raylib/blob/master/src/raymath.h
love.graphics.push() --> push
love.graphics.applyTransform(tform)
love.graphics.setColor --> color
love.graphics.pop --> pop
love.filesystem.getDirectoryItems(path) --> fs.ls
love.filesystem.getInfo(path) ...
  { type } --> fs.isFile / fs.isDirectory
love.graphics.newImage(path)
love.graphics.newQuad(x,y,w,h,tw,th)
love.graphics.origin()
love.graphics.draw(tex, quad)
love.graphics.newCanvas()
love.graphics.newShader
love.graphics.setDefaultFilter(?, ?, ?)
love.graphics.getWidth()
love.graphics.getHeight()
w, h, flags = love.window.getMode()
love.window.setMode(w, h, flags)
love.graphics.clear(r, g, b, a) --> clear
love.load --> setup
love.update --> update
love.draw --> draw
love.graphics.translate --> translate
love.graphics.rotate --> rotate
love.graphics.scale --> scale
love.resize