import("../../Pieces/common.lua")

theme.options = OptList {
    ColorOpt("light", "Light square", "white"),
    ColorOpt("dark", "Dark square", "black")
}

theme.background = function(size)
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

theme.name = "Custom Color"
theme.description = "Squares with a solid custom color."
theme.variants = { "Chess", "Shogi", "XiangQi" }
