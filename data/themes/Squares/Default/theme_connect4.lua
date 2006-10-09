import("../../Pieces/common.lua")

-- function theme.background(size)
--   local i = Image(size*7,size*7)
--   i:fill_rect(Rect(0,0,size*7,size), "#8090a0")
--   i:fill_rect(Rect(0,size,size*7,size*6), "#2030d0")
--   for x=0,6 do
--     i:draw_svg(Rect(x*size,0,size,size), "arrow_grey.svg")
--   end
--   for y=1,6 do
--     for x=0,6 do
--       i:draw_svg(Rect(x*size,y*size,size,size), "spiral.svg")
--     end
--   end
--   return i
-- end

function theme.background(size)
  local upper = Image(size,size)
  local lower = Image(size,size)
  upper:fill_rect(Rect(0,0,size,size), "#8090a0")
  upper:draw_svg(Rect(0,0,size,size), "arrow_grey.svg")
  lower:fill_rect(Rect(0,0,size,size), "#2030d0")
  lower:draw_svg(Rect(0,0,size,size), "spiral.svg")
  return { [Rect(0,0,size*7,size)] = upper, [Rect(0,size,size*7,size*6)] = lower }
end

theme.highlighting = fromSVG("arrow_yellow.svg")
theme.validmove = fromColor("#bdaede")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

name = "Default"
description = "Default squares"
variants = { "Connect4[default]" }
