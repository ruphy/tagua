function out(x)
  dump(x, "", "")
end

function dump(x, indent1, indent2)
  mt = getmetatable(x)
  if mt then
    io.write("> metatable: ")
    dump(mt, "", indent2 .. "  ")
    io.write("> object: ")
  else
    io.write(indent1)
  end
  if type(x) == "string" then
    io.write(x)
  elseif type(x) == "number" then
    io.write(x)
  elseif type(x) == "table" then
    print("{")
    for k, v in pairs(x) do
      dump(k, indent2 .. "    ", indent2 .. "    ")
      io.write(" = ")
      dump(v, "", indent2 .. "    ")
      print("")
    end
    print(indent2 .. "}")
  else
    io.write("<" .. type(x) .. ">")
  end
end

function debug()
  print "Rect"
  out(Rect)
  print ""
  
  print "r"
  out(r)
  print ""
end

r = Rect(2, 3, 100, 400)
print(r:height())


