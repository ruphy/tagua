import("../../Pieces/common.lua")

options = OptList {
    ColorOpt("light", "Light square", "white"),
    ColorOpt("dark", "Dark square", "black")
}

function theme.background(size)
  local dark_square = options.dark.value
  local light_square = options.light.value
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

name = "Custom Color"
description = "Squares with a solid custom color."
variants = { "Chess", "Shogi", "XiangQi" }
