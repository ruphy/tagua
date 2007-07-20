import("piece_theme.lua")
shadow           = 7
shadow_color     = "#404040"
shadow_offset_x  = 3
shadow_offset_y  = 3
shadow_grow      = 6

function fromGlyphW(glyph)
  return function(size)
    local g = LinearGradient(Point(0.2*size,0), Point(0.8*size,0))
    g[0] = "#bcbabc"
    g[1] = "#939593"
    return fromFontGlyph("Yahoo.ttf", glyph, g, "#e0e0e0", 2, false)(size)
  end
end

function fromGlyphB(glyph)
  return function(size)
    local g = LinearGradient(Point(0.2*size,0), Point(0.8*size,0))
    g[0] = "#cecfce"
    g[1] = "#636563"
    return fromFontGlyph("Yahoo.ttf", glyph, "#000000", g, 2, false)(size)
  end
end


theme.white_bishop = fromGlyphW("0x0062")
theme.white_king   = fromGlyphW("0x006B")
theme.white_knight = fromGlyphW("0x006E")
theme.white_pawn   = fromGlyphW("0x0070")
theme.white_queen  = fromGlyphW("0x0071")
theme.white_rook   = fromGlyphW("0x0072")

theme.black_bishop = fromGlyphB("0x0062")
theme.black_king   = fromGlyphB("0x006B")
theme.black_knight = fromGlyphB("0x006E")
theme.black_pawn   = fromGlyphB("0x0070")
theme.black_queen  = fromGlyphB("0x0071")
theme.black_rook   = fromGlyphB("0x0072")

theme.name = "Yahoo"
theme.description = "Yahoo pieces, converted\nto TTF with potrace.\n  (unknown author)"
theme.variants = { "Chess" }
