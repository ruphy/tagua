import("../../Pieces/common.lua")

function Fig(char)
  return Glyph("ukai.ttf", char, 4)
end

theme.rn_1 = Fig("0x4e00")
theme.rn_2 = Fig("0x4e8c")
theme.rn_3 = Fig("0x4e09")
theme.rn_4 = Fig("0x56db")
theme.rn_5 = Fig("0x4e94")
theme.rn_6 = Fig("0x516d")
theme.rn_7 = Fig("0x4e03")
theme.rn_8 = Fig("0x516b")
theme.rn_9 = Fig("0x4e5d")

theme.bn_1 = Fig("0x0031")
theme.bn_2 = Fig("0x0032")
theme.bn_3 = Fig("0x0033")
theme.bn_4 = Fig("0x0034")
theme.bn_5 = Fig("0x0035")
theme.bn_6 = Fig("0x0036")
theme.bn_7 = Fig("0x0037")
theme.bn_8 = Fig("0x0038")
theme.bn_9 = Fig("0x0039")

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
