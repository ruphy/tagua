import("../../pieces/common.lua")

theme.options = OptList {
    ColorOpt("square", "Square color", "#EBD6A0"),
    BoolOpt("useborder", "Use border", true, OptList {
        ColorOpt("border", "Border color", "#9C8755"),
        IntOpt("bwidth", "Border width", 3, 1, 10)
    })
}

theme.background = function(size)
  local i = Image(size,size)
  local square = options.square.value
  local useborder = options.useborder.value
  local border = options.useborder.sub_options.border.value
  local bwidth = options.useborder.sub_options.bwidth.value

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
theme.highlighting = fromPixmap("square_last.png")
theme.premove = fromPixmap("square_premove.png")
theme.selection = fromPixmap("square_select.png")

theme.name = "Shogi"
theme.description = "Shogi-like squares"
theme.variants = { "Shogi[default]", "Chess" }