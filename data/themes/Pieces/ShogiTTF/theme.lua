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

function fromChar(char, white, promoted, ratio)
  return function(size)
    local i = Image(size,size)
    i:clear()


    local g = RadialGradient(Point(size*0.5,size*0.5), size*0.5, Point(size*0.3,size*0.3))
    g[0] = "#ffeda2"
    g[1] = "#c0a870"
    local b = Brush(g)

    if white then
      i:translate(-size, -size)
      i:rotate(180)
      b:translate(-size, -size)
      b:rotate(180)
    end

    if ratio then
      i:scale(ratio, ratio)
      i:translate(size*(1-ratio)*0.5, size*(1-ratio)*0.5)
      b:scale(ratio, ratio)
      b:translate(size*(1-ratio)*0.5, size*(1-ratio)*0.5)
    end

    if true then
      i:draw_glyph(Rect(0,0,size,size), "Shogi.ttf", "0x2c", "black", b)
      i:draw_glyph(Rect(0,0,size,size), "Shogi.ttf", char,
                                      promoted and "#d00000" or "#004000",
                                      "#fff3c8", 4, false)
    else
      i:fill_rect(Rect(0,0,size,size), b);
    end
    return i
  end
end

theme.black_king      = addShadow(fromChar("0x6B", false, false))
theme.black_rook      = addShadow(fromChar("0x72", false, false, 0.96))
theme.black_p_rook    = addShadow(fromChar("0x52", false, true, 0.96))
theme.black_bishop    = addShadow(fromChar("0x62", false, false, 0.93))
theme.black_p_bishop  = addShadow(fromChar("0x42", false, true, 0.93))
theme.black_gold      = addShadow(fromChar("0x67", false, false, 0.9))
theme.black_silver    = addShadow(fromChar("0x73", false, false, 0.9))
theme.black_p_silver  = addShadow(fromChar("0x53", false, true, 0.9))
theme.black_knight     = addShadow(fromChar("0x68", false, false, 0.86))
theme.black_p_knight   = addShadow(fromChar("0x48", false, true, 0.86))
theme.black_lance     = addShadow(fromChar("0x6C", false, false, 0.83))
theme.black_p_lance   = addShadow(fromChar("0x4C", false, true, 0.83))
theme.black_pawn      = addShadow(fromChar("0x70", false, false, 0.8))
theme.black_p_pawn    = addShadow(fromChar("0x50", false, true, 0.8))

theme.white_king      = addShadow(fromChar("0x6B", true, false))
theme.white_rook      = addShadow(fromChar("0x72", true, false, 0.96))
theme.white_p_rook    = addShadow(fromChar("0x52", true, true, 0.96))
theme.white_bishop    = addShadow(fromChar("0x62", true, false, 0.93))
theme.white_p_bishop  = addShadow(fromChar("0x42", true, true, 0.93))
theme.white_gold      = addShadow(fromChar("0x67", true, false, 0.9))
theme.white_silver    = addShadow(fromChar("0x73", true, false, 0.9))
theme.white_p_silver  = addShadow(fromChar("0x53", true, true, 0.9))
theme.white_knight     = addShadow(fromChar("0x68", true, false, 0.86))
theme.white_p_knight   = addShadow(fromChar("0x48", true, true, 0.86))
theme.white_lance     = addShadow(fromChar("0x6C", true, false, 0.83))
theme.white_p_lance   = addShadow(fromChar("0x4C", true, true, 0.83))
theme.white_pawn      = addShadow(fromChar("0x70", true, false, 0.8))
theme.white_p_pawn    = addShadow(fromChar("0x50", true, true, 0.8))

-- To be able to adapt this theme to chess too
theme.black_queen  = theme.black_gold
theme.white_queen  = theme.white_gold

theme.name = "Shogi"
theme.description = "Shogi TTF theme,\n  created with potrace"
theme.variants = { "Chess", "Shogi[Default]" }
