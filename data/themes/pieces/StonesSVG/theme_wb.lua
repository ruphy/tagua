import("piece_theme.lua")
shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

theme.white_stone  = fromSVG("white.svg")
theme.black_stone  = fromSVG("black.svg")

theme.name = "White/Black"
theme.description = "White/Black SVG stones\n  by Maurizio Monge\n"
theme.variants = { "Reversi[default]" }
