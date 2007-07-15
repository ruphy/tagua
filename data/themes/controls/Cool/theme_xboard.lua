import("../../pieces/common.lua")



local border_ratio = 0
local clock_height_ratio = 0.23
local clock_border_ratio = 0
local clock_ratio = 4
local pool_width = 3
local pool_piece_ratio = clock_ratio/pool_width

theme.layout = function(args)
  for key,value in pairs(args) do
    print("args[",key,"] = ",value)
  end

  retv = {}
  retv.square_size = math.floor(math.min(
                  args.width/(args.grid_size.x+2*border_ratio),
            args.height/(args.grid_size.y+2*border_ratio+clock_ratio*clock_height_ratio+2*clock_border_ratio) ) )
  retv.border_size = math.floor(retv.square_size*border_ratio)
  retv.clock_size  = math.floor(retv.square_size*clock_ratio)
  retv.pool_width  = pool_width;
  retv.pool_piece_size = math.floor(retv.square_size*pool_piece_ratio)

  local clock_border = math.floor(retv.square_size*clock_border_ratio)
  local clock_height = math.floor(retv.clock_size*clock_height_ratio)

  retv.board_position = Point(retv.border_size, retv.border_size+clock_height+2*clock_border)
  retv.clock0_position = Point(args.grid_size.x*retv.square_size+2*retv.border_size
                                            -retv.clock_size-clock_border,   clock_border)
  retv.clock1_position = Point(clock_border, clock_border)
  retv.pool0_position = Point();
  retv.pool1_position = Point();

  return retv
end

theme.border = function(b, args)
  return {}
end

theme.wallpaper = function()
  return Image()
end

function create_clock(col)
  return function(w)
    print("clock ",w)
    local h = math.floor(w*clock_height_ratio)
    local i = Image(w,h)
    i:clear(col)
    return i
  end
end

theme.clock_active_background = create_clock("black")
theme.clock_inactive_background = create_clock("white")

theme.clock_active_text = function(size)
  return Brush("white")
end

theme.clock_inactive_text = function(size)
  return Brush("black")
end

theme.clock_background_offset = function(w)
  return Point()
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
  return Rect(w*0.4, h*0.08, w*0.58, h*0.66)
end

theme.clock_player_rect = function(w)
  local h = math.floor(w*clock_height_ratio)
  return Rect(w*0.14, h*0.68, w*0.69, h*0.28)
end

theme.name = "XBoard"
theme.description = "XBoard style"
theme.variants = { "any[default]" }
