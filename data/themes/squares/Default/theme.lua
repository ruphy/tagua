import("../../pieces/common.lua")
import("selection.lua")

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

function alone(color)
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

theme.name = "Default"
theme.description = "Default squares"
theme.variants = { "Chess[default]", "Shogi", "XiangQi" }
