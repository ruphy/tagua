import("piece_theme.lua")
shadow=7.0
shadow_color="#404050"
shadow_offset_x=6
shadow_offset_y=4
shadow_grow=5

theme.white_stone  = fromSVG("red.svg")
theme.black_stone  = fromSVG("yellow.svg")

theme.name = "Red/Yellow"
theme.description = "Red/Yellow SVG stones\n  by Maurizio Monge\n"
theme.variants = { "Reversi", "Connect4[Default]" }
