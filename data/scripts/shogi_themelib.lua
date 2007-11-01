-- generic functions for shogi themes
-- FIXME: still depends on Shogi.ttf for the tile

function tile(white, ratio)
  return function(size, args)
    local i = Image(size,size)
    i:clear()

    local g = RadialGradient(Point(size*0.5,size*0.5), size*0.5, Point(size*0.3,size*0.3))
    g[0] = "#ffeda2"
    g[1] = "#c0a870"
    local b = Brush(g)
    
    local flip = (white or args.flipped) and not (white and args.flipped)

    if flip then
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

    i:draw_glyph(Rect(0,0,size,size), "../ShogiSVG-TTF/Shogi.ttf", "0x2c", "black", b)
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

function shogi_moves(moves)
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

shogichars = {}
-- standard Shogi
shogichars["king"] 		= {"0x7389", {"0x7389", "0x5c06"}}
shogichars["jade general"] 	= {"0x738b", {"0x738b", "0x5c06"}}
shogichars["flying chariot"] 	= {"0x98db", {"0x98db", "0x8eca"}}
shogichars["bishop"] 		= {"0x89d2", {"0x89d2", "0x884c"}}
shogichars["gold general"] 	= {"0x91d1", {"0x91d1", "0x5c06"}}
shogichars["silver general"] 	= {"0x9280", {"0x9280", "0x5c06"}}
shogichars["knight"] 		= {"0x6842", {"0x6842", "0x99ac"}}
shogichars["incense chariot"] 	= {"0x9999", {"0x9999", "0x8eca"}}
shogichars["pawn"] 		= {"0x6b69", {"0x6b69", "0x5175"}}
shogichars["dragon king"] 	= {"0x9f8d", {"0x9f8d", "0x738b"}}
shogichars["dragon horse"] 	= {"0x99ac", {"0x9f8d", "0x99ac"}}
shogichars["narigin"] 		= {"0x5168", {"0x6210", "0x9280"}}
shogichars["narikei"] 		= {"0x572d", {"0x6210", "0x6842"}}
shogichars["narikyo"] 		= {"0x674f", {"0x6210", "0x9999"}}
shogichars["tokin"] 		= {"0x3068", {"0x3068", "0x91d1"}}
-- Sho Shogi
shogichars["drunken elephant"]	= {"0x9154", {"0x9154", "0x8c61"}}
shogichars["crown prince"]	= {"0x592a", {"0x592a", "0x5b50"}}
-- Tori Shogi
shogichars["tori phoenix"] 		= { "0x9d6c" }
shogichars["tori falcon"] 		= { "0x9df9" }
shogichars["tori eagle"] 		= { "0x9d70" }
shogichars["tori crane"] 		= { "0x9db4" }
shogichars["tori quail"] 		= { "0x9d89" }
shogichars["tori pheasant"] 		= { "0x96c9" }
shogichars["tori swallow"] 		= { "0x71d5" }
shogichars["tori goose"] 		= { "0x9d08" }
-- Chess compatibility (part of Dai Shogi)
shogichars["free king"]		= {"0x5954", {"0x5954", "0x738b"}}

shogimoves_king = {
   {-1,1},{0,1},{1,1},
   {-1,0},{1,0},
   {-1,-1},{0,-1},{1,-1}}
shogimoves_rook = {{-1,0,1},{1,0,1},{0,-1,1},{0,1,1}}
shogimoves_dragon = {
   {-1,0,1},{1,0,1},{0,-1,1},{0,1,1},
   {-1,-1},{1,-1},{-1,1},{1,1}}
shogimoves_bishop = {{-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1}}
shogimoves_horse = {
   {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1},
   {-1,0},{1,0},{0,-1},{0,1}}
shogimoves_gold = {
   {-1,1},{0,1},{1,1},
   {-1,0},{1,0},{0,-1}}
shogimoves_silver = {
   {-1,1},{0,1},{1,1},
   {-1,-1},{1,-1}}
shogimoves_knight = {{-1,2},{1,2}}
shogimoves_lance = {{0,1,1}}
shogimoves_pawn = {{0,1}}

shogimoves_drunken_elephant = {
   {-1,1},{0,1},{1,1},
   {-1,0},{1,0},
   {-1,-1},{1,-1}}

shogimoves_freeking = {
   {-1,1,1},{0,1,1},{1,1,1},
   {-1,0,1},{1,0,1},
   {-1,-1,1},{0,-1,1},{1,-1,1}}

shogimoves_eagle = {
   {-1,1,1},{0,1},{1,1,1},
   {-1,0},{1,0},
   {-1,-1},{-2,-2},{0,-1,1},{1,-1},{2,-2}}
shogimoves_crane = {
   {-1,1},{0,1},{1,1},
   {-1,-1},{0,-1},{1,-1}}
shogimoves_right_quail = {{0,1,1},{-1,-1,1},{1,-1}}
shogimoves_left_quail = {{0,1,1},{1,-1,1},{-1,-1}}
shogimoves_pheasant = {{0,2},{-1,-1},{1,-1}}
shogimoves_goose = {{-2,2},{2,2},{0,-2}}
