module KBoard

class Point
  attr_reader :x, :y
  
  def initialize(x, y)
    @x = x
    @y = y
  end
  
  def +(other)
    Point.new(@x + other.x, @y + other.y)
  end
  
  def -(other)
    Point.new(@x - other.x, @y - other.y)
  end
  
  def *(k)
    Point.new(@x * k, @y * k)
  end
  
  def self.shift(v)
    x = v.shift
    if x.respond_to? :x and x.respond_to? :y
      x
    else
      new(x, v.shift)
    end
  end
end

class Grid
  attr_reader :size
  
  def initialize(size)
    @size = size
    @grid = Array.new(size.x * size.y)
  end
  
  def [](*args)
    p = Point.shift(args)
    @grid[p.x + p.y * size.x]
  end
  
  def []=(*args)
    p = Point.shift(args)
    val = args.shift
    @grid[p.x + p.y * size.x] = val
  end
  
  def clone
    res = self.class.new(@size)
    res.grid = @grid.dup
    res
  end
  
  def ==(other)
    return false unless @size == other.size
    @grid.each do |i|
      return false unless self[i] == other[i]
    end
    true
  end
  
  def each_square
    @grid.each {|i| yield Point.new(i % size.x, i / size.x) }
  end
  
private
  attr_accessor :grid
end

module GridAccess
  def [](*args)
    p = Point.shift(args)
    @board[p]
  end
  
  def []=(*args)
    p = Point.shift(args)
    val = args.shift
    @board[p] = val
  end
end

def self.register(var)

end

end