window{title="test"}
background(white)

-- 2D: x, y, r, sx, sy
-- 3D: x, y, z, rx, ry, rz, sx, sy, sz

Transform = ecs.component{x=0, y=0}
Planet = ecs.component{name="unknown", radius=0, color=white}
Orbit = ecs.component{spin=0.1}

function newPlanet(transform, planet, orbit)
  local planet = ecs.entity( transform, planet ) 
  local orbit = ecs.entity( orbit or Orbit() )

  orbit.add(planet)
  return orbit, planet
end

System{
  "Planet",
  update = function(e, planet)
    print("draw", e)
    fill(planet.color)
    circle(0, 0, planet.radius)
  end
}

function setup()  
  local sun, solar_system = newPlanet( Transform{x=game.width/2, y=game.height/2}, Planet{radius=100, color=red} )
  local earth, earth_orbit = newPlanet( Transform{x=100}, Planet{radius=50, color=blue} )
  local moon, moon_orbit = newPlanet( Transform{x=20}, Planet{radius=20, color=white} )

  -- https://www.lua.org/pil/7.1.html
  -- runtime_view / group
  for entity, t in ecs.group( Transform ) do 
    print(t.x, t.y)
  end

  ecs.scene:add(
    solar_system + 
      earth_orbit + 
        moon_orbit
  )
end

