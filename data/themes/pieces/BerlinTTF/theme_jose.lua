-- this theme copies perfectly the default theme of jose,
-- except for the shadow that is better :)

import("piece_theme.lua")
shadow           = 7
shadow_color     = "#404040"
shadow_offset_x  = 3
shadow_offset_y  = 3
shadow_grow      = 6

function fromGlyph(glyph)
  return fromFontGlyph("Berlin.ttf", glyph, "black", "white", 6.0)
end

theme.black_bishop = fromGlyph("0x006E")
theme.black_king   = fromGlyph("0x006C")
theme.black_knight = fromGlyph("0x006A")
theme.black_pawn   = fromGlyph("0x006F")
theme.black_queen  = fromGlyph("0x0077")
theme.black_rook   = fromGlyph("0x0074")

theme.white_bishop = fromGlyph("0x0062")
theme.white_king   = fromGlyph("0x006B")
theme.white_knight = fromGlyph("0x0068")
theme.white_pawn   = fromGlyph("0x0070")
theme.white_queen  = fromGlyph("0x0071")
theme.white_rook   = fromGlyph("0x0072")

theme.name = "Berlin (Jose style)"
theme.description = "Berlin TTF theme,\nas Jose's default.\n  by Eric Bentzen"
theme.variants = { "Chess" }
