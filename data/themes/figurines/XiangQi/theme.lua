import("../../Pieces/common.lua")

function Fig(char)
  return Glyph("ukai.ttf", char, 4)
end

rn_1 = Fig("0x4e00")
rn_2 = Fig("0x4e8c")
rn_3 = Fig("0x4e09")
rn_4 = Fig("0x56db")
rn_5 = Fig("0x4e94")
rn_6 = Fig("0x516d")
rn_7 = Fig("0x4e03")
rn_8 = Fig("0x516b")
rn_9 = Fig("0x4e5d")

bn_1 = Fig("0x0031")
bn_2 = Fig("0x0032")
bn_3 = Fig("0x0033")
bn_4 = Fig("0x0034")
bn_5 = Fig("0x0035")
bn_6 = Fig("0x0036")
bn_7 = Fig("0x0037")
bn_8 = Fig("0x0038")
bn_9 = Fig("0x0039")

theme.black_general   = Fig("0x5c07")
theme.red_general     = Fig("0x5e25")
theme.black_advisor   = Fig("0x58eb")
theme.red_advisor     = Fig("0x4ed5")
theme.black_elephant  = Fig("0x8c61")
theme.red_elephant    = Fig("0x76f8")
theme.black_horse     = Fig("0x99ac")
theme.red_horse       = Fig("0x508c")
theme.black_chariot   = Fig("0x8eca")
theme.red_chariot     = Fig("0x4fe5")
theme.black_cannon    = Fig("0x7832")
theme.red_cannon      = Fig("0x70ae")
theme.black_soldier   = Fig("0x5352")
theme.red_soldier     = Fig("0x5175")

theme.advances  = Fig("0x9032")
theme.retreats  = Fig("0x9000")
theme.traverses = Fig("0x5E73")

theme.front     = Fig("0x524d")
theme.rear      = Fig("0x540e")

theme.name = "XiangQi"
theme.description = "XiangQi figurines."
theme.variants = { "XiangQi[default]" }
