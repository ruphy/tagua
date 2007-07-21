import("piece_theme.lua")
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


