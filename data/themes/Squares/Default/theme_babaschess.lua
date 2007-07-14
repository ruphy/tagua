import("../../Pieces/common.lua")

theme.background = function(size)
  local dark_square = "#008284"
  local light_square = "#c6dfc6"
  local i = Image(size*2,size*2)
  i:fill_rect(Rect(0,0,size,size), light_square)
  i:fill_rect(Rect(size,0,size,size), dark_square)
  i:fill_rect(Rect(0,size,size,size), dark_square)
  i:fill_rect(Rect(size,size,size,size), light_square)
  return i
end

theme.validmove = fromColor("#ffff8c")
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

theme.name = "BabasChess"
theme.description = "BabasChess default squares"
theme.variants = { "Chess" }
