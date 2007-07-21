import("piece_theme.lua")
shadow           = 7
shadow_color     = "#404040"
shadow_offset_x  = 3
shadow_offset_y  = 3
shadow_grow      = 6

function fromGlyphBG(fg, background)
  return function(glyph)
    return function(size)
      local i = Image(background)
      local b = Brush(i)
      b:scale(size/i.width, size/i.height)
      return fromFontGlyph("Alpha.ttf", glyph, fg, b)(size)
    end
  end
end

fromGlyphW = fromGlyphBG( "#1f1f1f", "gradient lightgrey.bmp")
fromGlyphB = fromGlyphBG( "#b1b1b1", "gradient darkgrey.bmp")

theme.white_bishop = fromGlyphW("0xF062")
theme.white_king   = fromGlyphW("0xF06B")
theme.white_knight = fromGlyphW("0xF068")
theme.white_pawn   = fromGlyphW("0xF070")
theme.white_queen  = fromGlyphW("0xF071")
theme.white_rook   = fromGlyphW("0x00A0")

theme.black_bishop = fromGlyphB("0xF062")
theme.black_king   = fromGlyphB("0xF06B")
theme.black_knight = fromGlyphB("0xF068")
theme.black_pawn   = fromGlyphB("0xF070")
theme.black_queen  = fromGlyphB("0xF071")
theme.black_rook   = fromGlyphB("0x00A0")

