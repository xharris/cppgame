window{title="test"}
background(white)

print(background())

function setup()  
  solar_system    = Entity()
  sun             = Entity{
    Transform = {x=game.width/2, y=game.height/2}, 
    Planet = { radius = 100, color = blue }
  }
  -- earth_orbit     = Entity{x=100, spin=0.1}
  -- earth           = Entity{radius=50, color=blue, jim=20}
  -- moon_orbit      = Entity{x=20, spin=0.1}
  -- moon            = Entity{radius=25, color=white}

  -- game.scene:add( 
  --   solar_system + {
  --     sun,
  --     earth_orbit + {
  --       earth,
  --       moon_orbit +
  --         moon
  --     }
  --   }
  -- )
end

System{
  "Transform", "Planet",
  draw = function(e)
    local t, planet = e.Transform, e.Planet
    fill(planet.color)
    circle(t.x, t.y, planet.radius)
  end,
  update = function(e, dt)
    local t = e.Transform
    t.x = t.x + 5 * dt
  end
}


