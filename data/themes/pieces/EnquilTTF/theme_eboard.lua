import("piece_theme.lua")

shadow           = 7
shadow_color     = "#404040"
shadow_offset_x  = 3
shadow_offset_y  = 3
shadow_grow      = 6

function fromGlyphGrad(fg, grad1, grad2)
  return function(glyph)
    return function(size)
      local g = LinearGradient(Point(0.1*size,0), Point(1*size,0.2*size))
      g[0] = grad1
      g[1] = grad2
      return fromFontGlyph("Enquil.ttf", glyph, fg, g)(size)
    end
  end
end

fromGlyphW = fromGlyphGrad("black", "#ffffff", "#b0b0c0")
fromGlyphB = fromGlyphGrad("white", "#404060", "#000000")

theme.black_bishop = fromGlyphB("0x0062")
theme.black_king   = fromGlyphB("0x006B")
theme.black_knight = fromGlyphB("0x006E")
theme.black_pawn   = fromGlyphB("0x0070")
theme.black_queen  = fromGlyphB("0x0071")
theme.black_rook   = fromGlyphB("0x0072")

theme.white_bishop = fromGlyphW("0x0062")
theme.white_king   = fromGlyphW("0x006B")
theme.white_knight = fromGlyphW("0x006E")
theme.white_pawn   = fromGlyphW("0x0070")
theme.white_queen  = fromGlyphW("0x0071")
theme.white_rook   = fromGlyphW("0x0072")

