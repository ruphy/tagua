import("../../pieces/common.lua")

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
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

theme.name = "XBoard"
theme.description = "XBoard default squares"
theme.variants = { "Chess" }
