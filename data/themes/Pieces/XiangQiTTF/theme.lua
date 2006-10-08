import("../common.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

white_col = "#d00000"
black_col = "#000070"
white_col2 = "#a00000"
black_col2 = "#000000"

function fromChar(char, white)
  return function(size)
    local i = Image(size,size)
    i:clear()
    local g = RadialGradient(Point(size*0.5,size*0.5), size*0.5, Point(size*0.3,size*0.3))
    g[0] = "#ffeda2"
    g[1] = "#c0a870"
    i:draw_glyph(Rect(0,0,size,size), "XiangQi.ttf", "0x2c", "black", g, 0, true)
    i:draw_glyph(Rect(0,0,size,size), "XiangQi.ttf", "0x2e",
                                      white and white_col2 or black_col2,
                                      "#fff3c8", 5, false)
    i:draw_glyph(Rect(0,0,size,size), "XiangQi.ttf", char,
                                      white and white_col or black_col,
                                      "#fff3c8", 4, false)
    return i
  end
end

theme.black_advisor   = addShadow(fromChar("0x41", false))
theme.black_cannon    = addShadow(fromChar("0x43", false))
theme.black_elephant  = addShadow(fromChar("0x45", false))
theme.black_general   = addShadow(fromChar("0x47", false))
theme.black_horse     = addShadow(fromChar("0x48", false))
theme.black_chariot   = addShadow(fromChar("0x52", false))
theme.black_soldier   = addShadow(fromChar("0x53", false))
theme.white_advisor   = addShadow(fromChar("0x61", true))
theme.white_cannon    = addShadow(fromChar("0x63", true))
theme.white_elephant  = addShadow(fromChar("0x65", true))
theme.white_general   = addShadow(fromChar("0x67", true))
theme.white_horse     = addShadow(fromChar("0x68", true))
theme.white_chariot   = addShadow(fromChar("0x72", true))
theme.white_soldier   = addShadow(fromChar("0x73", true))

-- To be able to adapt this theme to chess too
theme.black_bishop = theme.black_advisor
theme.black_king   = theme.black_general
theme.black_knight = theme.black_horse
theme.black_pawn   = theme.black_soldier
theme.black_queen  = theme.black_elephant
theme.black_rook   = theme.black_chariot
theme.white_bishop = theme.white_advisor
theme.white_king   = theme.white_general
theme.white_knight = theme.white_horse
theme.white_pawn   = theme.white_soldier
theme.white_queen  = theme.white_elephant
theme.white_rook   = theme.white_chariot

name = "XiangQi"
description = "XiangQi TTF theme,\n  created with potrace"
variants = { "Chess", "XiangQi[Default]" }
