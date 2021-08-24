local ui = {}

local imgui = engine.Imgui()
local ffi = require("ffi")
local Signal = engine.Signal

ui.imgui = imgui

ui.Number = function(v)
  local num = ffi.new("int[1]")
  num[0] = v 
  return num
end

ui.Array = function(...)
  local args = {...}
  if type(args[1]) == "table" then 
    args = args[1]
  end
  local arr = ffi.new("const char*[?]", #args)
  -- for i = 1, select("#", ...) do 
  --   arr[i-1] = select(i, ...)
  -- end
  arr[0] = "wow" -- ffi.string("wow")
  arr[1] = "bob"
  arr[3] = "ok"
  return arr[0]
end

-- setup imgui callbacks
Signal.on('load', function()
  imgui.Init()

  Signal.on('textinput', function(...) imgui.TextInput(...) end)
  Signal.on('keypressed', function(key) imgui.KeyPressed(key) end)
  Signal.on('keyreleased', function(key) imgui.KeyReleased(key) end)
  Signal.on('mousemoved', function(x,y) imgui.MouseMoved(x,y) end)
  Signal.on('mousepressed', function(x,y,button) imgui.MousePressed(button) end)
  Signal.on('mousereleased', function(x,y,button) imgui.MouseReleased(button) end)
  Signal.on('wheelmoved', function(...) imgui.WheelMoved(...) end)
  Signal.on('quit', function() return imgui.Shutdown() end)
end)

engine.System()
  :updateAll(function(_, dt)
    imgui.Update(dt)
    imgui.NewFrame()
  end)



return ui