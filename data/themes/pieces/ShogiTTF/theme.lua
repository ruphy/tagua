import("piece_theme.lua")
import("shogi_themelib.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

theme.options = OptList {
  BoolOpt("moves_overlay", "Moves overlay", true),
  BoolOpt("single_kanji", "Single kanji on tiles", false)
}

font="mikachan.ttf"
--font="Sword.ttf"
--font="epgyobld.ttf"

function addChar(complete, single, promoted)
  return function(i, size)
    if theme.options.single_kanji.value then
      i:draw_glyph(Rect(size*0.2,size*0.25,size*0.6,size*0.6), font, single,
		   promoted and "#d00000" or "#004000",
		   "#fff3c8", 4, false)
    else
      --local charsize, margin1, margin2 = 0.45, 0.09, -0.02 -- epgyobld
      local charsize, margin1, margin2 = 0.4, 0.12, 0 -- mikachan
      i:draw_glyph(Rect(size*(0.5-charsize/2),size*margin1,size*charsize,size*charsize),
		   font, complete[1], promoted and "#d00000" or "#004000",
		   "#fff3c8", 4, false)
      i:draw_glyph(Rect(size*(0.5-charsize/2),size*(0.5+margin2),size*charsize,size*charsize),
		   font, complete[2], promoted and "#d00000" or "#004000",
		   "#fff3c8", 4, false)
    end
    return i
  end
end

function shogi_piece(first, char, white, promoted, ratio, ...)
  return addShadow(overlay(tile(white, ratio),
		    shogi_moves(...),
		    addChar(first, char, promoted)))
end

theme.black_king      = shogi_piece({"0x738b", "0x5c06"}, "0x738b", false, false, 1,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},
				    {-1,-1},{0,-1},{1,-1})
theme.black_rook      = shogi_piece({"0x98db", "0x8eca"}, "0x98db", false, false, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1})
theme.black_p_rook    = shogi_piece({"0x9f8d", "0x738b"}, "0x9f8d", false, true, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1},
				    {-1,-1},{1,-1},{-1,1},{1,1})
theme.black_bishop    = shogi_piece({"0x89d2", "0x884c"}, "0x89d2", false, false, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1})
theme.black_p_bishop  = shogi_piece({"0x9f8d", "0x99ac"}, "0x99ac", false, true, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1},
				    {-1,0},{1,0},{0,-1},{0,1})
theme.black_gold      = shogi_piece({"0x91d1", "0x5c06"}, "0x91d1", false, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_silver    = shogi_piece({"0x9280", "0x5c06"}, "0x9280", false, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,-1},{1,-1})
theme.black_p_silver  = shogi_piece({"0x6210", "0x9280"}, "0x5168", false, true, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_knight    = shogi_piece({"0x6842", "0x99ac"}, "0x6842", false, false, 0.86,
				    {-1,2},{1,2})
theme.black_p_knight  = shogi_piece({"0x6210", "0x6842"}, "0x572d", false, true, 0.86,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_lance     = shogi_piece({"0x9999", "0x8eca"}, "0x9999", false, false, 0.83,
				    {0,1,1})
theme.black_p_lance   = shogi_piece({"0x6210", "0x9999"}, "0x674f", false, true, 0.83,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.black_pawn      = shogi_piece({"0x6b69", "0x5175"}, "0x6b69", false, false, 0.8,
				    {0,1})
theme.black_p_pawn    = shogi_piece({"0x3068", "0x91d1"}, "0x3068", false, true, 0.8,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})

theme.white_king      = shogi_piece({"0x7389", "0x5c06"}, "0x7389", true, false, 1,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},
				    {-1,-1},{0,-1},{1,-1})
theme.white_rook      = shogi_piece({"0x98db", "0x8eca"}, "0x98db", true, false, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1})
theme.white_p_rook    = shogi_piece({"0x9f8d", "0x738b"}, "0x9f8d", true, true, 0.96,
				    {-1,0,1},{1,0,1},{0,-1,1},{0,1,1},
				    {-1,-1},{1,-1},{-1,1},{1,1})
theme.white_bishop    = shogi_piece({"0x89d2", "0x884c"}, "0x89d2", true, false, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1})
theme.white_p_bishop  = shogi_piece({"0x9f8d", "0x99ac"}, "0x99ac", true, true, 0.93,
				    {-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1},
				    {-1,0},{1,0},{0,-1},{0,1})
theme.white_gold      = shogi_piece({"0x91d1", "0x5c06"}, "0x91d1", true, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_silver    = shogi_piece({"0x9280", "0x5c06"}, "0x9280", true, false, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,-1},{1,-1})
theme.white_p_silver  = shogi_piece({"0x6210", "0x9280"}, "0x5168", true, true, 0.9,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_knight    = shogi_piece({"0x6842", "0x99ac"}, "0x6842", true, false, 0.86,
				    {-1,2},{1,2})
theme.white_p_knight  = shogi_piece({"0x6210", "0x6842"}, "0x572d", true, true, 0.86,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_lance     = shogi_piece({"0x9999", "0x8eca"}, "0x9999", true, false, 0.83,
				    {0,1,1})
theme.white_p_lance   = shogi_piece({"0x6210", "0x9999"}, "0x674f", true, true, 0.83,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})
theme.white_pawn      = shogi_piece({"0x6b69", "0x5175"}, "0x6b69", true, false, 0.8,
				    {0,1})
theme.white_p_pawn    = shogi_piece({"0x3068", "0x91d1"}, "0x3068", true, true, 0.8,
				    {-1,1},{0,1},{1,1},
				    {-1,0},{1,0},{0,-1})

-- This is part of Dai Shohi

theme.black_freeking  = shogi_piece({"0x5954", "0x738b"}, "0x5954", false, false, 1,
				    {-1,1,1},{0,1,1},{1,1,1},
				    {-1,0,1},{1,0,1},
				    {-1,-1,1},{0,-1,1},{1,-1,1})
theme.white_freeking  = shogi_piece({"0x5954", "0x738b"}, "0x5954", true, false, 1,
				    {-1,1,1},{0,1,1},{1,1,1},
				    {-1,0,1},{1,0,1},
				    {-1,-1,1},{0,-1,1},{1,-1,1})

-- To be able to adapt this theme to chess too
theme.black_queen  = theme.black_freeking
theme.white_queen  = theme.white_freeking
