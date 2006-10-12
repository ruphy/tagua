#!/usr/bin/ruby

require 'optparse'
require 'set'

input_file = nil
output_file = nil
font_name = nil

$options = OptionParser.new do |opt|
  opt.banner = "Usage: strip_font.rb [options] luafiles"

  opt.on( '-n', '--name FONT_NAME', 'Output font name' )    do |name|         font_name = name end
  opt.on( '-i', '--input INPUT_FONT', 'Input font')         do |file|         input_file = file end
  opt.on( '-o', '--output OUTPUT_FONT', 'Font to generate') do |file|         output_file = file end
end

$options.parse!(ARGV)

lua_files = ARGV

selection = Set.new

ARGV.each do |file|
  File.open(file) do |f|
    content = f.read
    content.scan(/(["'])0x([0-9a-fA-F]{4})\1/).each do |match|
      print(match.inspect+"\n")
      selection.add "SelectMore(0u"+match[1]+");"
    end
  end
end

a = %{
  Open("#{input_file}");
#{selection.to_a.join("\n")}
  SelectInvert();
  DetachAndRemoveGlyphs();
  SetFontNames("#{font_name}");
  SetUniqueID(0);
  Generate("#{output_file}");
}

system("echo -en #{a.inspect} | fontforge -c")

