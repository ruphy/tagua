import("piece_theme.lua")

function Fig(char)
  return Glyph("mikachan.ttf", char, 2)
end

theme.num_9 = Fig("0x4e5d")
theme.num_8 = Fig("0x516b")
theme.num_7 = Fig("0x4e03")
theme.num_6 = Fig("0x516d")
theme.num_5 = Fig("0x4e94")
theme.num_4 = Fig("0x56db")
theme.num_3 = Fig("0x4e09")
theme.num_2 = Fig("0x4e8c")
theme.num_1 = Fig("0x4e00")

theme.king1   = Fig("0x738b")
theme.king2   = Fig("0x7389")
theme.rook   = Fig("0x98db")
theme.p_rook   = Fig("0x9f8d")
theme.bishop = Fig("0x89d2")
theme.p_bishop = Fig("0x99ac")
theme.gold   = Fig("0x91d1")
theme.silver = Fig("0x9280")
theme.p_silver = Fig("0x5168")
theme.knight = Fig("0x6842")
theme.p_knight = Fig("0x572d")
theme.lance  = Fig("0x9999")
theme.p_lance  = Fig("0x674f")
theme.pawn   = Fig("0x6b69")
theme.p_pawn   = Fig("0x3068")


