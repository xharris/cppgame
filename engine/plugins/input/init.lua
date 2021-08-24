local Input = callable{
  __ = {
    index = function(t, k)
      if k == 'mouse_x' then 
        return (love.mouse.getX() - engine.Game.padx) / engine.Game.scale
      end
      if k == 'mouse_y' then 
        return (love.mouse.getY() - engine.Game.pady) / engine.Game.scale
      end
      return rawget(t, k)
    end
  }
}


return Input 