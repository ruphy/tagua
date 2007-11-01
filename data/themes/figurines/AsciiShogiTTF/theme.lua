import("piece_theme.lua")
import("shogi_themelib.lua")

function Fig(char)
  return Glyph("../ShogiTTF/mikachan.ttf", char, 2)
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

theme.phoenix		= Fig("Ph")
theme.crane		= Fig("Cr")
theme.pheasant		= Fig("Pt")
theme.right_quail	= Fig("RQ")
theme.left_quail	= Fig("LQ")
theme.falcon		= Fig("Fa")
theme.swallow		= Fig("Sw")
