window{title="test"}
background(white)

local img_ball, spr_robot

function load()
  img_ball = image("res/ball.png")
  image("res/bluerobot.png")
    :sprite("robot_walk", {'1-8'}, 33, 33) -- , offx, offy)
    :sprite("robot_stand", {1}, 33, 33)

  -- spr_robot = sprite("robot_walk")
end

function draw()
  font{size=20}
  push() 
    fill(blue)
    push()
      fill(green)
      text("text(str) "..game.fps)
    pop()
    text("text(str,x,y)", 190, 200)
  pop()

  img_ball()
  -- spr_robot()
end

-- x, y, sx, sy, r, kx, ky, z

solar_system    = node()
sun             = node{x=game.width/2, y=game.height/2, radius=100}
earth_orbit     = node{x=100, orbit=0.1}
earth           = node{radius=50}
moon_orbit      = node{x=20, orbit=0.1}
moon            = node{radius=25}

print(sun, sun.id, sun.x, earth_orbit.orbit)

-- system{
--   "orbit", 
--   process = function(e, dt)
--     e.r = e.r + math.rad(e.orbit) * dt
--   end
-- }

-- system{
--   "radius",
--   draw = function(e)
--     fill(e.color or orange)
--     circle(e.x, e.y, e.radius)
--   end
-- }

--[[
solar_system.add(
  sun,
  earth_orbit.add(
    earth,
    moon_orbit.add(
      moon
    )
  )
)
]]

-- solar_system + {
--   sun,
--   earth_orbit + {
--     earth,
--     moon_orbit +
--       moon
--   }
-- }

