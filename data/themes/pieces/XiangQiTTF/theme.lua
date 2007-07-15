import("../common.lua")

shadow=5.0
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

theme.black_advisor   = addShadow(fromChar("0x61", false))
theme.black_cannon    = addShadow(fromChar("0x63", false))
theme.black_elephant  = addShadow(fromChar("0x65", false))
theme.black_general   = addShadow(fromChar("0x67", false))
theme.black_horse     = addShadow(fromChar("0x68", false))
theme.black_chariot   = addShadow(fromChar("0x72", false))
theme.black_soldier   = addShadow(fromChar("0x73", false))
theme.red_advisor   = addShadow(fromChar("0x41", true))
theme.red_cannon    = addShadow(fromChar("0x43", true))
theme.red_elephant  = addShadow(fromChar("0x45", true))
theme.red_general   = addShadow(fromChar("0x47", true))
theme.red_horse     = addShadow(fromChar("0x48", true))
theme.red_chariot   = addShadow(fromChar("0x52", true))
theme.red_soldier   = addShadow(fromChar("0x53", true))

-- To be able to adapt this theme to chess too
theme.black_bishop = theme.black_advisor
theme.black_king   = theme.black_general
theme.black_knight = theme.black_horse
theme.black_pawn   = theme.black_soldier
theme.black_queen  = theme.black_elephant
theme.black_rook   = theme.black_chariot
theme.white_bishop = theme.red_advisor
theme.white_king   = theme.red_general
theme.white_knight = theme.red_horse
theme.white_pawn   = theme.red_soldier
theme.white_queen  = theme.red_elephant
theme.white_rook   = theme.red_chariot

theme.name = "XiangQi"
theme.description = "XiangQi TTF theme,\n  created with potrace"
theme.variants = { "Chess", "XiangQi[Default]" }
