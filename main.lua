window{title="test"}
background(white)

local img_ball, spr_robot

function setup()
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

solar_system    = Node()
sun             = Node{x=game.width/2, y=game.height/2, radius=100}
earth_orbit     = Node{x=100, spin=0.1}
earth           = Node{radius=50, color=blue}
moon_orbit      = Node{x=20, spin=0.1}
moon            = Node{radius=25, color=white}

sun.x = 20
sun.radius = 200
print("sun", sun.id, sun.x, sun.radius)
print("earth_orbit", earth_orbit.id, earth_orbit.spin)

-- system{
--   "orbit", 
--   process = function(e, dt)
--     e.r = e.r + math.rad(e.orbit) * dt
--   end
-- }

System{
  "radius",
  draw = function(e)
    fill(e.color or orange)
    circle(e.x, e.y, e.radius)
  end
}

System{
  "spin",
  update = function(e, dt)
    e.r = e.r + math.rad(5) * dt
  end
}

game.scene:add( 
  solar_system + {
    sun,
    earth_orbit + {
      earth,
      moon_orbit +
        moon
    }
  }
)

