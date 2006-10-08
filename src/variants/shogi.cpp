#include "shogi.h"
#include "xchess/animator.h"
#include "xchess/piece.h"
#include "xchess/move.h"
#include "highlevel.h"
#include "moveserializer.impl.h"
#include "crazyhouse_p.h"

class ShogiPiece {
public:
  enum Type {
    KING,
    GOLD,
    SILVER,
    KNIGHT,
    LANCE,
    ROOK,
    BISHOP,
    PAWN,
    INVALID_TYPE
  };
  enum Color {
    BLACK,
    WHITE,
    INVALID_COLOR
  };
  typedef bool PromotionType;
private:
  Color m_color;
  Type m_type;
  bool m_promoted;
public:
  ShogiPiece()
  : m_color(INVALID_COLOR)
  , m_type(INVALID_TYPE)
  , m_promoted(false) { }
  ShogiPiece(ShogiPiece::Color color, ShogiPiece::Type type, bool promoted = false);
  ShogiPiece(const ShogiPiece& other);
  
  void promote() { m_promoted = true; }
  
  bool operator<(const ShogiPiece& p) const {
    if (m_promoted == p.m_promoted)
      if (m_color == p.m_color)
        return m_type < p.m_type;
      else
        return m_color < p.m_color;
    else
      return m_promoted < p.m_promoted;
  }
  
  QString name() const;
  static QString typeName(ShogiPiece::Type t);
  bool valid() const { return m_color != INVALID_COLOR && m_type != INVALID_TYPE; }
  operator bool() const { return valid(); }
  bool operator!() const { return !valid(); }
  
  bool operator==(const ShogiPiece& p) const {
    return m_promoted == p.m_promoted &&
           m_color == p.m_color &&
           m_type == p.m_type;
  }
  
  bool operator!=(const ShogiPiece& p) const {
    return !(operator==(p));
  }
  
  static Type getType(const QString& t);
  static QString typeSymbol(ShogiPiece::Type t);
  
  bool canMove(const class ShogiPosition&, const Point&, const Point&) const { return true; }
  Color color() const { return m_color; }
  Type type() const { return m_type; }
  
  static Color oppositeColor(Color c) { return c == WHITE ? BLACK : WHITE; }
};

ShogiPiece::ShogiPiece(ShogiPiece::Color color, ShogiPiece::Type type, bool promoted)
: m_color(color)
, m_type(type)
, m_promoted(promoted) { }

ShogiPiece::ShogiPiece(const ShogiPiece& other)
: m_color(other.m_color)
, m_type(other.m_type)
, m_promoted(other.m_promoted) { }

QString ShogiPiece::name() const {
  QString res = m_color == WHITE ? "white_" : "black_";
  if (m_promoted)
    res += "p_";
  res += typeName(m_type);
  return res;
}

QString ShogiPiece::typeName(ShogiPiece::Type t) {
  switch (t) {
  case KING:
    return "king";
  case GOLD:
    return "gold";
  case SILVER:
    return "silver";
  case KNIGHT:
    return "knight";
  case LANCE:
    return "lance";
  case ROOK:
    return "rook";
  case BISHOP:
    return "bishop";
  case PAWN:
    return "pawn";
  default:
    return "unknown";
  }
}

ShogiPiece::Type ShogiPiece::getType(const QString&) {
  return KING; // FIXME
}

QString ShogiPiece::typeSymbol(ShogiPiece::Type) {
  return "P"; // FIXME
}

// ------------------------------

class ShogiMove {
  ShogiPiece m_dropped;
  bool m_promote;
public:
  Point from;
  Point to;

  ShogiMove();
  ShogiMove(const Point& from, const Point& to, bool promote);
  ShogiMove(const ShogiPiece& piece, const Point& to);
  
  static ShogiMove createDropMove(const ShogiPiece& piece, const Point& to);
  QString toString(int) const;
  
  bool operator==(const ShogiMove& other) const;
  
  const ShogiPiece& dropped() const { return m_dropped; }
  bool promote() const { return m_promote; }
  bool valid() const { return to.valid(); }
};

ShogiMove::ShogiMove()
: m_promote(true)
, from(Point::invalid())
, to(Point::invalid()) { }

ShogiMove::ShogiMove(const Point& from, const Point& to, bool promote)
: m_promote(promote)
, from(from)
, to(to) { }

ShogiMove::ShogiMove(const ShogiPiece& piece, const Point& to)
: m_dropped(piece)
, m_promote(true)
, from(Point::invalid())
, to(to) { }

ShogiMove ShogiMove::createDropMove(const ShogiPiece& piece, const Point& to) {
  return ShogiMove(piece, to);
}

QString ShogiMove::toString(int) const {
  return "";
}

bool ShogiMove::operator==(const ShogiMove& other) const {
  if (m_dropped)
    return m_dropped == other.m_dropped
        && to == other.to;
  else
    return m_promote == other.m_promote
        && to == other.to
        && from == other.from;
}

// ------------------------------

class ShogiPosition {
public:
  typedef ShogiPiece Piece;
  typedef ShogiMove Move;
  typedef std::map<ShogiPiece, int> Pool;
private:
  Piece::Color m_turn;
  Grid<Piece> m_board;
  Pool m_pool;
public:
  ShogiPosition();
  ShogiPosition(const ShogiPosition&);
  ShogiPosition(Piece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  ShogiPosition(const QList<boost::shared_ptr<BaseOpt> >& opts);
  virtual ShogiPosition* clone() const { return new ShogiPosition(*this); }
  virtual ~ShogiPosition() { }
  
  virtual void setup();
  
  bool testMove(Move&) const { return true; }
   
  virtual void addToPool(const Piece& p, int n) { m_pool[p] += n; }
  virtual void removeFromPool(const Piece& p, int n) {
    if((m_pool[p] -= n) <= 0)
      m_pool.erase(p);
  }
  Pool& pool() { return m_pool; }
  const Pool& pool() const { return m_pool; }
  
  const ShogiPiece* get(const Point& p) const;
  ShogiPiece* get(const Point& p); 
  void set(const Point& p, Piece* piece);
   
  Piece::Color turn() const { return m_turn; }
  void setTurn(Piece::Color turn) { m_turn = turn; }
  Piece::Color previousTurn() const { return Piece::oppositeColor(m_turn); }
  void switchTurn() { m_turn = Piece::oppositeColor(m_turn); }
  
  void move(const ShogiMove& m);
  
  void fromFEN(const QString&, bool& ok) { ok = false; }
  QString fen(int, int) const { return ""; }
  bool operator==(const ShogiPosition& p) const;
  
  static Move getVerboseMove(Piece::Color turn, const VerboseNotation& m);
  Move getMove(const AlgebraicNotation&, bool& ok) const;
  boost::shared_ptr<ShogiPiece> moveHint(const ShogiMove& m) const;
  
  Point size() const { return Point(9,9); }
  void dump() const { }
  
  bool promotionZone(Piece::Color color, const Point& p);
};

ShogiPosition::ShogiPosition()
: m_turn(ShogiPiece::BLACK)
, m_board(9,9) { }

ShogiPosition::ShogiPosition(const ShogiPosition& other)
: m_turn(other.m_turn)
, m_board(other.m_board) { }

ShogiPosition::ShogiPosition(Piece::Color turn, bool, bool, bool, bool, const Point&)
: m_turn(turn) 
, m_board(9, 9) { }

ShogiPosition::ShogiPosition(const QList<boost::shared_ptr<BaseOpt> >&)
: m_turn(ShogiPiece::BLACK)
, m_board(9,9) { }

ShogiPosition::Move ShogiPosition::getVerboseMove(Piece::Color, const VerboseNotation&) {
  return Move();
}

ShogiPosition::Move ShogiPosition::getMove(const AlgebraicNotation&, bool& ok) const {
  ok = false;
  return Move();
}

const ShogiPiece* ShogiPosition::get(const Point& p) const {
  return m_board.valid(p) && m_board[p] ? &m_board[p] : 0;
}

ShogiPiece* ShogiPosition::get(const Point& p) {
  return m_board.valid(p) && m_board[p] ? &m_board[p] : 0;
}

void ShogiPosition::set(const Point& p, ShogiPiece* piece) {
  if (!m_board.valid(p)) return;
  if (piece)
    m_board[p] = *piece;
  else
    m_board[p] = Piece();
}

bool ShogiPosition::operator==(const ShogiPosition& p) const {
  return m_turn == p.m_turn
      && m_board == p.m_board;
}

boost::shared_ptr<ShogiPiece> ShogiPosition::moveHint(const ShogiMove& m) const {
  if (m.dropped()) return boost::shared_ptr<ShogiPiece>(new ShogiPiece(m.dropped()));
  else return boost::shared_ptr<ShogiPiece>();
}

bool ShogiPosition::promotionZone(Piece::Color color, const Point& p) {
  return color == ShogiPiece::WHITE ? p.y >= 6 : p.y <= 2; 
}

#define SET_PIECE(i,j, color, type) m_board[Point(i,j)] = Piece(ShogiPiece::color, ShogiPiece::type)
void ShogiPosition::setup() {
  for (int i = 0; i < 9; i++) {
    SET_PIECE(i, 2, WHITE, PAWN);
    SET_PIECE(i, 6, BLACK, PAWN);
  }

  SET_PIECE(0,0, WHITE, LANCE);
  SET_PIECE(1,0, WHITE, KNIGHT);
  SET_PIECE(2,0, WHITE, SILVER);
  SET_PIECE(3,0, WHITE, GOLD);
  SET_PIECE(4,0, WHITE, KING);
  SET_PIECE(5,0, WHITE, GOLD);
  SET_PIECE(6,0, WHITE, SILVER);
  SET_PIECE(7,0, WHITE, KNIGHT);
  SET_PIECE(8,0, WHITE, LANCE);
  SET_PIECE(1,1, WHITE, ROOK);
  SET_PIECE(7,1, WHITE, BISHOP);

  SET_PIECE(0,8, BLACK, LANCE);
  SET_PIECE(1,8, BLACK, KNIGHT);
  SET_PIECE(2,8, BLACK, SILVER);
  SET_PIECE(3,8, BLACK, GOLD);
  SET_PIECE(4,8, BLACK, KING);
  SET_PIECE(5,8, BLACK, GOLD);
  SET_PIECE(6,8, BLACK, SILVER);
  SET_PIECE(7,8, BLACK, KNIGHT);
  SET_PIECE(8,8, BLACK, LANCE);
  SET_PIECE(1,7, BLACK, BISHOP);
  SET_PIECE(7,7, BLACK, ROOK);


  m_turn = ShogiPiece::BLACK;
}
#undef SET_PIECE

void ShogiPosition::move(const ShogiMove& m) {
  if (m.dropped())
    m_board[m.to] = m.dropped();
  else {
    if (Piece captured = m_board[m.to]) {
      std::cout << "adding to pool" << std::endl;
      addToPool(Piece(captured.color(), captured.type(), false), 1);
    }

    m_board[m.to] = m_board[m.from];
    m_board[m.from] = Piece();
  }
  
  if (promotionZone(m_turn, m.to) && m.promote()) {
    m_board[m.to].promote();
  }
  
  switchTurn();
}

template <>
class MoveSerializer<ShogiPosition> {
public:
  MoveSerializer(const ShogiMove&, const ShogiPosition&) { }
  QString SAN() const { return ""; }
};

class ShogiAnimator : protected CrazyhouseAnimator {
protected:
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
  virtual boost::shared_ptr<MovementAnimation>
    createMovementAnimation(const Element& element, const QPoint& destination);
public:
  ShogiAnimator(PointConverter* converter, GraphicalPosition* position);
  virtual ~ShogiAnimator(){}
  virtual AnimationPtr warp(AbstractPosition::Ptr);
  virtual AnimationPtr forward(AbstractPosition::Ptr, const ShogiMove& move);
  virtual AnimationPtr back(AbstractPosition::Ptr, const ShogiMove& move);
};

boost::shared_ptr<MovementAnimation>
ShogiAnimator::createMovementAnimation(const Element& element, const QPoint& destination) {
  if (element.piece()->type() == static_cast<int>(ShogiPiece::KNIGHT))
    return boost::shared_ptr<MovementAnimation>(new KnightMovementAnimation(element.sprite(),
                                                       destination, m_anim_rotate, 1.0));
  else
    return boost::shared_ptr<MovementAnimation>(new MovementAnimation(element.sprite(),
                                                                destination, 1.0));
}

ShogiAnimator::ShogiAnimator(PointConverter* converter, GraphicalPosition* position)
: CrazyhouseAnimator(converter, position) { }

ShogiAnimator::AnimationPtr ShogiAnimator::warp(AbstractPosition::Ptr pos) {
  return CrazyhouseAnimator::warp(pos);
}

ShogiAnimator::AnimationPtr ShogiAnimator::forward(AbstractPosition::Ptr pos, const ShogiMove& move) {
  if (move.dropped())
    return CrazyhouseAnimator::forward(pos, CrazyhouseMove(CrazyhousePiece(WHITE, KING), move.to));
  else
    return CrazyhouseAnimator::forward(pos, CrazyhouseMove(move.from, move.to));
}

ShogiAnimator::AnimationPtr ShogiAnimator::back(AbstractPosition::Ptr pos, const ShogiMove& move) {
  if (move.dropped())
    return CrazyhouseAnimator::back(pos, CrazyhouseMove(CrazyhousePiece(WHITE, KING), move.to));
  else
    return CrazyhouseAnimator::back(pos, CrazyhouseMove(move.from, move.to));
}


class ShogiVariantInfo {
public:
  typedef ShogiPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef ShogiAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static QStringList borderCoords(){
    return QStringList() << "i" << "h" << "g" << "f" << "e" << "d" << "c" << "b" << "a"
                         << "9" << "8" << "7" << "6" << "5" << "4" << "3" << "2" << "1"; 
  }
  static int moveListLayout() { return 0; }
  static OptList positionOptions() { return OptList(); }
  static const char *m_name;
  static const char *m_theme_proxy;
};

const char *ShogiVariantInfo::m_name = "Shogi";
const char *ShogiVariantInfo::m_theme_proxy = "Shogi";


void ShogiVariantInfo::forallPieces(PieceFunction& f) {
  ChessVariant::forallPieces(f);
}

VariantInfo* ShogiVariant::static_shogi_variant = 0;

VariantInfo* ShogiVariant::info() {
  if (!static_shogi_variant)
    static_shogi_variant = new WrappedVariantInfo<ShogiVariantInfo>;
  return static_shogi_variant;
}
