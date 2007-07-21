import("piece_theme.lua")
shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

function fromSVG1(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(size*0.25,size*0.25,size*0.5,size*0.5), file)
    print("create from "..file)
    return i
  end
end

function fromSVG2(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(size*0.45,size*0.05,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.05,size*0.45,size*0.5,size*0.5), file)
    return i
  end
end

function fromSVG3(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(size*0.25,size*0.0334936490538904,size*0.5,size*0.5), file)
    i:draw_svg(Rect(0,size*0.46650635094611,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,size*0.46650635094611,size*0.5,size*0.5), file)
    return i
  end
end

function fromSVG4(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(0,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(0,size*0.5,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,size*0.5,size*0.5,size*0.5), file)
    return i
  end
end

function fromSVG5(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(0,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(0,size*0.5,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,size*0.5,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.25,size*0.25,size*0.5,size*0.5), file)
    return i
  end
end

function fromSVG6(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(0,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(0,size*0.25,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,size*0.25,size*0.5,size*0.5), file)
    i:draw_svg(Rect(0,size*0.5,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,size*0.5,size*0.5,size*0.5), file)
    return i
  end
end

function fromSVG7(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(size*0.08,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.42,0,size*0.5,size*0.5), file)
    i:draw_svg(Rect(0,size*0.25,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.5,size*0.25,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.08,size*0.5,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.42,size*0.5,size*0.5,size*0.5), file)
    i:draw_svg(Rect(size*0.25,size*0.25,size*0.5,size*0.5), file)
    return i
  end
end

theme.white_add  = addShadow(fromSVG_Direct("red_add.svg"))
theme.black_add  = addShadow(fromSVG_Direct("blue_add.svg"))
theme.white_1  = addShadow(fromSVG1("red.svg"))
theme.black_1  = addShadow(fromSVG1("blue.svg"))
theme.neutral_1  = addShadow(fromSVG1("grey.svg"))
theme.white_2  = addShadow(fromSVG2("red.svg"))
theme.black_2  = addShadow(fromSVG2("blue.svg"))
theme.white_3  = addShadow(fromSVG3("red.svg"))
theme.black_3  = addShadow(fromSVG3("blue.svg"))
theme.white_4  = addShadow(fromSVG4("red.svg"))
theme.black_4  = addShadow(fromSVG4("blue.svg"))
theme.white_5  = addShadow(fromSVG5("red.svg"))
theme.black_5  = addShadow(fromSVG5("blue.svg"))
theme.white_6  = addShadow(fromSVG6("red.svg"))
theme.black_6  = addShadow(fromSVG6("blue.svg"))
theme.white_7  = addShadow(fromSVG7("red.svg"))
theme.black_7  = addShadow(fromSVG7("blue.svg"))

