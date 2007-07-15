import("../../pieces/common.lua")

theme.options = OptList {
    ColorOpt("light", "Light square", "white"),
    ColorOpt("dark", "Dark square", "black"),
    IntOpt("alpha", "Alpha", 192, 0, 255),
}

theme.background = function(size)
  local dark_square = theme.options.dark.value
  local light_square = theme.options.light.value
  dark_square.a = theme.options.alpha.value
  light_square.a = theme.options.alpha.value
  local i = Image(size*2,size*2)
  i:set_paint_over(false)
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
