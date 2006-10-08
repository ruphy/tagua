import("../common.lua")

shadow           = 7
shadow_color     = "#404040"
shadow_offset_x  = 3
shadow_offset_y  = 3
shadow_grow      = 6

function fromGlyph(glyph)
  return fromFontGlyph("Alpha.ttf", glyph, "black", "white", 6.0)
end

theme.white_bishop = fromGlyph("0xF062")
theme.white_king   = fromGlyph("0xF06B")
theme.white_knight = fromGlyph("0xF068")
theme.white_pawn   = fromGlyph("0xF070")
theme.white_queen  = fromGlyph("0xF071")
theme.white_rook   = fromGlyph("0x00A0")

theme.black_bishop = fromGlyph("0xF06E")
theme.black_king   = fromGlyph("0xF06C")
theme.black_knight = fromGlyph("0xF06A")
theme.black_pawn   = fromGlyph("0xF06F")
theme.black_queen  = fromGlyph("0xF077")
theme.black_rook   = fromGlyph("0xF074")

name = "Alpha (Jose style)"
description = "Alpha TTF theme.\n  by Eric Bentzen"
variants = { "Chess" }
