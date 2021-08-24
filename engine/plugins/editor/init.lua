local editor = {
  _VERSION        = 'engine v0.8.0',
  _URL            = 'https://github.com/xharris/mechanic6-remake',
  _DESCRIPTION    = 'An engine I\'ll probably use in my games',
  _PLUGINS        = { "input" },
  _LICENSE        = [[
    MIT LICENSE

    Copyright (c) 2021 Xavier Harris

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ]]
}

local r = get_require(...)
local Signal = engine.Signal

engine.Component("Snap", { x=32, y=32 })

-- engine.Order{
--   engine = {
--     "grid", ""
--   }
-- }

local ui = engine.Plugin "ui"
local imgui = ui.imgui
r "stage"

local show_demo = true
local MODE = {none=0, image=1, object=2}
local place_mode = MODE.none -- "image", "object"
local selected_image = nil

local textures = {}
local get_texture = function(img)
  if not img then return nil end
  if textures[img] then return textures[img] end 
  local img = engine.Asset.image(img)
  textures[img] = {
    data = img,
    w = img:getWidth(),
    h = img:getHeight()
  }
  return textures[img]
end

Signal.on('draw', function()
  if show_demo then 
    imgui.ShowDemoWindow()
  end  

  do -- TILE / OBJECT SELECTOR
    imgui.SetNextWindowPos(imgui.ImVec2_Float(20, 20), imgui.ImGuiCond_FirstUseEver)
    imgui.SetNextWindowSize(imgui.ImVec2_Float(200, engine.Game.height - 400), imgui.ImGuiCond_FirstUseEver)
    imgui.Begin("Place")
    do -- select object type
      imgui.PushItemWidth(-1)
      local ffi_place_mode = ui.Number(place_mode)
      if imgui.Combo_Str("##hi", ffi_place_mode, ui.Array(table.keys(MODE)), 3) then 
        place_mode = ffi_place_mode[0]
      end
      imgui.PopItemWidth()
      

    --   -- IMAGE (TILE) 
    --   if place_mode.value == MODE.image then 
    --     imgui.PushItemWidth(-1)
    --     local images = engine.Asset.list("image")
    --     selected_image = imgui.Combo("file", selected_image, images, #images)
    --     if selected_image > 0 then 
    --       local tex = get_texture(images[selected_image])
    --       local max_w = imgui.GetContentRegionAvailWidth()
    --       local padding = 20 
    --       local flags = {"ImGuiWindowFlags_NoScrollbar"}
    --       if tex.h+padding > 200 then 
    --         table.insert(flags, "ImGuiWindowFlags_AlwaysVerticalScrollbar")
    --       end 
    --       if tex.w+padding > max_w then 
    --         table.insert(flags, "ImGuiWindowFlags_AlwaysHorizontalScrollbar")
    --       end
    --       imgui.BeginChildFrame(imgui.GetID("image"), math.min(tex.w + padding, max_w), math.min(tex.h + padding, 200), flags)
    --       imgui.Image(tex.data, tex.w, tex.h)
    --       imgui.EndChild()
    --     end
    --     imgui.PopItemWidth()
    --   end
    --   -- OBJECT 
    --   if place_mode.value == MODE.object then 
    --     imgui.Button("add")
    --     imgui.SameLine()
    --     imgui.Button("delete")
    --   end
    --   -- imgui.PopItemWidth()
    end
    imgui.End()
  end
  
  imgui.Render()
  imgui.RenderDrawLists()
end)

return editor