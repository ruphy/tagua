
function addShadow(func)
  return function(size)
    local isz = math.floor( size*100/(100+shadow_grow) + 0.5)
    local offx = shadow_offset_x*isz/200
    local offy = shadow_offset_y*isz/200
    local i = func(isz)
    local s = i:create_shadow( shadow*isz/100, shadow_color,
                                  Point(size-isz, size-isz),
                                  Point(offx, offy) )
    s:draw_image(Rect((size-isz)/2-offx, (size-isz)/2-offy,isz,isz), i, Rect(0,0,isz,isz))
    return s
  end
end

function fromSVG_Direct(file)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_svg(Rect(0,0,size,size), file)
    return i
  end
end

function fromSVG(file)
  if shadow then
    return addShadow(fromSVG_Direct(file))
  else
    return fromSVG_Direct(file)
  end
end

function fromFontGlyph_Direct(...)
  local t = (function(...) return ... end):partial(...)
  return function(size)
    local i = Image(size,size)
    i:clear()
    i:draw_glyph(Rect(0,0,size,size), t())
    return i
  end
end

-- function fromFontGlyph_Direct(file, glyph, fg, bg, border, draw_inner_bg)
--   return function(size)
--     local i = Image(size,size)
--     i:clear()
--     i:draw_glyph(Rect(0,0,size,size), file, glyph, fg, bg, border, draw_inner_bg)
--     return i
--   end
-- end

function fromFontGlyph(...)
  if shadow then
    return addShadow(fromFontGlyph_Direct(...))
  else
    return fromFontGlyph_Direct(...)
  end
end

function fromPixmap(file)
  return function(size)
    local i = Image(size,size)
    i:set_paint_over(false)
    i:draw_image(Rect(0,0,size,size), file)
    return i
  end
end

function fromColor(color)
  return function(size)
    local i = Image(size,size)
    i:clear(color)
    return i
  end
end
