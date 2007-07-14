import("../../Pieces/common.lua")

theme.background = function(size)
  local i = Image(size,size)
  i:clear("#a0a0a0");
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

theme.name = "Default"
theme.description = "Default squares"
theme.variants = { "ChainReaction[default]" }

