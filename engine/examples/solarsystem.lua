local color = require "plugins.color"

local module = {
  load = function()
    local sun_orbit, sun = Planet(20, 0, 10, "orange")
    local earth_orbit, earth = Planet(10, 100, 50, "lightblue")
    local moon_orbit, moon = Planet(5, 50, 0, "gray")
    
    sun_orbit.transform.x = engine.Game.width/2
    sun_orbit.transform.y = engine.Game.height/2
  
    sun_orbit:add(earth_orbit)
    earth_orbit:add(moon_orbit)

    return sun_orbit, earth_orbit, moon_orbit
  end
}

Planet = function(size, distance, rotate, color)
  local planet = engine.Entity{ circle=size, color=color }
  local orbit = engine.Entity{ rotate=rotate }
  orbit.transform.x = distance
  orbit:add(planet)
  return orbit, planet 
end

engine.System("rotate")
  :update(function(e, dt)
    e.transform.r = e.transform.r + math.rad(e.rotate) * dt 
  end)

engine.System("circle", "color")
  :draw(function(e)
    fill(color(e.color))
    .circle(0, 0, e.circle)
  end)

return module