import("../../Pieces/common.lua")

options = OptList {
    ColorOpt("square", "Square color", "#D9C48F"),
    ColorOpt("table", "Table color", "#B9A46F"),
    BoolOpt("useborder", "Use border", true, OptList {
        ColorOpt("border", "Border color", "#724D18"),
        IntOpt("bwidth", "Border width", 2, 1, 10)
    })
}

function Pt(x, y)
  return Point(math.floor(x), math.floor(y))
end

function theme.background(size)
  local square = options.square.value
  local table  = options.table.value
  local border = options.useborder.sub_options.border.value
  local bwidth = options.useborder.sub_options.bwidth.value
  local rb = Image(size,size)
  local rt = Image(size,size)
  local lb = Image(size,size)
  local lt = Image(size,size)
  local rlb = Image(size,size)
  local rlt = Image(size,size)
  local i_rlb = Image(size,size)
  local i_rlt = Image(size,size)
  local ltb = Image(size,size)
  local rtb = Image(size,size)
  local i_ltb1 = Image(size,size)
  local i_rtb1 = Image(size,size)
  local i_ltb2 = Image(size,size)
  local i_rtb2 = Image(size,size)
  local rltb = Image(size,size)
  local castle1 = Image(3*size,3*size)
  local castle2 = Image(3*size,3*size)
  rb:clear(table)
  rb:fill_rect(Rect(size*0.5,size*0.5,size*0.5, size*0.5), square)
  rb:draw_line(Pt(size,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  rb:draw_line(Pt(size*0.5,size),Pt(size*0.5,size*0.5), border, bwidth)
  rt:clear(table)
  rt:fill_rect(Rect(size*0.5,0,size*0.5, size*0.5), square)
  rt:draw_line(Pt(size,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  rt:draw_line(Pt(size*0.5,0),Pt(size*0.5,size*0.5), border, bwidth)
  lb:clear(table)
  lb:fill_rect(Rect(0,size*0.5,size*0.5, size*0.5), square)
  lb:draw_line(Pt(0,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  lb:draw_line(Pt(size*0.5,size),Pt(size*0.5,size*0.5), border, bwidth)
  lt:clear(table)
  lt:fill_rect(Rect(0,0,size*0.5, size*0.5), square)
  lt:draw_line(Pt(0,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  lt:draw_line(Pt(size*0.5,0),Pt(size*0.5,size*0.5), border, bwidth)
  rlb:clear(table)
  rlb:fill_rect(Rect(0,size*0.5,size, size*0.5), square)
  rlb:draw_line(Pt(size,size*0.5),Pt(0,size*0.5), border, bwidth)
  rlb:draw_line(Pt(size*0.5,size),Pt(size*0.5,size*0.5), border, bwidth)
  rlt:clear(table)
  rlt:fill_rect(Rect(0,0,size, size*0.5), square)
  rlt:draw_line(Pt(size,size*0.5),Pt(0,size*0.5), border, bwidth)
  rlt:draw_line(Pt(size*0.5,0),Pt(size*0.5,size*0.5), border, bwidth)
  i_rlb:clear(square)
  i_rlb:fill_rect(Rect(0,0,size, size*0.5), Color(0,255,255,32))
  i_rlb:draw_line(Pt(size,size*0.5),Pt(0,size*0.5), border, bwidth)
  i_rlb:draw_line(Pt(size*0.5,size),Pt(size*0.5,size*0.5), border, bwidth)
  i_rlt:clear(square)
  i_rlt:fill_rect(Rect(0,size*0.5,size, size*0.5), Color(0,255,255,32))
  i_rlt:draw_line(Pt(size,size*0.5),Pt(0,size*0.5), border, bwidth)
  i_rlt:draw_line(Pt(size*0.5,0),Pt(size*0.5,size*0.5), border, bwidth)
  ltb:clear(table)
  ltb:fill_rect(Rect(0,0,size*0.5, size), square)
  ltb:draw_line(Pt(0,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  ltb:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  rtb:clear(table)
  rtb:fill_rect(Rect(size*0.5,0,size*0.5, size), square)
  rtb:draw_line(Pt(size,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  rtb:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  i_ltb1:clear(table)
  i_ltb1:fill_rect(Rect(0,0,size*0.5, size), square)
  i_ltb1:fill_rect(Rect(0,size*0.5,size*0.5, size*0.5), Color(0,255,255,32))
  i_ltb1:draw_line(Pt(0,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  i_ltb1:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  i_rtb1:clear(table)
  i_rtb1:fill_rect(Rect(size*0.5,0,size*0.5, size), square)
  i_rtb1:fill_rect(Rect(size*0.5,size*0.5,size*0.5, size*0.5), Color(0,255,255,32))
  i_rtb1:draw_line(Pt(size,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  i_rtb1:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  i_ltb2:clear(table)
  i_ltb2:fill_rect(Rect(0,0,size*0.5, size), square)
  i_ltb2:fill_rect(Rect(0,0,size*0.5, size*0.5), Color(0,255,255,32))
  i_ltb2:draw_line(Pt(0,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  i_ltb2:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  i_rtb2:clear(table)
  i_rtb2:fill_rect(Rect(size*0.5,0,size*0.5, size), square)
  i_rtb2:fill_rect(Rect(size*0.5,0,size*0.5, size*0.5), Color(0,255,255,32))
  i_rtb2:draw_line(Pt(size,size*0.5),Pt(size*0.5,size*0.5), border, bwidth)
  i_rtb2:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  rltb:clear(square)
  rltb:draw_line(Pt(size,size*0.5),Pt(0,size*0.5), border, bwidth)
  rltb:draw_line(Pt(size*0.5,size),Pt(size*0.5,0), border, bwidth)
  castle1:clear(square)
  castle1:fill_rect(Rect(0,0,size*3, size*0.5), table)
  castle1:fill_rect(Rect(size*0.5,size*0.5,size*2, size*2), Color(0,0,0,32))
  castle1:draw_line(Pt(size*0.5,size*0.5),Pt(size*2.5,size*2.5), border, bwidth)
  castle1:draw_line(Pt(size*2.5,size*0.5),Pt(size*0.5,size*2.5), border, bwidth)
  castle1:draw_line(Pt(0,size*0.5),Pt(3*size,size*0.5), border, bwidth)
  castle1:draw_line(Pt(0,size*1.5),Pt(3*size,size*1.5), border, bwidth)
  castle1:draw_line(Pt(0,size*2.5),Pt(3*size,size*2.5), border, bwidth)
  castle1:draw_line(Pt(size*0.5, 0.5*size),Pt(size*0.5, 3*size), border, bwidth)
  castle1:draw_line(Pt(size*1.5, 0.5*size),Pt(size*1.5, 3*size), border, bwidth)
  castle1:draw_line(Pt(size*2.5, 0.5*size),Pt(size*2.5, 3*size), border, bwidth)
  castle2:clear(square)
  castle2:fill_rect(Rect(0,size*2.5,size*3, size*0.5), table)
  castle2:fill_rect(Rect(size*0.5,size*0.5,size*2, size*2), Color(255,0,0,32))
  castle2:draw_line(Pt(size*0.5,size*0.5),Pt(size*2.5,size*2.5), border, bwidth)
  castle2:draw_line(Pt(size*2.5,size*0.5),Pt(size*0.5,size*2.5), border, bwidth)
  castle2:draw_line(Pt(0,size*0.5),Pt(3*size,size*0.5), border, bwidth)
  castle2:draw_line(Pt(0,size*1.5),Pt(3*size,size*1.5), border, bwidth)
  castle2:draw_line(Pt(0,size*2.5),Pt(3*size,size*2.5), border, bwidth)
  castle2:draw_line(Pt(size*0.5, 2.5*size),Pt(size*0.5, 0), border, bwidth)
  castle2:draw_line(Pt(size*1.5, 2.5*size),Pt(size*1.5, 0), border, bwidth)
  castle2:draw_line(Pt(size*2.5, 2.5*size),Pt(size*2.5, 0), border, bwidth)

  local retv = {
      [Rect(0,0,size,size)] = rb,
      [Rect(8*size,0,size,size)] = lb,
      [Rect(0,9*size,size,size)] = rt,
      [Rect(8*size,9*size,size,size)] = lt,
      [Rect(size,0,2*size,size)] = rlb,
      [Rect(6*size,0,2*size,size)] = rlb,
      [Rect(size,5*size,7*size,size)] = i_rlb,
      [Rect(size,4*size,7*size,size)] = i_rlt,
      [Rect(size,9*size,2*size,size)] = rlt,
      [Rect(6*size,9*size,2*size,size)] = rlt,
      [Rect(0,size,size,3*size)] = rtb,
      [Rect(0,4*size,size,size)] = i_rtb1,
      [Rect(0,5*size,size,size)] = i_rtb2,
      [Rect(0,6*size,size,3*size)] = rtb,
      [Rect(8*size,size,size,3*size)] = ltb,
      [Rect(8*size,4*size,size,size)] = i_ltb1,
      [Rect(8*size,5*size,size,size)] = i_ltb2,
      [Rect(8*size,6*size,size,3*size)] = ltb,
      [Rect(size,size,2*size,3*size)] = rltb,
      [Rect(6*size,size,2*size,3*size)] = rltb,
      [Rect(size,6*size,2*size,3*size)] = rltb,
      [Rect(6*size,6*size,2*size,3*size)] = rltb,
      [Rect(3*size,3*size,3*size,size)] = rltb,
      [Rect(3*size,6*size,3*size,size)] = rltb,
      [Rect(3*size,0,3*size,3*size)] = castle1,
      [Rect(3*size,7*size,3*size,3*size)] = castle2
    }
  return retv
end

function alone(color)
  return function(size)
    local i = Image(size,size)
    local g = RadialGradient(Point(size*0.485,size*0.485), size*0.58)
    g[0] = Color(0,0,0,0)
    g[0.8] = color
    g[1] = Color(0,0,0,0)
    i:clear()
    i:fill_rect(Rect(0,0,size,size), g)
    return i
  end
end

theme.validmove =    alone(Color(255,192,255,128))
theme.highlighting = alone(Color(192,255,255,128))
theme.premove =      alone(Color(0,0,255,128))
theme.selection =    alone(Color(255,0,0,128))

name = "XiangQi"
description = "XiangQi squares"
variants = { "Shogi", "XiangQi[default]", "Chess", "ChainReaction" }
