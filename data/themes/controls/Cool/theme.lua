import("../../pieces/common.lua")

function addShadow(i, size)
  local s = i:create_shadow( size, "#ffffff", Point(0, 0), Point(0, 0) )
  s:draw_image(Rect(0, 0, i.width, i.height), i)
  return s
end

--theme.view_ratio =
theme.layout = function(args)
  for key,value in pairs(args) do
    print("args[",key,"] = ",value)
  end
end

theme.border = function(size)
  local b = math.floor(size*2/3);
  local rb = math.floor(b*2/3);
  local s8 = size*8;
  local tr = Image(b, b);  tr:clear(); tr:draw_svg(Rect(0,b-rb,rb,rb), "corner.svg"); tr = addShadow(tr, size/6)
  local tl = Image(b, b);  tl:set_paint_over(false); tl:rotate(270); tl:translate(0,b); tl:draw_image(Rect(0,0,b,b), tr)
  local bl = Image(b, b);  bl:set_paint_over(false); bl:rotate(180); bl:translate(b,b); bl:draw_image(Rect(0,0,b,b), tr)
  local br = Image(b, b);  br:set_paint_over(false); br:rotate(90); br:translate(b,0); br:draw_image(Rect(0,0,b,b), tr)
  local t = Image(s8, b);  t:clear(); t:draw_svg(Rect(0,b-rb,s8,rb), "bar.svg"); t = addShadow(t, size/6)
  local bt = Image(s8, b); bt:set_paint_over(false); bt:rotate(180); bt:translate(s8,b); bt:draw_image(Rect(0,0,s8,b), t)
  local l = Image(b, s8);  l:set_paint_over(false); l:rotate(270); l:translate(0,s8); l:draw_image(Rect(0,0,s8,b), t)
  local r = Image(b, s8);  r:set_paint_over(false); r:rotate(90); r:translate(b,0); r:draw_image(Rect(0,0,s8,b), t)
  return {
    [Rect(-b,-b,b,b)] = tl,
    [Rect(s8,-b,b,b)] = tr,
    [Rect(-b,s8,b,b)] = bl,
    [Rect(s8,s8,b,b)] = br,
    [Rect(0,-b,s8,b)] = t,
    [Rect(0,s8,s8,b)] = bt,
    [Rect(-b,0,b,s8)] = l,
    [Rect(s8,0,b,s8)] = r
  }
end

theme.wallpaper = function()
  local i
  --i = Image("Wallpapers/autumn.jpg", false)
  --i = Image("Wallpapers/winter.jpg", false)
  i = Image("Wallpapers/dragonballz.jpg", false)
  --i = Image("Wallpapers/Bled (nastja).jpg", false)
  i:exp_blur(5);
  return i
end

ratio = 2.6;

function create_clock(file, col)
  return function(size)
    print("clock ",size)
    local w = size*ratio
    local h = math.floor(w*2/5)
    local i = Image(w,h)
    i:clear()
    i:draw_svg(Rect(0,0,w,h), file)
    local s = i:create_shadow( size/6, col, Point(math.floor(size/2.5), math.floor(size/2.5)), Point(0, 0) )
    s:draw_image(Rect(math.floor(size/5), math.floor(size/5), i.width, i.height), i)
    return s
  end
end

theme.clock_active_background = create_clock("active_clock.svg", "#ff8080")
theme.clock_inactive_background = create_clock("inactive_clock.svg", "#e0e0e0")

theme.clock_active_text = function(size)
  return Brush("#000000")
end

theme.clock_inactive_text = function(size)
  return Brush("#808080")
end

theme.clock_background_offset = function(size)
  return Point(-math.floor(size/5), -math.floor(size/5))
end

theme.clock_height = function(size)
  return math.floor((size*ratio)*2/5);
end

theme.clock_caption_rect = function(size)
  local w = size*ratio
  local h = math.floor(w*2/5)
  return Rect(w*0.02, h*0.22, w*0.4, h*0.38)
end

theme.clock_time_rect = function(size)
  local w = size*ratio
  local h = math.floor(w*2/5)
  return Rect(w*0.4, h*0.08, w*0.58, h*0.66)
end

theme.clock_player_rect = function(size)
  local w = size*ratio
  local h = math.floor(w*2/5)
  return Rect(w*0.14, h*0.68, w*0.69, h*0.28)
end

theme.name = "Cool"
theme.description = "Cool controls"
theme.variants = { "any" }
