import("piece_theme.lua")
import("shogi_themelib.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

theme.options = OptList {
  BoolOpt("moves_overlay", "Moves overlay", true),
  BoolOpt("single_kanji", "Single kanji on tiles", false),
  SelectOpt("symbols", "Symbol set", BoolOptList {
--    BoolOpt("use_predefined", "Predefined", true, OptList {
--      ComboOpt("font", "Symbol set", fontnames)
--    }),
    BoolOpt("use_custom", "Custom font", false, OptList {
      UrlOpt("file", "SVG glyph directory", "../../figurines/ShogiTTF/mikachan.ttf")
    }),
--    BoolOpt("use_system", "System font", false, OptList {
--      FontOpt("font", "System font", Font("Arial", true, true))
--    }),
  })
}

function addChar(complete, single, promoted)
  return function(i, size)
    font = theme.options.symbols.options.use_custom.sub_options.file.value
    if theme.options.single_kanji.value then
      i:draw_glyph(Rect(size*0.2,size*0.25,size*0.6,size*0.6), font, single,
		   promoted and "#d00000" or "#004000",
		   "#fff3c8", 4, false)
    else
      --local charsize, margin1, margin2 = 0.45, 0.09, -0.02 -- epgyobld
      local charsize, margin1, margin2 = 0.4, 0.12, 0
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

function shogi_piece(first, char, white, promoted, ratio, moves)
  return addShadow(overlay(tile(white, ratio),
			   shogi_moves(moves),
			   addChar(first, char, promoted)))
end

theme.black_king      = shogi_piece(shogichars["jade general"][2], shogichars["jade general"][1],
				    false, false, 1, shogimoves_king)
theme.black_rook      = shogi_piece(shogichars["flying chariot"][2], shogichars["flying chariot"][1],
				    false, false, 0.96, shogimoves_rook)
theme.black_p_rook    = shogi_piece(shogichars["dragon king"][2], shogichars["dragon king"][1],
				    false, true, 0.96, shogimoves_dragon)
theme.black_bishop    = shogi_piece(shogichars["bishop"][2], shogichars["bishop"][1],
				    false, false, 0.93, shogimoves_bishop)
theme.black_p_bishop  = shogi_piece(shogichars["dragon horse"][2], shogichars["dragon horse"][1],
				    false, true, 0.93, shogimoves_horse)
theme.black_gold      = shogi_piece(shogichars["gold general"][2], shogichars["gold general"][1],
				    false, false, 0.9, shogimoves_gold)
theme.black_silver    = shogi_piece(shogichars["silver general"][2], shogichars["silver general"][1],
				    false, false, 0.9, shogimoves_silver)
theme.black_p_silver  = shogi_piece(shogichars["narikei"][2], shogichars["narikei"][1],
				    false, true, 0.9, shogimoves_gold)
theme.black_knight    = shogi_piece(shogichars["knight"][2], shogichars["knight"][1],
				    false, false, 0.86, shogimoves_knight)
theme.black_p_knight  = shogi_piece(shogichars["narikei"][2], shogichars["narikei"][1],
				    false, true, 0.86, shogimoves_gold)
theme.black_lance     = shogi_piece(shogichars["incense chariot"][2], shogichars["incense chariot"][1],
				    false, false, 0.83, shogimoves_lance)
theme.black_p_lance   = shogi_piece(shogichars["narikyo"][2], shogichars["narikyo"][1],
				    false, true, 0.83, shogimoves_gold)
theme.black_pawn      = shogi_piece(shogichars["pawn"][2], shogichars["pawn"][1],
				    false, false, 0.8, shogimoves_pawn)
theme.black_p_pawn    = shogi_piece(shogichars["tokin"][2], shogichars["tokin"][1],
				    false, true, 0.8, shogimoves_gold)

theme.white_king      = shogi_piece(shogichars["king"][2], shogichars["king"][1],
				    true, false, 1, shogimoves_king)
theme.white_rook      = shogi_piece(shogichars["flying chariot"][2], shogichars["flying chariot"][1],
				    true, false, 0.96, shogimoves_rook)
theme.white_p_rook    = shogi_piece(shogichars["dragon king"][2], shogichars["dragon king"][1],
				    true, true, 0.96, shogimoves_dragon)
theme.white_bishop    = shogi_piece(shogichars["bishop"][2], shogichars["bishop"][1],
				    true, false, 0.93, shogimoves_bishop)
theme.white_p_bishop  = shogi_piece(shogichars["dragon horse"][2], shogichars["dragon horse"][1],
				    true, true, 0.93, shogimoves_horse)
theme.white_gold      = shogi_piece(shogichars["gold general"][2], shogichars["gold general"][1],
				    true, false, 0.9, shogimoves_gold)
theme.white_silver    = shogi_piece(shogichars["silver general"][2], shogichars["silver general"][1],
				    true, false, 0.9, shogimoves_silver)
theme.white_p_silver  = shogi_piece(shogichars["narikei"][2], shogichars["narikei"][1],
				    true, true, 0.9, shogimoves_gold)
theme.white_knight    = shogi_piece(shogichars["knight"][2], shogichars["knight"][1],
				    true, false, 0.86, shogimoves_knight)
theme.white_p_knight  = shogi_piece(shogichars["narikei"][2], shogichars["narikei"][1],
				    true, true, 0.86, shogimoves_gold)
theme.white_lance     = shogi_piece(shogichars["incense chariot"][2], shogichars["incense chariot"][1],
				    true, false, 0.83, shogimoves_lance)
theme.white_p_lance   = shogi_piece(shogichars["narikyo"][2], shogichars["narikyo"][1],
				    true, true, 0.83, shogimoves_gold)
theme.white_pawn      = shogi_piece(shogichars["pawn"][2], shogichars["pawn"][1],
				    true, false, 0.8, shogimoves_pawn)
theme.white_p_pawn    = shogi_piece(shogichars["tokin"][2],shogichars["tokin"][1],
				    true, true, 0.8, shogimoves_gold)

-- Sho Shogi

theme.black_drunken_elephant = shogi_piece(shogichars["drunken elephant"][2], shogichars["drunken elephant"][1],
					   false, false, 1, shogimoves_drunken_elephant)
theme.black_p_drunken_elephant = shogi_piece(shogichars["crown prince"][2], shogichars["crown prince"][1],
					     false, true, 1, shogimoves_king)

theme.white_drunken_elephant = shogi_piece(shogichars["drunken elephant"][2], shogichars["drunken elephant"][1],
					   true, false, 1, shogimoves_drunken_elephant)
theme.white_p_drunken_elephant = shogi_piece(shogichars["crown prince"][2], shogichars["crown prince"][1],
					     true, true, 1, shogimoves_king)

-- This is part of Dai Shohi

theme.black_freeking  = shogi_piece(shogichars["free king"][2], shogichars["free king"][1],
				    false, false, 1, shogimoves_freeking)
theme.white_freeking  = shogi_piece(shogichars["free king"][2], shogichars["free king"][1],
				    true, false, 1, shogimoves_freeking)

-- To be able to adapt this theme to chess too
theme.black_queen  = theme.black_freeking
theme.white_queen  = theme.white_freeking
