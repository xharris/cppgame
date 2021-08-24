local module = {
  load = function(follow_entity)
    engine.View{
      transform = { 
        ox = engine.Game.width/4, oy = engine.Game.height/4,
        x = engine.Game.width/2, y = engine.Game.height/2
      },
      view = { 
        entity = follow_entity,
        r = math.rad(45)
      },
      size = { w=engine.Game.width/2, h=engine.Game.height/2 },
      -- on_mouse = true
      rotate = 10
    }
    engine.View("alt", {
      view = {
        entity = follow_entity
      },
      size = { w=100, h=100 },
      on_mouse = 'local'
    })
  end
}

engine.System("view", "size")
  :draw(function(ent)
    local view, t, size = ent.view, ent.transform, ent.size
    love.graphics.setColor(1,1,1,1)
    love.graphics.rectangle('line', 0, 0, size.w, size.h)
  end)

engine.System("on_mouse")
  :update(function(ent, dt)
    if ent.on_mouse == 'local' then 
      ent.transform.x, ent.transform.y = input.mouse_x, input.mouse_y
    else
      ent.transform.x, ent.transform.y = engine.View.getWorld(input.mouse_x, input.mouse_y)
    end
  end)

return module 