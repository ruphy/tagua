on["^(%S+) tells you: "] = function()
  sentence = line(match.to, line.to)
  sentence.bold = true
  nick = Text("<" .. ref[1].text .. "> ")
  nick.color = "#104050"

  return nick + sentence
end

on["^%S+ rating adjustment:|^No rating adjustment done."] = function()
--  send_text("Thank you")

  return line
end

filter("^<12>")

filter("^<b1>")

nick = ""

-- Notification
highlight["^(?:Present company includes|Notification|Challenge|You|:).*$"] = {
  color = Color(158,29,180)
}

-- Seek
on["(^%S+%s%((%d|%+)+%))( seeking .*) %(\"play (%d+)\" to respond%)"] = function()
  ref[4].bold = true

  return ref[1] + ref[3] + Text(" ==> ") + ref[4]
end

-- Channel tell
highlight["^%S+%((%d|%+)+%):.*$"] = {
  color = Color(177,130,0)
}

-- Shouts
highlight["^(?:%-%->|%S+%s+(?:(?:c%-)?shouts:)).*$"] = {
  color = Color(0,145,65)
}

-- Game
highlight["^(?:%{|Game|Creating).*$"] = {
  bold = true,
  color = Color(214,0,3)
}

-- Tells
-- highlight["^%S+%s+tells you.*$"] = {
--   color = Color(0,40,221)
-- }



