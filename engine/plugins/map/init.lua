local r = get_require(...)
local shash = r("shash")

local Map = {}

engine.Component "Map"
engine.Component "MapTileLayer" 

local Map = class {
  init = function(self)
    self.entity = engine.Entity{ Map=self }
    self.layers = {}
    self.shash = shash.new()
  end,

  _getSpritebatch = function(self, layer, image)
    layer = layer or "_"
    if not self.layers[layer] then 
      self.layers[layer] = {}
    end
    if not self.layers[layer][image] then 
      self.layers[layer][image] = engine.Entity{ MapTileLayer={ image=image } }
      self.entity:add(self.layers[layer][image])
    end
    return self.layers[layer][image].MapTileLayer._spritebatch, layer
  end,

  addTile = function(self, image, x, y, tx, ty, tw, th, _layer)
    local sb, layer = self:_getSpritebatch(_layer, image)
    self.shash:add({
      id = sb:add(engine.Asset.quad(image, tx, ty, tw, th), x, y),
      image = image, 
      layer = layer
    }, x, y, tw, th)    
  end,

  removeTile = function(self, x, y, w, h, layer, image)
    w = w or 1
    h = h or 1
    self.shash:each(x, y, w, h, function(obj)
      if (not image or obj.image == image) and (not layer or obj.layer == layer) then 
        self.shash:remove(obj)
        local sb = self:_getSpritebatch(obj.layer, obj.image)
        sb:set(obj.id, 0, 0, 0, 0, 0)
      end
    end)
  end,

  addEntity = function(self, ent)
    self.entity:add(ent)
  end
}

Map.load = function(name)
  local map = Map()
  -- load ...
  return map 
end

engine.System("MapTileLayer")
  :add(function(ent)
    local mtl = ent.MapTileLayer
    mtl._spritebatch = love.graphics.newSpriteBatch(engine.Asset.image(mtl.image))
  end)
  :draw(function(ent)
    love.graphics.draw(ent.MapTileLayer._spritebatch) 
  end)

return Map