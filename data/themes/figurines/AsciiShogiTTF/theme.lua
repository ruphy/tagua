import("piece_theme.lua")
import("shogi_themelib.lua")

function Fig(char)
  return Glyph("../ShogiTTF/mikachan.ttf", char, 2)
end

function Plain(str)
  return Glyph(nil, str, 2)
end

theme.num_9 = Plain("i")
theme.num_8 = Plain("h")
theme.num_7 = Plain("g")
theme.num_6 = Plain("f")
theme.num_5 = Plain("e")
theme.num_4 = Plain("d")
theme.num_3 = Plain("c")
theme.num_2 = Plain("b")
theme.num_1 = Plain("a")

theme.phoenix		= Plain("Ph")
theme.crane		= Plain("Cr")
theme.pheasant		= Plain("Pt")
theme.right_quail	= Plain("RQ")
theme.left_quail	= Plain("LQ")
theme.falcon		= Plain("Fa")
theme.swallow		= Plain("Sw")

theme.king1 		= Plain("K")
theme.king2 		= Plain("K")
theme.rook 		= Plain("R")
theme.bishop 		= Plain("B")
theme.gold 		= Plain("G")
theme.silver 		= Plain("S")
theme.knight 		= Plain("N")
theme.lance 		= Plain("L")
theme.pawn 		= Plain("P")
theme.drunken_elephant 	= Plain("DE")
