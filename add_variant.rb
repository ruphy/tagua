#!/usr/bin/ruby

require 'erb'

TEMPLATE = 'templates/variant_template'
VARIANT_DIR = 'src/variants'

variant = ARGV.shift or exit
%w(h cpp).each do |ext|
	File.open(TEMPLATE + ".#{ext}.erb") do |f|
		template = ERB.new(f.read)
		out = File.join(VARIANT_DIR, variant + ".#{ext}")
		File.open(out, 'w') do |outfile| 
			outfile.write(template.result(binding))
		end
		puts "generated #{out}"
	end
end

