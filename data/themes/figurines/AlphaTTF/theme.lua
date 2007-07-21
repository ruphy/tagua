import("piece_theme.lua")

function Fig(char)
  return Glyph("../../Pieces/AlphaTTF/Alpha.ttf", char, 2)
end

theme.bishop = Fig("0xF085")
theme.king   = Fig("0xF082")
theme.knight = Fig("0xF086")
theme.pawn   = Fig("0xF087")
theme.queen  = Fig("0xF083")
theme.rook   = Fig("0xF084")

