local p_require = get_require(...)

local require_effect = function(name)
  moonshine.effects[name] = p_require(name)(moonshine)
end

require_effect("boxblur")
require_effect("chromasep")
require_effect("glow")
require_effect("filmgrain")