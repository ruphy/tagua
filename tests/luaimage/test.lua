

print("Hello!!")

a = Rect("1",3,4,5)
b = Point(4,5)

print(a)
a.x = -3
a:translate(b)

print(a.x)
print("right = "..a.right)

print(a)

puppamelo = Image(200,800)
puppamelo:clear()
if(true) then
  puppamelo:fill_rect(Rect(0,0,200,800), Color(255,0,0,128))
  puppamelo:fill_rect(Rect(50,50,100,700), "#ffff00")
  puppamelo:fill_rect(Rect(75,100,50,600), "#0000ff")

  puppamelo:draw_image(Rect(0,0,200,200), "data/white_rook.png")
  puppamelo:draw_svg(Rect(0,200,200,200), "data/wk.svg")

  if(true) then
    puppamelo:fill_rect(Rect(0,400,200,200), "yellow")
    ziocane = Brush(Image("data/white_rook.png"))
    ziocane:scale(0.2,0.2)
    ziocane:rotate(45)
    ziocane = Color(255,0,0,128)
    puppamelo:draw_glyph(Rect(0,200,128,4*128), "data/Enquil.ttf", "0x006B",
                    Color(0,128,0,128), ziocane, 0.0, true);
  end
else
  puppamelo:fill_rect(Rect(50,50,200,800), Image("data/white_rook.png"))
end
teto = Image(200,200)
teto:clear()
teto:draw_glyph(Rect(0,0,200,200), "data/Enquil.ttf", "0x0071",
                                  "#ffff00", "#ffffff", 5.0, false)
shad = teto:create_shadow(25, "#000000")

puppamelo:draw_image(Rect(0,600,200,200), shad)
puppamelo:draw_image(Rect(0,600,200,200), teto)

theme.puppamelo = function(size)
  print("porca zoccola! "..size)
  return puppamelo;
end

--
--puppamelo = shad
-- a:translate(a)
