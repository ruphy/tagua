import("piece_theme.lua")
import("selection.lua")

theme.background = function(size)
  local dark_square = "#73a26b"
  local light_square = "#c6c363"
  local i = Image(size*2,size*2)
  i:fill_rect(Rect(0,0,size,size), light_square)
  i:fill_rect(Rect(size,0,size,size), dark_square)
  i:fill_rect(Rect(0,size,size,size), dark_square)
  i:fill_rect(Rect(size,size,size,size), light_square)
  i:draw_line(Point(0,0),Point(2*size,0), "black", 2);
  i:draw_line(Point(0,size),Point(2*size,size), "black", 2);
  i:draw_line(Point(0,2*size),Point(2*size,2*size), "black", 2);
  i:draw_line(Point(0,0),Point(0,2*size), "black", 2);
  i:draw_line(Point(size,0),Point(size,2*size), "black", 2);
  i:draw_line(Point(2*size,0),Point(2*size,2*size), "black", 2);
  return i
end

theme.validmove = fromColor("#bdaede")

