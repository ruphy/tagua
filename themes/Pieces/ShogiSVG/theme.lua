import("../common.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

function fromSVG(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(0,0,size,size), "tile.svg")
    i:draw_svg(Rect(0,0,size,size), file)
    return i
  end
end

function fromSVG_ROT(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:translate(size,size)
    i:rotate(180)
    i:draw_svg(Rect(0,0,size,size), "tile.svg")
    i:draw_svg(Rect(0,0,size,size), file)
    return i
  end
end

theme.black_bishop = addShadow(fromSVG_ROT("bishop.svg"))
theme.black_king   = addShadow(fromSVG_ROT("king.svg"))
theme.black_knight = addShadow(fromSVG_ROT("horse.svg"))
theme.black_pawn   = addShadow(fromSVG_ROT("pawn.svg"))
theme.black_queen  = addShadow(fromSVG_ROT("gold.svg"))
theme.black_rook   = addShadow(fromSVG_ROT("rook.svg"))
theme.white_bishop = addShadow(fromSVG("bishop.svg"))
theme.white_king   = addShadow(fromSVG("king.svg"))
theme.white_knight = addShadow(fromSVG("horse.svg"))
theme.white_pawn   = addShadow(fromSVG("pawn.svg"))
theme.white_queen  = addShadow(fromSVG("gold.svg"))
theme.white_rook   = addShadow(fromSVG("rook.svg"))

name = "Shogi"
description = "Shogi SVG theme,\n  generate with potrace"
variants = { "Chess[default]" }
