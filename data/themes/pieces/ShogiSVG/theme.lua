import("piece_theme.lua")
import("shogi_themelib.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

fontnames = {"Potraced from ???"} -- , "Potraced from shogi.net", "Potraced from daemonshogi"}
fontlist = {"default"} -- , "shogi.net", "daemonshogi"}

theme.options = OptList {
  BoolOpt("moves_overlay", "Moves overlay", true),
  SelectOpt("symbols", "Symbol set", BoolOptList {
    BoolOpt("use_predefined", "Predefined", true, OptList {
      ComboOpt("font", "Symbol set", fontnames)
    }),
    BoolOpt("use_custom", "Custom", false, OptList {
      UrlOpt("dir", "SVG glyph directory", "default/")
    }),
  })
}

function addGlyph(file)
  return function(i, size)
    local dir
    if theme.options.symbols.options.use_predefined.value then
      dir = fontlist[theme.options.symbols.options.use_predefined.sub_options.font.selected+1]
    else
      dir = theme.options.symbols.options.use_custom.sub_options.dir.value
    end
    i:draw_svg(Rect(0,0,size,size), dir .. '/' .. file)
    return i
  end
end

function shogi_piece(file, white, ratio, moves)
  return addShadow(overlay(tile(white, ratio),
			   shogi_moves(moves),
			   addGlyph(file)))
end

theme.black_king     = shogi_piece("king.svg", false, 1, shogimoves_king)
theme.black_rook     = shogi_piece("rook.svg", false, 0.96, shogimoves_rook)
theme.black_p_rook   = shogi_piece("prook.svg", false, 0.96, shogimoves_dragon)
theme.black_bishop   = shogi_piece("bishop.svg", false, 0.93, shogimoves_bishop)
theme.black_p_bishop = shogi_piece("pbishop.svg", false, 0.93, shogimoves_horse)
theme.black_gold     = shogi_piece("gold.svg", false, 0.9, shogimoves_gold)
theme.black_silver   = shogi_piece("silver.svg", false, 0.9, shogimoves_silver)
theme.black_p_silver = shogi_piece("psilver.svg", false, 0.9, shogimoves_gold)
theme.black_knight   = shogi_piece("horse.svg", false, 0.86, shogimoves_knight)
theme.black_p_knight = shogi_piece("phorse.svg", false, 0.86, shogimoves_gold)
theme.black_lance    = shogi_piece("lance.svg", false, 0.83, shogimoves_lance)
theme.black_p_lance  = shogi_piece("plance.svg", false, 0.83, shogimoves_gold)
theme.black_pawn     = shogi_piece("pawn.svg", false, 0.8, shogimoves_pawn)
theme.black_p_pawn   = shogi_piece("ppawn.svg", false, 0.8, shogimoves_gold)

theme.white_king     = shogi_piece("king.svg", true, 1, shogimoves_king)
theme.white_rook     = shogi_piece("rook.svg", true, 0.96, shogimoves_rook)
theme.white_p_rook   = shogi_piece("prook.svg", true, 0.96, shogimoves_dragon)
theme.white_bishop   = shogi_piece("bishop.svg", true, 0.93, shogimoves_bishop)
theme.white_p_bishop = shogi_piece("pbishop.svg", true, 0.93, shogimoves_horse)
theme.white_gold     = shogi_piece("gold.svg", true, 0.9, shogimoves_gold)
theme.white_silver   = shogi_piece("silver.svg", true, 0.9, shogimoves_silver)
theme.white_p_silver = shogi_piece("psilver.svg", true, 0.9, shogimoves_gold)
theme.white_knight   = shogi_piece("horse.svg", true, 0.86, shogimoves_knight)
theme.white_p_knight = shogi_piece("phorse.svg", true, 0.86, shogimoves_gold)
theme.white_lance    = shogi_piece("lance.svg", true, 0.83, shogimoves_lance)
theme.white_p_lance  = shogi_piece("plance.svg", true, 0.83, shogimoves_gold)
theme.white_pawn     = shogi_piece("pawn.svg", true, 0.8, shogimoves_pawn)
theme.white_p_pawn   = shogi_piece("ppawn.svg", true, 0.8, shogimoves_gold)

-- To be able to adapt this theme to chess too
theme.black_queen    = shogi_piece("gold.svg", false, 0.9, shogimoves_freeking)
theme.white_queen    = shogi_piece("gold.svg", true, 0.9, shogimoves_freeking)
