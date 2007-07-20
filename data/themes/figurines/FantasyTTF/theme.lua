import("piece_theme.lua")

function Fig(char)
  return Glyph("Fantasy.ttf", char, 3)
end

theme.bishop = Fig("0x0062")
theme.king   = Fig("0x006b")
theme.knight = Fig("0x006e")
theme.pawn   = Fig("0x0070")
theme.queen  = Fig("0x0071")
theme.rook   = Fig("0x0072")

theme.name = "Fantasy"
theme.description = "Fantasy TTF figurines.\n  by Maurizio Monge"
theme.variants = { "Chess[default]" }
