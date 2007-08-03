#!/usr/bin/ruby
require 'erb'

def usage
  warn "Usage: #$0 [(-f | --filename) FILENAME] TEST_NAME [TEST] [TEST] ..."
  exit 1
end

arg = ARGV.shift or usage
if arg =~ /^-/
  case arg
  when '-f', '--filename'
    @filename = ARGV.shift or usage
  else
    usage
  end
else
  @name = arg
  @filename = @name.downcase
end
@tests = ARGV

%w(cpp h).each do |ext|
  File.open("#{@name.downcase}.#{ext}",'w') do |file|
    file.write(ERB.new(File.open("template.#{ext}.erb").read).result(binding))
  end
end
