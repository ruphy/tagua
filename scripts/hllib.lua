-------------------------------------------------------------------------------
--   Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
--             (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
--             
--   This program is free software; you can redistribute it and/or modify
--   it under the terms of the GNU General Public License as published by
--   the Free Software Foundation; either version 2 of the License, or
--   (at your option) any later version.
-------------------------------------------------------------------------------

Text = {
  create_from_hline = function(hline, from, to)
    local o = {
      line = hline,
      from = from,
      to = to
    }
    setmetatable(o, Text[".object_meta_table"])
    return o
  end,
  [".getter_table"] = {
    text = function(self)
      return self.line:mid(self.from, self.to)
    end
  },
  [".method_table"] = {
    dump = function(self)
      self.line:dump()
    end,
    clone = function(self)
      local hline = self.line:extract(self.from, self.to)
      return Text.create_from_hline(hline, 0, hline.length)
    end
  },
  [".meta_table"] = {
    __call = function(prototype, str)
      local hline = HLine(str)
      return prototype.create_from_hline(hline, 0, hline.length)
    end
  },
  [".object_meta_table"] = {
    __index = function(self, property)
      local h = Text[".getter_table"][property]
      if h ~= nil then
        return h(self)
      else
        h = Text[".method_table"][property]
        if h ~= nil then
          return h
        else
          return nil
        end
      end
    end,
  
    __newindex = function(self, property, value)
      self.line["set_" .. property](self.line, self.from, self.to, value)
    end,
    
    __call = function(self, from, to)
      if (to > self.to) then 
        error("Index " .. to .. " is too large")
      end
      return Text.create_from_hline(self.line, from + self.from, to + self.from)
    end,
    
    __add = function(self, other)
      local hline = self:clone().line:append(other.line, other.from, other.to)
      return Text.create_from_hline(hline, 0, hline.length)
    end
  }
}
setmetatable(Text, Text[".meta_table"])

__patterns__ = { }

on = {}
highlight = {}

setmetatable(on, {
  __newindex = function(self, re, h)
    local event = {
      pattern = re,
      handler = h
    }
    table.insert(__patterns__, event)
  end,
  __call = function(self, re, h)
    self[re] = h
  end
})

setmetatable(highlight, {
  __newindex = function(self, re, fmt)
    local function handler()
      local function set_property(prop)
        if fmt[prop] ~= nil then
          match[prop] = fmt[prop]
        end
      end
      set_property("bold")
      set_property("italic")
      set_property("color")
      return line
    end
    table.insert(__patterns__, { pattern = re, handler = handler})
  end,
  
  __call = function(self, re, fmt)
    self[re] = fmt
  end
})

function filter(re)
  table.insert(__patterns__, { pattern = re, handler = function() end})
end

function __run_event__(event, str, match, ref, state)
  env = getfenv(event.handler)
  env.line = Text(str)
  env.match = env.line(match.from, match.to)
  env.ref = {}
  for i, r in ipairs(ref) do
    env.ref[i] = env.line(ref[i].from, ref[i].to)
  end
  for k, v in pairs(state) do
    env[k] = v
  end
  setfenv(event.handler, env)
  
  local result = event.handler()
  
  if result ~= nil then
    return result
  end
end
