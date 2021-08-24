-- TABLE

table.hasValue = function(t, value)
  for i, v in ipairs(t) do
    if v == value then return true end 
  end
  return false 
end

table.keys = function(t)
  local keys = {}
  for k,v in pairs(t) do
    table.insert(keys, k)
  end
  return keys 
end

table.size = function(t)
  local i = 0
  for k,v in pairs(t) do
    i = i + 1
  end
  return i
end

table.values = function(t)
  local values = {}
  for k,v in pairs(t) do
    table.insert(values, v)
  end
  return values 
end

table.iterate = function(t, fn)
  local len = #t
  local offset = 0
  local removals = {}
  for o=1,len do
    local obj = t[o]
    if obj then
      -- return true to remove element
      if fn(obj, o) == true then
        table.insert(removals, o)
      end
    else 
      table.insert(removals, o)
    end 
  end
  if #removals > 0 then
    for i = #removals, 1, -1 do
      table.remove(t, removals[i])
    end
  end
end

table.find = function(t, value)
  for i, v in ipairs(t) do 
    if v == value then return i end 
  end 
  return 0
end

table.keySort = function(t, key, default)
  if #t == 0 then return end
  table.sort(t, function(a, b)
    if a == nil and b == nil then
        return false
    end
    if a == nil then
        return true
    end
    if b == nil then
        return false
    end
    if a[key] == nil then a[key] = default end
    if b[key] == nil then b[key] = default end
    return a[key] < b[key]
  end)
end

table.update = function (t, new_t, depth, hits)
  hits = hits or {}
  depth = depth or -1
  for k,v in pairs(new_t) do
    if type(t[k]) ~= 'table' or type(v) ~= 'table' or depth == 0 or (type(v) == 'table' and hits[v]) then 
      t[k] = v    
    else
      if type(v) == 'table' then 
        hits[v] = true 
      end
      table.update(t[k], v, depth - 1, hits)
    end
  end
end

table.defaults = function (t,defaults)
  for k,v in pairs(defaults) do
      if type(t) == 'table' and t[k] == nil then t[k] = v
      elseif type(v) == 'table' then table.defaults(t[k],defaults[k]) end
  end
  return t
end

-- STRING 

function string:starts(start)
  return string.sub(self,1,string.len(start))==start
 end
 function string:contains(q)
   return string.match(tostring(self), tostring(q)) ~= nil
 end
 function string:count(str)
   local _, count = string.gsub(self, str, "")
   return count
 end
 function string:capitalize()
   return string.upper(string.sub(self,1,1))..string.sub(self,2)
 end
 function string:split(sep)
   local sep, fields = sep or ":", {}
   local pattern = string.format("([^%s]+)", sep)
   self:gsub(pattern, function(c) fields[#fields+1] = c end)
   return fields
 end
 function string:replace(find, replace, wholeword)
   if wholeword then
       find = '%f[%a]'..find..'%f[%A]'
   end
   return (self:gsub(find,replace))
 end

-- MATH 

local sin, cos, rad, deg, abs, min, max = math.sin, math.cos, math.rad, math.deg, math.abs, math.min, math.max
floor = function(x) return math.floor(x+0.5) end
math.to2D = function(i, columns)
  return math.floor((i - 1) % columns) + 1,
         math.floor((i - 1) / columns) + 1
end
math.to1D = function(x, y, columns)
  return x + y * columns
end
math.move = function(x, y, angle, distance) -- TODO: try swapping cos and sin
  return x + cos(angle) * distance,
         y + sin(angle) * distance
end

-- ETC

memoize = nil
do
  local mem_cache = {}
  setmetatable(mem_cache, {__mode = "kv"})
  memoize = function(f, cache)
      -- default cache or user-given cache?
      cache = cache or mem_cache
      if not cache[f] then 
          cache[f] = {}
      end 
      cache = cache[f]
      return function(...)
          local args = {...}
          local cache_str = '<no-args>'
          local found_args = false
          for i, v in ipairs(args) do
              if v ~= nil then 
                  if not found_args then 
                      found_args = true 
                      cache_str = ''
                  end

                  if i ~= 1 then 
                      cache_str = cache_str .. '~'
                  end 
                  if type(v) == "table" then
                      cache_str = cache_str .. tbl_to_str(v)
                  else
                      cache_str = cache_str .. tostring(v)
                  end
              end
          end 
          -- retrieve cached value?
          local ret = cache[cache_str]
          if not ret then
              -- not cached yet
              ret = { f(unpack(args)) }
              cache[cache_str] = ret 
              -- print('store',cache_str,'as',unpack(ret))
          end
          return unpack(ret)
      end
  end
end 

monitor = class{
  init = function(self)
    self._cache = {}
  end,

  track = function(self, t, ...)
    for s = 1, select("#", ...) do 
      self:reset(t, select(s, ...))
    end
  end,

  changed = function(self, t, k, reset)
    local res = self._cache[t] and self._cache[t][k] ~= t[k]
    if reset then 
      self:reset(t, k)
    end
    return res
  end,  

  reset = function(self, t, k)
    if not self._cache[t] then 
      self._cache[t] = {}
    end
    self._cache[t][k] = t[k]
  end,

  untrack = function(self, t, k)
    if not k then 
      self._cache[t] = nil 
    elseif self._cache[t] then 
      self._cache[t][k] = nil 
    end
  end
}

callable = function(t)
  local mt = { __call = t.__call }
  if t.__ then
      for mm, fn in pairs(t.__) do mt['__'..mm] = t.__[mm] end
  end
  return setmetatable(t, mt)
end

copy = function(orig, copies)
  copies = copies or {}
  local orig_type = type(orig)
  local t_copy
  if orig_type == 'table' then
      if copies[orig] then
          t_copy = copies[orig]
      else
          t_copy = {}
          copies[orig] = t_copy
          for orig_key, orig_value in next, orig, nil do
              t_copy[copy(orig_key, copies)] = copy(orig_value, copies)
          end
          setmetatable(t_copy, copy(getmetatable(orig), copies))
      end
  else -- number, string, boolean, etc
      t_copy = orig
  end
  return t_copy
end

