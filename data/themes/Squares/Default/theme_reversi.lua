import("../../Pieces/common.lua")

function theme.background(size)
  local i = Image(size,size)
  i:clear("#008040");
  i:draw_line(Point(0,0),Point(size,0), "black", 2);
  i:draw_line(Point(0,size),Point(size,size), "black", 2);
  i:draw_line(Point(0,0),Point(0,size), "black", 2);
  i:draw_line(Point(size,0),Point(size,size), "black", 2);
  return i
end

theme.validmove = fromColor("#bdaede")
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

name = "Default"
description = "Default squares"
variants = { "Reversi[default]" }
