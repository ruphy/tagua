import("../../Pieces/common.lua")

options = OptList {
    ColorOpt("square", "Square color", "#EBD6A0"),
    BoolOpt("useborder", "Use border", true, OptList {
        ColorOpt("border", "Border color", "#9C8755"),
        IntOpt("bwidth", "Border width", 3, 1, 10)
    })
}

-- function theme.background(size)
--   local i = Image(size,size)
--   local square = options.square.value
--   local useborder = options.useborder.value
--   local border = options.useborder.sub_options.border.value
--   local bwidth = options.useborder.sub_options.bwidth.value
--
--   i:clear(square);
--   if(useborder) then
--     i:draw_line(Point(0,0),Point(size,0), border, bwidth);
--     i:draw_line(Point(0,size),Point(size,size), border, bwidth);
--     i:draw_line(Point(0,0),Point(0,size), border, bwidth);
--     i:draw_line(Point(size,0),Point(size,size), border, bwidth);
--   end
--   return i
-- end

function theme.background(size)
  local square = options.square.value
  local border = options.useborder.sub_options.border.value
  local bwidth = options.useborder.sub_options.bwidth.value
  local rb = Image(size,size)
  local rt = Image(size,size)
  local lb = Image(size,size)
  local lt = Image(size,size)
  local rlb = Image(size,size)
  local rlt = Image(size,size)
  local ltb = Image(size,size)
  local rtb = Image(size,size)
  local rtb = Image(size,size)
  local rltb = Image(size,size)
  local castle1 = Image(3*size,3*size)
  local castle2 = Image(3*size,3*size)
  rb:clear(square)
  rb:draw_line(Point(size,size*0.5),Point(size*0.5,size*0.5), border, bwidth);
  rb:draw_line(Point(size*0.5,size),Point(size*0.5,size*0.5), border, bwidth);
  rt:clear(square)
  rt:draw_line(Point(size,size*0.5),Point(size*0.5,size*0.5), border, bwidth);
  rt:draw_line(Point(size*0.5,0),Point(size*0.5,size*0.5), border, bwidth);
  lb:clear(square)
  lb:draw_line(Point(0,size*0.5),Point(size*0.5,size*0.5), border, bwidth);
  lb:draw_line(Point(size*0.5,size),Point(size*0.5,size*0.5), border, bwidth);
  lt:clear(square)
  lt:draw_line(Point(0,size*0.5),Point(size*0.5,size*0.5), border, bwidth);
  lt:draw_line(Point(size*0.5,0),Point(size*0.5,size*0.5), border, bwidth);
  rlb:clear(square)
  rlb:draw_line(Point(size,size*0.5),Point(0,size*0.5), border, bwidth);
  rlb:draw_line(Point(size*0.5,size),Point(size*0.5,size*0.5), border, bwidth);
  rlt:clear(square)
  rlt:draw_line(Point(size,size*0.5),Point(0,size*0.5), border, bwidth);
  rlt:draw_line(Point(size*0.5,0),Point(size*0.5,size*0.5), border, bwidth);
  ltb:clear(square)
  ltb:draw_line(Point(0,size*0.5),Point(size*0.5,size*0.5), border, bwidth);
  ltb:draw_line(Point(size*0.5,size),Point(size*0.5,0), border, bwidth);
  rtb:clear(square)
  rtb:draw_line(Point(size,size*0.5),Point(size*0.5,size*0.5), border, bwidth);
  rtb:draw_line(Point(size*0.5,size),Point(size*0.5,0), border, bwidth);
  rltb:clear(square)
  rltb:draw_line(Point(size,size*0.5),Point(0,size*0.5), border, bwidth);
  rltb:draw_line(Point(size*0.5,size),Point(size*0.5,0), border, bwidth);
  castle1:clear(square)
  castle1:draw_line(Point(size*0.5,size*0.5),Point(size*2.5,size*2.5), border, bwidth)
  castle1:draw_line(Point(size*2.5,size*0.5),Point(size*0.5,size*2.5), border, bwidth)
  castle1:draw_line(Point(0,size*0.5),Point(3*size,size*0.5), border, bwidth)
  castle1:draw_line(Point(0,size*1.5),Point(3*size,size*1.5), border, bwidth)
  castle1:draw_line(Point(0,size*2.5),Point(3*size,size*2.5), border, bwidth)
  castle1:draw_line(Point(size*0.5, 0.5*size),Point(size*0.5, 3*size), border, bwidth)
  castle1:draw_line(Point(size*1.5, 0.5*size),Point(size*1.5, 3*size), border, bwidth)
  castle1:draw_line(Point(size*2.5, 0.5*size),Point(size*2.5, 3*size), border, bwidth)
  castle2:clear(square)
  castle2:draw_line(Point(size*0.5,size*0.5),Point(size*2.5,size*2.5), border, bwidth)
  castle2:draw_line(Point(size*2.5,size*0.5),Point(size*0.5,size*2.5), border, bwidth)
  castle2:draw_line(Point(0,size*0.5),Point(3*size,size*0.5), border, bwidth)
  castle2:draw_line(Point(0,size*1.5),Point(3*size,size*1.5), border, bwidth)
  castle2:draw_line(Point(0,size*2.5),Point(3*size,size*2.5), border, bwidth)
  castle2:draw_line(Point(size*0.5, 2.5*size),Point(size*0.5, 0), border, bwidth)
  castle2:draw_line(Point(size*1.5, 2.5*size),Point(size*1.5, 0), border, bwidth)
  castle2:draw_line(Point(size*2.5, 2.5*size),Point(size*2.5, 0), border, bwidth)

  local retv = {
      [Rect(0,0,size,size)] = rb,
      [Rect(8*size,0,size,size)] = lb,
      [Rect(0,9*size,size,size)] = rt,
      [Rect(8*size,9*size,size,size)] = lt,
      [Rect(size,0,2*size,size)] = rlb,
      [Rect(6*size,0,2*size,size)] = rlb,
      [Rect(size,5*size,7*size,size)] = rlb,
      [Rect(size,4*size,7*size,size)] = rlt,
      [Rect(size,9*size,2*size,size)] = rlt,
      [Rect(6*size,9*size,2*size,size)] = rlt,
      [Rect(0,size,size,8*size)] = rtb,
      [Rect(8*size,size,size,8*size)] = ltb,
      [Rect(size,size,2*size,3*size)] = rltb,
      [Rect(6*size,size,2*size,3*size)] = rltb,
      [Rect(size,6*size,2*size,3*size)] = rltb,
      [Rect(6*size,6*size,2*size,3*size)] = rltb,
      [Rect(3*size,3*size,3*size,size)] = rltb,
      [Rect(3*size,6*size,3*size,size)] = rltb,
      [Rect(3*size,0,3*size,3*size)] = castle1,
      [Rect(3*size,7*size,3*size,3*size)] = castle2
    }
  for k,v in pairs(retv) do
    print("ecco ".. tostring(k))
  end
  return retv
end

theme.validmove = fromColor("#bdaede")
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

name = "XiangQi"
description = "XiangQi squares"
variants = { "Shogi", "XiangQi[default]", "Chess", "ChainReaction" }
