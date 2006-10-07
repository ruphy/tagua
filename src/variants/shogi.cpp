#if 0
#include "shogi.h"
#include "xchess/animator.h"
#include "xchess/piece.h"
#include "xchess/move.h"
#include "highlevel.h"
#include "moveserializer.impl.h"

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
  typedef ChessPiece::Color Color;
  
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
  
  bool operator==(const ShogiPiece& p) {
    return m_promoted == p.m_promoted &&
           m_color == p.m_color &&
           m_type == p.m_type;
  }
  
  bool operator!=(const ShogiPiece& p) {
    return !(operator==(p));
  }
  
  static Type getType(const QString& t);
  static QString typeSymbol(ShogiPiece::Type t);
  
  bool canMove(const class ShogiPosition&, const Point&, const Point&) const { return true; }
  Color color() const { return m_color; }
  Type type() const { return m_type; }
  
  static Color oppositeColor(Color c) { return ChessPiece::oppositeColor(c); }
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
    res += "p";
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

ShogiPiece::Type ShogiPiece::getType(const QString& t) {
  return KING; // FIXME
}

QString ShogiPiece::typeSymbol(ShogiPiece::Type t) {
  return "P"; // FIXME
}

// ------------------------------

class ShogiPosition : public Position<ChessMove, ShogiPiece, Grid<ShogiPiece> > {
  typedef Position<ChessMove, ShogiPiece, Grid<ShogiPiece> > BasePosition;
public:
  typedef ShogiPiece Piece;
  ShogiPosition();
  ShogiPosition(const BasePosition&);
  ShogiPosition(Piece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  virtual ShogiPosition* clone() const { return new ShogiPosition(*this); }
  
  virtual void setup();
};

ShogiPosition::ShogiPosition()
: BasePosition(9, 9) { }

ShogiPosition::ShogiPosition(const BasePosition& other)
: BasePosition(other) { }

ShogiPosition::ShogiPosition(Piece::Color turn, bool wk, bool wq,
                               bool bk, bool bq, const Point& ep)
: BasePosition(turn, wk, wq, bk, bq, ep) { }

#define SET_PIECE(i,j, color, type) m_board[Point(i,j)] = Piece(color, ShogiPiece::type)
void ShogiPosition::setup() {
  for (int i = 0; i < 9; i++) {
    SET_PIECE(i, 2, BLACK, PAWN);
    SET_PIECE(i, 6, WHITE, PAWN);
  }

  SET_PIECE(0,0, BLACK, LANCE);
  SET_PIECE(1,0, BLACK, KNIGHT);
  SET_PIECE(2,0, BLACK, SILVER);
  SET_PIECE(3,0, BLACK, GOLD);
  SET_PIECE(4,0, BLACK, KING);
  SET_PIECE(5,0, BLACK, GOLD);
  SET_PIECE(6,0, BLACK, SILVER);
  SET_PIECE(7,0, BLACK, KNIGHT);
  SET_PIECE(8,0, BLACK, LANCE);

  SET_PIECE(0,8, WHITE, ROOK);
  SET_PIECE(1,8, WHITE, KNIGHT);
  SET_PIECE(2,8, WHITE, SILVER);
  SET_PIECE(3,8, WHITE, GOLD);
  SET_PIECE(4,8, WHITE, KING);
  SET_PIECE(5,8, WHITE, SILVER);
  SET_PIECE(6,8, WHITE, GOLD);
  SET_PIECE(7,8, WHITE, KNIGHT);
  SET_PIECE(8,8, WHITE, LANCE);

  m_turn = WHITE;
  m_enPassantSquare = Point::invalid();
  m_castleWhiteKing = true;
  m_castleWhiteQueen = true;
  m_castleBlackKing = true;
  m_castleBlackQueen = true;
}
#undef SET_PIECE


class ShogiVariantInfo {
public:
  typedef ShogiPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef ChessAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h" << "i"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9";
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

#endif