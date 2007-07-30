-- generic functions for shogi themes
-- FIXME: still depends on Shogi.ttf for the tile

function tile(white, ratio)
  return function(size)
    local i = Image(size,size)
    i:clear()

    local g = RadialGradient(Point(size*0.5,size*0.5), size*0.5, Point(size*0.3,size*0.3))
    g[0] = "#ffeda2"
    g[1] = "#c0a870"
    local b = Brush(g)

    if white then
      i:translate(-size, -size)
      i:rotate(180)
      b:translate(-size, -size)
      b:rotate(180)
    end

    if ratio then
      i:scale(ratio, ratio)
      i:translate(size*(1-ratio)*0.5, size*(1-ratio)*0.5)
      b:scale(ratio, ratio)
      b:translate(size*(1-ratio)*0.5, size*(1-ratio)*0.5)
    end

    i:draw_glyph(Rect(0,0,size,size), "../ShogiTTF/Shogi.ttf", "0x2c", "black", b)
    --i:draw_svg(Rect(0,0,size,size), "../ShogiSVG/tile.svg")
    return i
  end
end

function draw_arrowhead(i, p, vx,vy, length, rwidth, linecolor,linewidth)
  -- normalize to 1 the direction vector
  local vl = (vx*vx + vy*vy)^0.5
  vx,vy = vx/vl, vy/vl
  -- the component vector along the arrow direction
  local vxdirect, vydirect = -vx*length, -vy*length
  -- an orthogonal vector of requested relative width
  local vxortho,vyortho = vydirect*rwidth, -vxdirect*rwidth

  i:draw_line(p,Point(p.x+vxdirect+vxortho,p.y-vydirect-vyortho), linecolor,linewidth)
  i:draw_line(p,Point(p.x+vxdirect-vxortho,p.y-vydirect+vyortho), linecolor,linewidth)
end

function shogi_moves(...)
  local moves = {...}
  return function(i,size)
    if not theme.options.moves_overlay.value then
       return i
    end

    local index, move
    --local xcenter, ycenter, width = size/2, size/2, size/4
    local xcenter, ycenter, width = 0.8*size, 0.82*size, 0.08*size
    local tilesize,shortsize = 0.8*width, 0.3*width
    local arrowlen,arrowrwidth = 0.2*width, 0.5
    local centerpoint = Point(xcenter,ycenter)
    local b = Brush("red")
    for index,move in ipairs(moves) do
      local x,y,long = move[1],move[2],move[3]
      if long then
	local p2 = Point(xcenter+1.5*x*width, ycenter-1.5*y*width)
	i:draw_line(centerpoint,p2, "purple",1)
	draw_arrowhead(i, p2, x,y, arrowlen,arrowrwidth, "purple",1)
      else
	i:draw_line(centerpoint, Point(xcenter+x*width,ycenter-y*width),"red",1)
	i:fill_rect(Rect(xcenter+x*width-shortsize/2, ycenter-y*width-shortsize/2,
			 shortsize, shortsize), b)
      end
    end

    -- the tile
    i:fill_rect(Rect(xcenter-tilesize/2,ycenter-tilesize/2,
		     tilesize,tilesize), b)

    return i
  end
end
