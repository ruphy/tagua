import("../../Pieces/common.lua")

function theme.background(size)
  local light = Brush(Image("marble04.jpg"))
  local dark = Brush(Image("marble13.jpg"))
  local img = Image(size*8,size*8)
  for x = 0,7 do
    for y = 0,7 do
      if ((x+y) % 2) > 0.5 then
        img:fill_rect(Rect(x*size,y*size,size,size), dark)
      else
        img:fill_rect(Rect(x*size,y*size,size,size), light)
      end
    end
  end
  return img
end

theme.validmove = fromColor("#bdaede")
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

name = "Jose"
description = "Jose default squares"
variants = { "Chess" }
