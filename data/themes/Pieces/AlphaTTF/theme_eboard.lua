import("../common.lua")

shadow           = 7
shadow_color     = "#404040"
shadow_offset_x  = 3
shadow_offset_y  = 3
shadow_grow      = 6

function fromGlyphGrad(fg, grad1, grad2)
  return function(glyph)
    return function(size)
      --local g = LinearGradient(Point(0.3*size,0), Point(0.7*size,0))
      local g = RadialGradient(Point(size*0.8,size*0.8), size*0.99)
      g[1] = grad1
      g[0] = grad2
      return fromFontGlyph("Alpha.ttf", glyph, fg, g)(size)
    end
  end
end

fromGlyphW = fromGlyphGrad("black", "#627183", "#DEEAEE")
fromGlyphB = fromGlyphGrad("black", "#292052", "#73697B")

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

name = "Alpha (Eboard style)"
description = "Alpha TTF theme.\n  by Eric Bentzen"
variants = { "Chess" }
