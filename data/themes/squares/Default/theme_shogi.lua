import("../../pieces/common.lua")
import("selection.lua")

theme.options = OptList {
    ColorOpt("square", "Square color", "#EBD6A0"),
    IntOpt("alpha", "Alpha", 192, 0, 255),
    BoolOpt("useborder", "Use border", true, OptList {
        ColorOpt("border", "Border color", "#9C8755"),
        IntOpt("bwidth", "Border width", 3, 1, 10)
    })
}

theme.background = function(size)
  local i = Image(size,size)
  local square = theme.options.square.value
  local useborder = theme.options.useborder.value
  local border = theme.options.useborder.sub_options.border.value
  local bwidth = theme.options.useborder.sub_options.bwidth.value
  square.a = theme.options.alpha.value

  i:clear(square);
  if(useborder) then
    i:draw_line(Point(0,0),Point(size,0), border, bwidth);
    i:draw_line(Point(0,size),Point(size,size), border, bwidth);
    i:draw_line(Point(0,0),Point(0,size), border, bwidth);
    i:draw_line(Point(size,0),Point(size,size), border, bwidth);
  end
  return i
end

theme.validmove = fromColor("#bdaede")

theme.name = "Shogi"
theme.description = "Shogi-like squares"
theme.variants = { "Shogi[default]", "Chess" }
