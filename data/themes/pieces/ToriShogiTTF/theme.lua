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

function addChar(single, promoted)
  return function(i, size)
    font = theme.options.symbols.options.use_custom.sub_options.file.value
    i:draw_glyph(Rect(size*0.2,size*0.25,size*0.6,size*0.6), font, single,
		 promoted and "#d00000" or "#004000",
		 "#fff3c8", 4, false)
    return i
  end
end

function shogi_piece(char, white, promoted, ratio, moves)
  return addShadow(overlay(tile(white, ratio),
			   shogi_moves(moves),
			   addChar(char, promoted)))
end

-- Tori Shogi
theme.black_phoenix 	= shogi_piece(shogichars["tori phoenix"][1],
				      false, false, 1, shogimoves_king)
theme.black_falcon 	= shogi_piece(shogichars["tori falcon"][1],
				      false, false, 1, shogimoves_drunken_elephant)
theme.black_p_falcon 	= shogi_piece(shogichars["tori eagle"][1],
				      false, true, 1, shogimoves_eagle)
theme.black_crane 	= shogi_piece(shogichars["tori crane"][1],
				      false, false, 1, shogimoves_crane)
theme.black_right_quail = shogi_piece(shogichars["tori quail"][1],
				      false, false, 1, shogimoves_right_quail)
theme.black_left_quail 	= shogi_piece(shogichars["tori quail"][1],
				      false, false, 1, shogimoves_left_quail)
theme.black_pheasant 	= shogi_piece(shogichars["tori pheasant"][1],
				      false, false, 1, shogimoves_pheasant)
theme.black_swallow 	= shogi_piece(shogichars["tori swallow"][1],
				      false, false, 1, shogimoves_pawn)
theme.black_p_swallow 	= shogi_piece(shogichars["tori goose"][1],
				      false, true, 1, shogimoves_goose)

theme.white_phoenix 	= shogi_piece(shogichars["tori phoenix"][1],
				      true, false, 1, shogimoves_king)
theme.white_falcon 	= shogi_piece(shogichars["tori falcon"][1],
				      true, false, 1, shogimoves_drunken_elephant)
theme.white_p_falcon 	= shogi_piece(shogichars["tori eagle"][1],
				      true, true, 1, shogimoves_eagle)
theme.white_crane 	= shogi_piece(shogichars["tori crane"][1],
				      true, false, 1, shogimoves_crane)
theme.white_right_quail = shogi_piece(shogichars["tori quail"][1],
				      true, false, 1, shogimoves_right_quail)
theme.white_left_quail 	= shogi_piece(shogichars["tori quail"][1],
				      true, false, 1, shogimoves_left_quail)
theme.white_pheasant 	= shogi_piece(shogichars["tori pheasant"][1],
				      true, false, 1, shogimoves_pheasant)
theme.white_swallow 	= shogi_piece(shogichars["tori swallow"][1],
				      true, false, 1, shogimoves_pawn)
theme.white_p_swallow 	= shogi_piece(shogichars["tori goose"][1],
				      true, true, 1, shogimoves_goose)
