import("../../Pieces/common.lua")

function Fig(char)
  return Glyph("Fantasy.ttf", char, 3)
end

bishop = Fig("0x0062")
king   = Fig("0x006b")
knight = Fig("0x006e")
pawn   = Fig("0x0070")
queen  = Fig("0x0071")
rook   = Fig("0x0072")

name = "Fantasy"
description = "Fantasy TTF figurines.\n  by Maurizio Monge"
variants = { "Chess[default]" }
