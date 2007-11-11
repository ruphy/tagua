#!/usr/bin/ruby

require 'find'

stmt = %{/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

}


Find.find('.') do |filename|
    if filename =~ /\.(cpp|h)$/
        code = File.open(filename) {|f| f.read}
        unless code =~ /copyright/i
            File.open(filename, 'w') {|f| f.write(stmt + code)}
        end
    end
end

