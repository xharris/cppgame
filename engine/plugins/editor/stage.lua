local input = engine.Plugin "input"
local color = engine.Plugin "color"

engine.Component("Grid", { redraw=false })

local maps = { test=map() }
local camera = {x=0, y=0}
local snap = {x=32, y=32}
local grid
local sys_grid

local camera_start, dragging
local drag_start = function(x, y)
  dragging = { x=x, y=y }
  camera_start = copy(camera)
end
local drag_stop = function()
  dragging = nil 
  camera_start = nil
end

local grid_redraw = function(resize)
  sys_grid:iterate(function(ent)
    if resize then 
      ent.Grid._canvas = love.graphics.newCanvas()
    end
    ent.Grid.redraw = true
  end)
end

sys_grid = engine.System("Grid")
  :add(function(ent) 
    local g = ent.Grid 
    ent.z = -1000
    g._canvas = love.graphics.newCanvas()
    grid_redraw()
  end)
  :updateAll(function(ents, dt)
    if dragging and camera_start then 
      camera.x = camera_start.x - (dragging.x - input.mouse_x)
      camera.y = camera_start.y - (dragging.y - input.mouse_y)
      grid_redraw()
    end
  end)
  :update(function(ent, dt)
    local g = ent.Grid
    if g.redraw then 
      g._canvas:renderTo(function()
        local lg = love.graphics
        lg.push('all')
        engine.Game.clear(1,1,1,0)
        lg.setColor(color('grey', "300", 0.2))
        lg.setLineWidth(1)
        lg.setLineStyle('rough')
        local linex, liney = 0, 0
        -- v lines
        for x = 0, engine.Game.width / snap.x do 
          linex = x * snap.x + (camera.x % snap.x)
          lg.line(linex, 0, linex, engine.Game.height)
        end
        -- h lines
        for y = 0, engine.Game.height / snap.y do 
          liney = y * snap.y + (camera.y % snap.y)
          lg.line(0, liney, engine.Game.width, liney)
        end
        -- origin
        lg.setColor(color('grey', "300", 1))
        lg.line(0,camera.y,engine.Game.width,camera.y) -- h
        lg.line(camera.x,0,camera.x,engine.Game.height) -- v
        lg.pop()
      end)
      g.redraw = false
    end
  end)
  :draw(function(ent)
    local g = ent.Grid
    love.graphics.draw(g._canvas)
  end)

engine.Game.scaling = "none"
engine.Signal.on('load', function()
  engine.Entity{ Grid={ }, Snap={ } }
end)

engine.Signal.on('resize', function()
  grid_redraw(true)
end)

engine.Signal.on('mousepressed', function(x, y, btn)
  if btn == 3 then 
    drag_start(input.mouse_x, input.mouse_y)
  end
end)

engine.Signal.on('mousereleased', function(x, y, btn)
  if btn == 3 then 
    drag_stop()
  end
end)