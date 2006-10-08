import("../../Pieces/common.lua")

function theme.background(size)
  local i = Image(size*7,size*7)
  i:fill_rect(Rect(0,0,size*7,size), "#8090a0")
  i:fill_rect(Rect(0,size,size*7,size*6), "#2030d0")
  for x=0,6 do
    i:draw_svg(Rect(x*size,0,size,size), "arrow_grey.svg")
  end
  for y=1,6 do
    for x=0,6 do
      i:draw_svg(Rect(x*size,y*size,size,size), "spiral.svg")
    end
  end
  return i
end

theme.highlighting = fromSVG("arrow_yellow.svg")
theme.validmove = fromColor("#bdaede")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

name = "Default"
description = "Default squares"
variants = { "Connect4[default]" }
