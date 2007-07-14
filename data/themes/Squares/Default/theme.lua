import("../../Pieces/common.lua")

theme.background = function (size)
  local dark_square = Color("#6b829c")
  local light_square = Color("#b8c0c0")
  dark_square.a = 224
  light_square.a = 224
  local i = Image(size*2,size*2)
  i:set_paint_over(false);
  i:fill_rect(Rect(0,0,size,size), light_square)
  i:fill_rect(Rect(size,0,size,size), dark_square)
  i:fill_rect(Rect(0,size,size,size), dark_square)
  i:fill_rect(Rect(size,size,size,size), light_square)
  return i
end

function addShadow(i, size)
  local s = i:create_shadow( size/6, "#ffffff", Point(0, 0), Point(0, 0) )
  s:draw_image(Rect(0, 0, i.width, i.height), i)
  return s
end

theme.border = function(size)
  local b = math.floor(size*2/3);
  local rb = math.floor(b*2/3);
  local s8 = size*8;
  local tr = Image(b, b);  tr:clear(); tr:draw_svg(Rect(0,b-rb,rb,rb), "corner.svg"); tr = addShadow(tr, size)
  local tl = Image(b, b);  tl:set_paint_over(false); tl:rotate(270); tl:translate(0,b); tl:draw_image(Rect(0,0,b,b), tr)
  local bl = Image(b, b);  bl:set_paint_over(false); bl:rotate(180); bl:translate(b,b); bl:draw_image(Rect(0,0,b,b), tr)
  local br = Image(b, b);  br:set_paint_over(false); br:rotate(90); br:translate(b,0); br:draw_image(Rect(0,0,b,b), tr)
  local t = Image(s8, b);  t:clear(); t:draw_svg(Rect(0,b-rb,s8,rb), "bar.svg"); t = addShadow(t, size)
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

print "hello!!!"

theme.wallpaper = function()
  print "wallpaper."
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
  return Rect(w*0.02, h*0.2, w*0.4, h*0.38)
end

theme.clock_time_rect = function(size)
  local w = size*ratio
  local h = math.floor(w*2/5)
  return Rect(w*0.4, h*0.06, w*0.58, h*0.66)
end

theme.clock_player_rect = function(size)
  local w = size*ratio
  local h = math.floor(w*2/5)
  return Rect(w*0.14, h*0.68, w*0.69, h*0.28)
end

function alone(color)
  return function(size)
    local i = Image(size,size)
    local g = RadialGradient(Point(size*0.485,size*0.485), size*0.54)
    g[0] = Color(0,0,0,0)
    g[0.2] = Color(0,0,0,0)
    g[0.85] = color
    g[1] = color
    i:clear()
    i:fill_rect(Rect(0,0,size,size), g)
    return i
  end
end

function alone2(color)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_line(Point(size*0.1,size*0.1), Point(size*0.9,size*0.1), color, size*0.1);
    i:draw_line(Point(size*0.1,size*0.9), Point(size*0.9,size*0.9), color, size*0.1);
    i:draw_line(Point(size*0.1,size*0.1), Point(size*0.1,size*0.9), color, size*0.1);
    i:draw_line(Point(size*0.9,size*0.1), Point(size*0.9,size*0.9), color, size*0.1);
    local a = Color(color)
    a.a = 64
    i:fill_rect(Rect(size*0.1,size*0.1,size*0.8,size*0.8), a);
    i:exp_blur(size*0.1);
    return i
  end
end

theme.validmove = fromColor("#cd9eee")
theme.highlighting = alone2("#80ff80")
theme.premove = alone2("#4040ff")
theme.selection = alone2("#ff4040")

theme.name = "Default"
theme.description = "Default squares"
theme.variants = { "Chess[default]", "Shogi", "XiangQi" }
