import("../../pieces/common.lua")


theme.options = OptList {
  BoolOpt("wallpaper", "Draw wallpaper", true, OptList {
      BoolOpt("blur", "Enable blur", false),
      UrlOpt("file", "File:", "Wallpapers/dragonballz.jpg")
  }),
  FontOpt("clock_font1", "Clock text font", Font("Sans", true, true)),
  FontOpt("clock_font2", "Clock time font", Font("Sans", true, false)),
  FontOpt("border_font", "Border font", Font("Sans", false, false))
}

local border_ratio = 0.67
local clock_ratio = 2.6
local clock_height_ratio = 0.4
local clock_border_ratio = 0.33
local pool_width = 3
local pool_piece_ratio = clock_ratio/pool_width
local border_text_near = 0.0
local border_text_far  = 0.5

theme.layout = function(args)
  for key,value in pairs(args) do
    print("args[",key,"] = ",value)
  end

  retv = {}
  retv.square_size = math.floor(math.min(
                  args.width/(args.grid_size.x+2*border_ratio+clock_ratio+2*clock_border_ratio),
                                           args.height/(args.grid_size.y+2*border_ratio) ) )
  retv.border_size = math.floor(retv.square_size*border_ratio)
  retv.border_text_near = math.floor(retv.border_size*border_text_near)
  retv.border_text_far = math.floor(retv.border_size*border_text_far)
  retv.clock_size  = math.floor(retv.square_size*clock_ratio)
  retv.pool_width  = pool_width;
  retv.pool_piece_size = math.floor(retv.square_size*pool_piece_ratio)

  local d = Point(math.floor((args.width-retv.square_size*(args.grid_size.x+
                                2*border_ratio+clock_ratio+2*clock_border_ratio))/2),
                 math.floor((args.height-retv.square_size*(args.grid_size.y+2*border_ratio))/2));
  local clock_border = math.floor(retv.square_size*clock_border_ratio)
  local clock_height = math.floor(retv.clock_size*clock_height_ratio)
  local panel_x = retv.square_size*args.grid_size.x + 2*retv.border_size + clock_border;
  retv.board_position = d + Point(retv.border_size, retv.border_size)
  retv.clock0_position = d + Point(panel_x, clock_border_ratio*retv.square_size)
  retv.clock1_position = d + Point(panel_x, retv.square_size*args.grid_size.y + 2*retv.border_size
                                     - clock_height - clock_border)
  retv.pool0_position = Point(d.x + panel_x, retv.clock0_position.y+clock_height+clock_border);
  retv.pool1_position = Point(d.x + panel_x, retv.clock1_position.y-clock_border);

  return retv
end


function addShadow(i, size)
  local s = i:create_shadow( size, "#ffffff", Point(0, 0), Point(0, 0) )
  s:draw_image(Rect(0, 0, i.width, i.height), i)
  return s
end

theme.border = function(b, args)
  local rb = math.floor(b*2/3);
  local w = args.width;
  local h = args.height;
  local tr = Image(b, b);  tr:clear(); tr:draw_svg(Rect(0,b-rb,rb,rb), "border_corner.svg"); tr = addShadow(tr, b/5)
  local tl = Image(b, b);  tl:set_paint_over(false); tl:rotate(270); tl:translate(0,b); tl:draw_image(Rect(0,0,b,b), tr)
  local bl = Image(b, b);  bl:set_paint_over(false); bl:rotate(180); bl:translate(b,b); bl:draw_image(Rect(0,0,b,b), tr)
  local br = Image(b, b);  br:set_paint_over(false); br:rotate(90); br:translate(b,0); br:draw_image(Rect(0,0,b,b), tr)
  local t = Image(w, b);  t:clear(); t:draw_svg(Rect(0,b-rb,w,rb), "border_top.svg"); t = addShadow(t, b/5)
  local bt = Image(w, b); bt:set_paint_over(false); bt:rotate(180); bt:translate(w,b); bt:draw_image(Rect(0,0,w,b), t)
  local l = Image(b, h);  l:set_paint_over(false); l:rotate(270); l:translate(0,h); l:draw_image(Rect(0,0,h,b), t)
  local r = Image(b, h);  r:set_paint_over(false); r:rotate(90); r:translate(b,0); r:draw_image(Rect(0,0,h,b), t)
  return {
    [Rect(-b,-b,b,b)] = tl,
    [Rect(w,-b,b,b)] = tr,
    [Rect(-b,h,b,b)] = bl,
    [Rect(w,h,b,b)] = br,
    [Rect(0,-b,w,b)] = t,
    [Rect(0,h,w,b)] = bt,
    [Rect(-b,0,b,h)] = l,
    [Rect(w,0,b,h)] = r
  }
end

theme.border_color = function()
  return "black";
end

theme.border_font = function()
  return theme.options.border_font.value;
end

theme.wallpaper = function()
  if(theme.options.wallpaper.value) then
    local i = Image(theme.options.wallpaper.sub_options.file.value, false)
    if(theme.options.wallpaper.sub_options.blur.value) then
      i:exp_blur(5);
    end
    return i
  else
    return Image()
  end
end

function create_clock(file, col)
  return function(w)
    local h = math.floor(w*clock_height_ratio)
    local i = Image(w,h)
    i:clear()
    i:draw_svg(Rect(0,0,w,h), file)
    local s = i:create_shadow( w/16, col, Point(math.floor(w/7), math.floor(w/7)), Point(0, 0) )
    s:draw_image(Rect(math.floor(w/14), math.floor(w/14), i.width, i.height), i)
    return s
  end
end

theme.clock_active_background = create_clock("active_clock.svg", "#ff8080")
theme.clock_inactive_background = create_clock("inactive_clock.svg", "#e0e0e0")

theme.clock_active_text = function(size)
  return Brush("#000000")
end

theme.clock_inactive_text = function(size)
  return Brush("#808080")
end

theme.clock_background_offset = function(w)
  local d = math.floor(w/14)
  return Point(-d, -d)
end

theme.clock_height = function(w)
  return math.floor(w*clock_height_ratio)
end

theme.clock_caption_rect = function(w)
  local h = math.floor(w*clock_height_ratio)
  return Rect(w*0.02, h*0.22, w*0.4, h*0.38)
end

theme.clock_time_rect = function(w)
  local h = math.floor(w*clock_height_ratio)
  return Rect(w*0.4, h*0.1, w*0.6, h*0.62)
end

theme.clock_player_rect = function(w)
  local h = math.floor(w*clock_height_ratio)
  return Rect(w*0.14, h*0.68, w*0.69, h*0.28)
end

theme.clock_caption_font = function()
  return theme.options.clock_font1.value;
end

theme.clock_time_font = function()
  return theme.options.clock_font2.value;
end

theme.clock_player_font = function()
  return theme.options.clock_font1.value;
end

theme.name = "Cool"
theme.description = "Cool controls"
theme.variants = { "any[default]" }
