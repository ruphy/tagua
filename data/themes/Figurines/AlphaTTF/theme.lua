import("../../Pieces/common.lua")

function Fig(char)
  return Glyph("../../Pieces/AlphaTTF/Alpha.ttf", char)
end

bishop = Fig("0xF062")
king   = Fig("0xF06B")
knight = Fig("0xF068")
pawn   = Fig("0xF070")
queen  = Fig("0xF071")
rook   = Fig("0x00A0")

name = "Alpha"
description = "Alpha TTF figurines.\n  by Eric Bentzen"
variants = { "Chess[default]" }
