import("piece_theme.lua")

function alone(color)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_line(Point(size*0.1,size*0.1), Point(size*0.9,size*0.1), color, size*0.1);
    i:draw_line(Point(size*0.1,size*0.9), Point(size*0.9,size*0.9), color, size*0.1);
    i:draw_line(Point(size*0.1,size*0.1), Point(size*0.1,size*0.9), color, size*0.1);
    i:draw_line(Point(size*0.9,size*0.1), Point(size*0.9,size*0.9), color, size*0.1);
    local a = Color(color)
    a.a = 64
    i:fill_rect(Rect(size*0.1,size*0.1,size*0.8,size*0.8), a);
    i:exp_blur(size*0.1);
    return i
  end
end

theme.validmove = fromColor("#cd9eee")
theme.highlighting = alone("#80ff80")
theme.premove = alone("#4040ff")
theme.selection = alone("#ff4040")
