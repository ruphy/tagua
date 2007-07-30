import("piece_theme.lua")
import("shogi_themelib.lua")

shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

function addGlyph(file)
  return function(i, size)
    i:draw_svg(Rect(0,0,size,size), file)
    return i
  end
end

function shogi_piece(file, white, ratio)
  return addShadow(overlay(tile(white, ratio),
			   addGlyph(file)))
end

theme.black_king     = shogi_piece("king.svg", false, 1)
theme.black_rook     = shogi_piece("rook.svg", false, 0.96)
theme.black_p_rook   = shogi_piece("prook.svg", false, 0.96)
theme.black_bishop   = shogi_piece("bishop.svg", false, 0.93)
theme.black_p_bishop = shogi_piece("pbishop.svg", false, 0.93)
theme.black_gold     = shogi_piece("gold.svg", false, 0.9)
theme.black_silver   = shogi_piece("silver.svg", false, 0.9)
theme.black_p_silver = shogi_piece("psilver.svg", false, 0.9)
theme.black_knight   = shogi_piece("horse.svg", false, 0.86)
theme.black_p_knight = shogi_piece("phorse.svg", false, 0.86)
theme.black_lance    = shogi_piece("lance.svg", false, 0.83)
theme.black_p_lance  = shogi_piece("plance.svg", false, 0.83)
theme.black_pawn     = shogi_piece("pawn.svg", false, 0.8)
theme.black_p_pawn   = shogi_piece("ppawn.svg", false, 0.8)

theme.white_king     = shogi_piece("king.svg", true, 1)
theme.white_rook     = shogi_piece("rook.svg", true, 0.96)
theme.white_p_rook   = shogi_piece("prook.svg", true, 0.96)
theme.white_bishop   = shogi_piece("bishop.svg", true, 0.93)
theme.white_p_bishop = shogi_piece("pbishop.svg", true, 0.93)
theme.white_gold     = shogi_piece("gold.svg", true, 0.9)
theme.white_silver   = shogi_piece("silver.svg", true, 0.9)
theme.white_p_silver = shogi_piece("psilver.svg", true, 0.9)
theme.white_knight   = shogi_piece("horse.svg", true, 0.86)
theme.white_p_knight = shogi_piece("phorse.svg", true, 0.86)
theme.white_lance    = shogi_piece("lance.svg", true, 0.83)
theme.white_p_lance  = shogi_piece("plance.svg", true, 0.83)
theme.white_pawn     = shogi_piece("pawn.svg", true, 0.8)
theme.white_p_pawn   = shogi_piece("ppawn.svg", true, 0.8)

-- To be able to adapt this theme to chess too
theme.black_queen    = shogi_piece("gold.svg", false, 0.9)
theme.white_queen    = shogi_piece("gold.svg", true, 0.9)
