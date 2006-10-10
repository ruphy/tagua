import("../../Pieces/common.lua")

function Fig(char)
  --return Glyph("../../Pieces/AlphaTTF/Alpha.ttf", char)
  return Glyph(char)
end

num_9 = Fig("0x4e5d")
num_8 = Fig("0x516b")
num_7 = Fig("0x4e03")
num_6 = Fig("0x516d")
num_5 = Fig("0x4e94")
num_4 = Fig("0x56db")
num_3 = Fig("0x4e09")
num_2 = Fig("0x4e8c")
num_1 = Fig("0x4e00")

king   = Fig("0x738b")
rook   = Fig("0x98db")
p_rook   = Fig("0x9f8d")
bishop = Fig("0x89d2")
p_bishop = Fig("0x99ac")
gold   = Fig("0x91d1")
silver = Fig("0x9280")
p_silver = Fig("0x5168")
knight = Fig("0x6842")
p_knight = Fig("0x572d")
lance  = Fig("0x9999")
p_lance  = Fig("0x674f")
pawn   = Fig("0x6b69")
p_pawn   = Fig("0x3068")


name = "Shogi"
description = "Shogi figurines."
variants = { "Shogi[default]" }
