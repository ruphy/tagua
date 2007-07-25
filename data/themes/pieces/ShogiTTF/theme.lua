import("piece_theme.lua")
shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

theme.options = OptList {
  BoolOpt("moves_overlay", "Moves overlay", true)
}

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

    i:draw_glyph(Rect(0,0,size,size), "Shogi.ttf", "0x2c", "black", b)
    return i
  end
end
function addChar(char, promoted)
  return function(i, size)
    i:draw_glyph(Rect(0,0,size,size), "Shogi.ttf", char,
		 promoted and "#d00000" or "#004000",
		 "#fff3c8", 4, false)
    return i
  end
end

function draw_arrowhead(i, x,y, vx,vy, length, rwidth, linecolor,linewidth)
  -- normalize to 1 the direction vector
  local vl = (vx*vx + vy*vy)^0.5
  vx,vy = vx/vl, vy/vl
  -- the component vector along the arrow direction
  local vxdirect, vydirect = -vx*length, -vy*length
  -- an orthogonal vector of requested relative width
  local vxortho,vyortho = vydirect*rwidth, -vxdirect*rwidth

  i:draw_line(Point(x,y),Point(x+vxdirect+vxortho,y-vydirect-vyortho), linecolor,linewidth)
  i:draw_line(Point(x,y),Point(x+vxdirect-vxortho,y-vydirect+vyortho), linecolor,linewidth)
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
	x2, y2 = xcenter+1.5*x*width, ycenter-1.5*y*width
	i:draw_line(centerpoint, Point(x2,y2),"purple",1)
	draw_arrowhead(i, x2,y2, x,y, arrowlen,arrowrwidth, "purple",1)
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
   
function shogi_piece(char, white, promoted, ratio, ...)
  return addShadow(overlay(tile(white, ratio),
		    shogi_moves(...),
		    addChar(char, promoted)))
end

theme.black_king      = shogi_piece("0x6B", false, false, 1,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},
				    {-1,-1},{0,-1},{1,-1})
theme.black_rook      = shogi_piece("0x72", false, false, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1})
theme.black_p_rook    = shogi_piece("0x52", false, true, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1},
				    {-1,-1},{1,-1},{-1,1},{1,1})
theme.black_bishop    = shogi_piece("0x62", false, false, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1})
theme.black_p_bishop  = shogi_piece("0x42", false, true, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1},
				    {-1,0},{1,0},{0,-1},{0,1})
theme.black_gold      = shogi_piece("0x67", false, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_silver    = shogi_piece("0x73", false, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,-1},{1,-1})
theme.black_p_silver  = shogi_piece("0x53", false, true, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_knight    = shogi_piece("0x68", false, false, 0.86,
				    {-1,2},{1,2})
theme.black_p_knight  = shogi_piece("0x48", false, true, 0.86,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_lance     = shogi_piece("0x6C", false, false, 0.83,
				    {0,1,1})
theme.black_p_lance   = shogi_piece("0x4C", false, true, 0.83,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_pawn      = shogi_piece("0x70", false, false, 0.8,
				    {0,1})
theme.black_p_pawn    = shogi_piece("0x50", false, true, 0.8,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})

theme.white_king      = shogi_piece("0x6B", true, false, 1,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},
				    {-1,-1},{0,-1},{1,-1})
theme.white_rook      = shogi_piece("0x72", true, false, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1})
theme.white_p_rook    = shogi_piece("0x52", true, true, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1},
				    {-1,-1},{1,-1},{-1,1},{1,1})
theme.white_bishop    = shogi_piece("0x62", true, false, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1})
theme.white_p_bishop  = shogi_piece("0x42", true, true, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1},
				    {-1,0},{1,0},{0,-1},{0,1})
theme.white_gold      = shogi_piece("0x67", true, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_silver    = shogi_piece("0x73", true, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,-1},{1,-1})
theme.white_p_silver  = shogi_piece("0x53", true, true, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_knight    = shogi_piece("0x68", true, false, 0.86,
				    {-1,2},{1,2})
theme.white_p_knight  = shogi_piece("0x48", true, true, 0.86,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_lance     = shogi_piece("0x6C", true, false, 0.83,
				    {0,1,1})
theme.white_p_lance   = shogi_piece("0x4C", true, true, 0.83,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_pawn      = shogi_piece("0x70", true, false, 0.8,
				    {0,1})
theme.white_p_pawn    = shogi_piece("0x50", true, true, 0.8,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})

-- To be able to adapt this theme to chess too
theme.black_queen  = theme.black_gold
theme.white_queen  = theme.white_gold

