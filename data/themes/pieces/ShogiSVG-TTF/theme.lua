import("piece_theme.lua")
import("shogi_themelib.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

fontnames = {"Potraced from ???", "Potraced from shogi.net"}
fontlist = {"Shogi.ttf", "Shogi2.ttf"}

theme.options = OptList {
  BoolOpt("moves_overlay", "Moves overlay", true),
  SelectOpt("symbols", "Symbol set", BoolOptList {
    BoolOpt("use_predefined", "Predefined", true, OptList {
      ComboOpt("font", "Symbol set", fontnames)
    }),
    BoolOpt("use_custom", "Custom font", false, OptList {
      UrlOpt("file", "SVG glyph directory", "Shogi.ttf")
    }),
--    BoolOpt("use_system", "System font", false, OptList {
--      FontOpt("font", "System font", Font("Arial", true, true))
--    }),
  })
}

function addChar(char, promoted)
  return function(i, size)
    i:draw_glyph(Rect(0,0,size,size),
		 theme.options.symbols.options.use_predefined.value
		    and fontlist[theme.options.symbols.options.use_predefined.sub_options.font.selected+1]
		    or theme.options.symbols.options.use_custom.value
		    and theme.options.symbols.options.use_custom.sub_options.file.value
		    or theme.options.symbols.options.use_system.sub_options.font.selected,
		 char, promoted and "#d00000" or "#004000",
		 "#fff3c8", 4, false)
    return i
  end
end

function shogi_piece(char, white, promoted, ratio, moves)
  return addShadow(overlay(tile(white, ratio),
			   shogi_moves(moves),
			   addChar(char, promoted)))
end

theme.black_king      = shogi_piece("0x6B", false, false, 1, shogimoves_king)
theme.black_rook      = shogi_piece("0x72", false, false, 0.96, shogimoves_rook)
theme.black_p_rook    = shogi_piece("0x52", false, true, 0.96, shogimoves_dragon)
theme.black_bishop    = shogi_piece("0x62", false, false, 0.93, shogimoves_bishop)
theme.black_p_bishop  = shogi_piece("0x42", false, true, 0.93, shogimoves_horse)
theme.black_gold      = shogi_piece("0x67", false, false, 0.9, shogimoves_gold)
theme.black_silver    = shogi_piece("0x73", false, false, 0.9, shogimoves_silver)
theme.black_p_silver  = shogi_piece("0x53", false, true, 0.9, shogimoves_gold)
theme.black_knight    = shogi_piece("0x68", false, false, 0.86, shogimoves_knight)
theme.black_p_knight  = shogi_piece("0x48", false, true, 0.86, shogimoves_gold)
theme.black_lance     = shogi_piece("0x6C", false, false, 0.83, shogimoves_lance)
theme.black_p_lance   = shogi_piece("0x4C", false, true, 0.83, shogimoves_gold)
theme.black_pawn      = shogi_piece("0x70", false, false, 0.8, shogimoves_pawn)
theme.black_p_pawn    = shogi_piece("0x50", false, true, 0.8, shogimoves_gold)

theme.white_king      = shogi_piece("0x6B", true, false, 1, shogimoves_king)
theme.white_rook      = shogi_piece("0x72", true, false, 0.96, shogimoves_rook)
theme.white_p_rook    = shogi_piece("0x52", true, true, 0.96, shogimoves_dragon)
theme.white_bishop    = shogi_piece("0x62", true, false, 0.93, shogimoves_bishop)
theme.white_p_bishop  = shogi_piece("0x42", true, true, 0.93, shogimoves_horse)
theme.white_gold      = shogi_piece("0x67", true, false, 0.9, shogimoves_gold)
theme.white_silver    = shogi_piece("0x73", true, false, 0.9, shogimoves_silver)
theme.white_p_silver  = shogi_piece("0x53", true, true, 0.9, shogimoves_gold)
theme.white_knight    = shogi_piece("0x68", true, false, 0.86, shogimoves_knight)
theme.white_p_knight  = shogi_piece("0x48", true, true, 0.86, shogimoves_gold)
theme.white_lance     = shogi_piece("0x6C", true, false, 0.83, shogimoves_lance)
theme.white_p_lance   = shogi_piece("0x4C", true, true, 0.83, shogimoves_gold)
theme.white_pawn      = shogi_piece("0x70", true, false, 0.8, shogimoves_pawn)
theme.white_p_pawn    = shogi_piece("0x50", true, true, 0.8, shogimoves_gold)


-- To be able to adapt this theme to chess too
-- FIXME: should find a FreeKing kanji instead
theme.black_queen  = shogi_piece("0x67", false, false, 1, shogimoves_freeking)
theme.white_queen  = shogi_piece("0x67", true, false, 1, shogimoves_freeking)
