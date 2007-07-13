import("../../Pieces/common.lua")

function theme.background(size)
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

function theme.border(size)
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

theme.wallpaper = function(size)
                    --return Image("Wallpapers/dragonballz.jpg", false)
                    return Image("Wallpapers/Bled (nastja).jpg", false)
                  end

theme.validmove = fromColor("#cd9eee")
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

name = "Default"
description = "Default squares"
variants = { "Chess[default]", "Shogi", "XiangQi" }
