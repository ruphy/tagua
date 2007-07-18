require 'tagua'

module TicTacToe

class Piece
  attr_reader :color
  
  def initialize(color)
    @color = color
  end
 
  def to_s
    "#{@color}_man"
  end
end

Move = Tagua::Point

class Pool
  def clear
  end
end

class Position
  include GridAccess
  attr_accessor :turn
  def previous_turn
    if @turn == :white
      :black
    else
      :white
    end
  end
  
  def initialize
    @turn = :white
    @board = Grid.new(size)
  end
  
  def size
    Point.new(3,3)
  end
  
  def setup
  end
  
  def test_move(m)
    not @board[m]
  end
  
  def move(m)
    @board[m] = Piece.new(@turn)
    switch_turn
  end
  
  def switch_turn
    @turn = if @turn == :white
      :black
    else
      :white
    end
  end
  
  def status
    each_line do |line|
      res = check_line(line)
      return "#{res} wins" if res
    end
    @grid.each_square do |i|
      return "in play" unless @grid[i]
    end
    "draw"
  end
  
  def clone
    res = self.class.new
    res.turn = @turn
    res.board = board.clone
    res
  end
  
  def ==(other)
    @turn == other.turn and @board == other.board
  end
protected
  def each_line
    (0..2).each do |i|
      yield (0..2).map{|y| Point.new(i, y) }
      yield (0..2).map{|x| Point.new(x, i) }
    end
    
    yield (0..2).map{|x| Point.new(x, x) }
    yield (0..2).map{|x| Point.new(x, 2 - x) }
  end
  
  def check_line(line)
    col = false
    line.each do |val|
      return false if col and col != val.color
      col = val.color
    end
    col
  end
  
  attr_accessor :board
end

end

Tagua.register "Tic Tac Toe", TicTacToe
