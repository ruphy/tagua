import("../../Pieces/common.lua")

function Fig(char)
  return Glyph("../../Pieces/AlphaTTF/Alpha.ttf", char, 2)
end

bishop = Fig("0xF085")
king   = Fig("0xF082")
knight = Fig("0xF086")
pawn   = Fig("0xF087")
queen  = Fig("0xF083")
rook   = Fig("0xF084")

name = "Alpha"
description = "Alpha TTF figurines.\n  by Eric Bentzen"
variants = { "Chess" }
