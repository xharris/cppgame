local hex2rgb = memoize(function(hex, a)
  local hex = hex:gsub("#","")
  if hex:len() == 3 then
    return (tonumber("0x"..hex:sub(1,1))*17)/255, (tonumber("0x"..hex:sub(2,2))*17)/255, (tonumber("0x"..hex:sub(3,3))*17)/255
  else
    return tonumber("0x"..hex:sub(1,2))/255, tonumber("0x"..hex:sub(3,4))/255, tonumber("0x"..hex:sub(5,6))/255
  end
end)

local color
color = callable{
  library = {},

  __call = function(t, c, shade, b, a)
    local c_type = type(c)
    -- { r, g, b }
    if c_type == "table" then 
      return unpack(c), shade or 1
    end
    -- rgba
    if c_type == "number" and b ~= nil then 
      return c, shade, b, a
    end
    -- #FFFFFF
    if c_type == "string" and c:starts("#") then 
      return hex2rgb(c), shade or 1
    end
    -- preloaded color
    if c == "gray" then c = "grey" end
    assert(color.library[c], "Color '"..c.."' not found")
    if type(shade) == "number" then -- shade is an alpha value
      b = shade 
      shade = nil 
    end
    if not shade then 
      local keys = table.keys(color.library[c])
      shade = keys[floor(#keys/2)]
    else 
      shade = tostring(shade)
    end
    assert(color.library[c][shade], "Shade '"..shade.."' of color '"..c.."' not found")
    c = color.library[c][shade]
    return c[1], c[2], c[3], b or 1
  end,

  load = function(json_file, append)
    contents, size = love.filesystem.read(json_file)
    if not contents then return end 

    data = json.decode(contents)
    if not append then 
      color.library = {}
    end
    for c, shades in pairs(data) do 
      for shade, hex in pairs(shades) do 
        color.set(c, shade, hex)
      end 
    end
  end,

  set = function(c, shade, hex)
    if not color.library[c] then 
      color.library[c] = {}
    end
    color.library[c][shade] = {rgb(hex2rgb(hex))}
  end
}

color.load("engine/plugins/color/material.json")

return color