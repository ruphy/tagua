#include "xiangqi.h"
#include "xchess/animator.h"
#include "xchess/piece.h"
#include "xchess/move.h"
#include "highlevel.h"
#include "moveserializer.impl.h"
#include "crazyhouse_p.h"

class XiangQiPiece {
public:
  enum Type {
    GENERAL,
    HORSE,
    CHARIOT,
    ADVISOR,
    CANNON,
    SOLDIER,
    ELEPHANT,
    INVALID_TYPE
  };
  enum Color {
    RED,
    BLACK,
    INVALID_COLOR
  };
  typedef bool PromotionType;
private:
  Color m_color;
  Type m_type;
  bool m_promoted;
public:
  XiangQiPiece()
  : m_color(INVALID_COLOR)
  , m_type(INVALID_TYPE)
  , m_promoted(false) { }
  XiangQiPiece(XiangQiPiece::Color color, XiangQiPiece::Type type, bool promoted = false);
  XiangQiPiece(const XiangQiPiece& other);

  bool inPalace(const Point& p) const;

  void promote() { m_promoted = true; }
  bool promoted() { return m_promoted; }

  bool operator<(const XiangQiPiece& p) const {
    if (m_promoted == p.m_promoted)
      if (m_color == p.m_color)
        return m_type < p.m_type;
      else
        return m_color < p.m_color;
    else
      return m_promoted < p.m_promoted;
  }

  QString name() const;
  static QString typeName(XiangQiPiece::Type t);
  bool valid() const { return m_color != INVALID_COLOR && m_type != INVALID_TYPE; }
  operator bool() const { return valid(); }
  bool operator!() const { return !valid(); }

  bool operator==(const XiangQiPiece& p) const {
    return m_promoted == p.m_promoted &&
           m_color == p.m_color &&
           m_type == p.m_type;
  }

  bool operator!=(const XiangQiPiece& p) const {
    return !(operator==(p));
  }

  static Type getType(const QString& t);
  static QString typeSymbol(XiangQiPiece::Type t);

  bool canMove(const class XiangQiPosition&, const Point&, const Point&) const;
  Color color() const { return m_color; }
  Type type() const { return m_type; }

  static Color oppositeColor(Color c) { return c == RED ? BLACK : BLACK ? RED : INVALID_COLOR; }
  Point direction() const { return Point(0, m_color == RED ? -1 : 1); }
};

XiangQiPiece::XiangQiPiece(XiangQiPiece::Color color, XiangQiPiece::Type type, bool promoted)
: m_color(color)
, m_type(type)
, m_promoted(promoted) { }

XiangQiPiece::XiangQiPiece(const XiangQiPiece& other)
: m_color(other.m_color)
, m_type(other.m_type)
, m_promoted(other.m_promoted) { }

bool XiangQiPiece::inPalace(const Point& p) const {
  if(m_color == RED)
    return p.x > 2 && p.x < 6 && p.y > 6;
  else
    return p.x > 2 && p.x < 6 && p.y < 3;
}

QString XiangQiPiece::name() const {
  QString res = m_color == RED ? "red_" : "black_";
  if (m_promoted)
    res += "p_";
  res += typeName(m_type);
  return res;
}

QString XiangQiPiece::typeName(XiangQiPiece::Type t) {
  switch (t) {
    case GENERAL:
      return "general";
    case HORSE:
      return "horse";
    case CHARIOT:
      return "chariot";
    case ADVISOR:
      return "advisor";
    case CANNON:
      return "cannon";
    case SOLDIER:
      return "soldier";
    case ELEPHANT:
      return "elephant";
    default:
      return "unknown";
  }
}

XiangQiPiece::Type XiangQiPiece::getType(const QString&) {
  return GENERAL; // FIXME
}

QString XiangQiPiece::typeSymbol(XiangQiPiece::Type t) {
  switch (t) {
    case GENERAL:
      return "G";
    case HORSE:
      return "H";
    case CHARIOT:
      return "R";
    case ADVISOR:
      return "A";
    case CANNON:
      return "C";
    case SOLDIER:
      return "S";
    case ELEPHANT:
      return "E";
    default:
      return "?";
  }
}

// ------------------------------

class XiangQiMove {
  XiangQiPiece m_dropped;
  bool m_promote;
  template<typename T> friend class MoveSerializer;
public:
  Point from;
  Point to;

  XiangQiMove();
  XiangQiMove(const Point& from, const Point& to, bool promote);
  XiangQiMove(const XiangQiPiece& piece, const Point& to);

  static XiangQiMove createDropMove(const XiangQiPiece& piece, const Point& to);
  QString toString(int) const;

  bool operator==(const XiangQiMove& other) const;

  const XiangQiPiece& dropped() const { return m_dropped; }
  bool promote() const { return m_promote; }
  bool valid() const { return to.valid(); }
};

XiangQiMove::XiangQiMove()
: m_promote(true)
, from(Point::invalid())
, to(Point::invalid()) { }

XiangQiMove::XiangQiMove(const Point& from, const Point& to, bool promote)
: m_promote(promote)
, from(from)
, to(to) { }

XiangQiMove::XiangQiMove(const XiangQiPiece& piece, const Point& to)
: m_dropped(piece)
, m_promote(false)
, from(Point::invalid())
, to(to) { }

XiangQiMove XiangQiMove::createDropMove(const XiangQiPiece& piece, const Point& to) {
  return XiangQiMove(piece, to);
}

QString XiangQiMove::toString(int) const {
  return "";
}

bool XiangQiMove::operator==(const XiangQiMove& other) const {
  if (m_dropped)
    return m_dropped == other.m_dropped
        && to == other.to;
  else
    return m_promote == other.m_promote
        && to == other.to
        && from == other.from;
}

// ------------------------------

class XiangQiPosition {
public:
  typedef XiangQiPiece Piece;
  typedef XiangQiMove Move;
  typedef std::map<XiangQiPiece, int> Pool;
private:
  Piece::Color m_turn;
  Grid<Piece> m_board;
  Pool m_pool;
public:
  template<typename T> friend class MoveSerializer;
  XiangQiPosition();
  XiangQiPosition(const XiangQiPosition&);
  XiangQiPosition(Piece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  XiangQiPosition(const QList<boost::shared_ptr<BaseOpt> >& opts);
  virtual XiangQiPosition* clone() const { return new XiangQiPosition(*this); }
  virtual ~XiangQiPosition() { }

  virtual void setup();

  bool testMove(Move&) const;
  bool pseudolegal(Move& m) const;

  virtual void addToPool(const Piece&, int) { }
  virtual void removeFromPool(const Piece&, int) {  }
  Pool& pool() { return m_pool; }
  const Pool& pool() const { return m_pool; }

  const XiangQiPiece* get(const Point& p) const;
  XiangQiPiece* get(const Point& p);
  void set(const Point& p, Piece* piece);

  XiangQiPiece operator[](const Point& p) const { return m_board[p]; }

  Piece::Color turn() const { return m_turn; }
  void setTurn(Piece::Color turn) { m_turn = turn; }
  Piece::Color previousTurn() const { return Piece::oppositeColor(m_turn); }
  void switchTurn() { m_turn = Piece::oppositeColor(m_turn); }

  void move(const XiangQiMove& m);

  void fromFEN(const QString&, bool& ok) { ok = false; }
  QString fen(int, int) const { return ""; }
  bool operator==(const XiangQiPosition& p) const;

  static Move getVerboseMove(Piece::Color turn, const VerboseNotation& m);
  Move getMove(const AlgebraicNotation&, bool& ok) const;
  boost::shared_ptr<XiangQiPiece> moveHint(const XiangQiMove& m) const;

  Point size() const { return Point(9,10); }
  void dump() const { }

  PathInfo path(const Point& from, const Point& to) const { return m_board.path(from, to); }
};

XiangQiPosition::XiangQiPosition()
: m_turn(XiangQiPiece::BLACK)
, m_board(9,10) { }

XiangQiPosition::XiangQiPosition(const XiangQiPosition& other)
: m_turn(other.m_turn)
, m_board(other.m_board)
, m_pool(other.m_pool) { }

XiangQiPosition::XiangQiPosition(Piece::Color turn, bool, bool, bool, bool, const Point&)
: m_turn(turn)
, m_board(9,10) { }

XiangQiPosition::XiangQiPosition(const QList<boost::shared_ptr<BaseOpt> >&)
: m_turn(XiangQiPiece::BLACK)
, m_board(9,10) { }

bool XiangQiPiece::canMove(const XiangQiPosition& pos,
                         const Point& from, const Point& to) const {
  if (!from.valid()) return false;
  if (!to.valid()) return false;
  if (from == to) return false;
  if (pos[to].color() == m_color) return false;
  Point delta = to - from;

  switch (m_type) {
  case GENERAL:
    return (abs(delta.x) + abs(delta.y) == 1)
          && inPalace(to);
  case ADVISOR:
    return (abs(delta.x) == 1) && (abs(delta.y) == 1)
            && inPalace(to);
  case ELEPHANT:
    return (abs(delta.x) == 2) && (abs(delta.y) == 2)
            && !pos[Point((to.x+from.x)/2, (to.y+from.y)/2)]
            && (m_color == RED ? to.y > 5 : to.y < 6);
  case HORSE:
    return (delta.x*delta.x + delta.y*delta.y) == 5
            && !pos[Point((to.x+from.x*2+1)/3, (to.y+from.y*2+1)/3)];
  case CHARIOT:
    {
      PathInfo path = pos.path(from, to);
      return path.parallel() && path.clear();
    }
  case CANNON:
    {
      PathInfo path = pos.path(from, to);
      return path.parallel() &&
              (pos[to] ? path.numObstacles() == 1 : path.clear());
    }
  case SOLDIER:
    return (delta.x == 0 && delta.y == direction().y)
          || (abs(delta.x) == 1 && delta.y == 0 &&
             (m_color == RED ? to.y < 6 : to.y > 5));
  default:
    return false;
  }
}


XiangQiPosition::Move XiangQiPosition::getVerboseMove(Piece::Color, const VerboseNotation&) {
  return Move();
}

XiangQiPosition::Move XiangQiPosition::getMove(const AlgebraicNotation&, bool& ok) const {
  ok = false;
  return Move();
}

const XiangQiPiece* XiangQiPosition::get(const Point& p) const {
  return m_board.valid(p) && m_board[p] ? &m_board[p] : 0;
}

XiangQiPiece* XiangQiPosition::get(const Point& p) {
  return m_board.valid(p) && m_board[p] ? &m_board[p] : 0;
}

void XiangQiPosition::set(const Point& p, XiangQiPiece* piece) {
  if (!m_board.valid(p)) return;
  if (piece)
    m_board[p] = *piece;
  else
    m_board[p] = Piece();
}

bool XiangQiPosition::operator==(const XiangQiPosition& p) const {
  return m_turn == p.m_turn
      && m_board == p.m_board;
}

boost::shared_ptr<XiangQiPiece> XiangQiPosition::moveHint(const XiangQiMove& m) const {
  if (m.dropped()) return boost::shared_ptr<XiangQiPiece>(new XiangQiPiece(m.dropped()));
  else return boost::shared_ptr<XiangQiPiece>();
}

#define SET_PIECE(i,j, color, type) m_board[Point(i,j)] = Piece(XiangQiPiece::color, XiangQiPiece::type)
void XiangQiPosition::setup() {
  SET_PIECE(0,0, BLACK, CHARIOT);
  SET_PIECE(1,0, BLACK, HORSE);
  SET_PIECE(2,0, BLACK, ELEPHANT);
  SET_PIECE(3,0, BLACK, ADVISOR);
  SET_PIECE(4,0, BLACK, GENERAL);
  SET_PIECE(5,0, BLACK, ADVISOR);
  SET_PIECE(6,0, BLACK, ELEPHANT);
  SET_PIECE(7,0, BLACK, HORSE);
  SET_PIECE(8,0, BLACK, CHARIOT);
  SET_PIECE(1,2, BLACK, CANNON);
  SET_PIECE(7,2, BLACK, CANNON);
  for(int i=0;i<=8;i+=2)
    SET_PIECE(i, 3, BLACK, SOLDIER);

  SET_PIECE(0,9, RED, CHARIOT);
  SET_PIECE(1,9, RED, HORSE);
  SET_PIECE(2,9, RED, ELEPHANT);
  SET_PIECE(3,9, RED, ADVISOR);
  SET_PIECE(4,9, RED, GENERAL);
  SET_PIECE(5,9, RED, ADVISOR);
  SET_PIECE(6,9, RED, ELEPHANT);
  SET_PIECE(7,9, RED, HORSE);
  SET_PIECE(8,9, RED, CHARIOT);
  SET_PIECE(1,7, RED, CANNON);
  SET_PIECE(7,7, RED, CANNON);
  for(int i=0;i<=8;i+=2)
    SET_PIECE(i, 6, RED, SOLDIER);

  m_turn = XiangQiPiece::RED;
}
#undef SET_PIECE

bool XiangQiPosition::testMove(Move& m) const {
  return pseudolegal(m);
}

bool XiangQiPosition::pseudolegal(Move& m) const {
  const Piece& p = m_board[m.from];
  return p.canMove(*this, m.from, m.to);
}

void XiangQiPosition::move(const XiangQiMove& m) {
  if (Piece captured = m_board[m.to]) {
    addToPool(Piece(Piece::oppositeColor(captured.color()),
                    captured.type(), false), 1);
  }

  m_board[m.to] = m_board[m.from];
  m_board[m.from] = Piece();

  switchTurn();
}

template <>
class MoveSerializer<XiangQiPosition> {
  const XiangQiMove&     m_move;
  const XiangQiPosition& m_ref;
public:
  MoveSerializer(const XiangQiMove& m, const XiangQiPosition& r)
    : m_move(m), m_ref(r) { }
  QString SAN() const {
    XiangQiPiece p = m_ref[m_move.from];
    int order = 0;
    int num = 1;
    for(int i=0;i<10;i++) {
      Point f(m_move.from.x, i);
      if(f != m_move.from && m_ref[f] == p) {
        XiangQiMove m(f, m_move.to-m_move.from+f, false);
        if(m_ref.testMove(m)) {
          order = ((i < m_move.to.y) != (p.color() == XiangQiPiece::RED)) ? -1 : 1;
          num++;
        }
      }
    }

    QString retv;
    if(num >= 4 && (order == 0 || order == num-1)) //4/5 pawns case
      retv += (order==0) ? "-" : "+";
    else
      retv += XiangQiPiece::typeSymbol(p.type());

    if(num==1 || (num==3 && order==1) || (num==5 && order==2))
      retv += QString::number((p.color() == XiangQiPiece::RED) ? 9-m_move.from.x : m_move.from.x+1);
    else
      retv += (order<num/2) ? "-" : "+";

    if(m_move.from.x == m_move.to.x) {
      retv += ((m_move.to.y>m_move.from.y) != (p.color() == XiangQiPiece::RED)) ? "+" : "-";
      retv += QString::number(abs(m_move.to.y-m_move.from.y));
    }
    else {
      retv += (m_move.to.y == m_move.from.y) ? "." :
            ((m_move.to.y>m_move.from.y) != (p.color() == XiangQiPiece::RED)) ? "+" : "-";
      retv += QString::number((p.color() == XiangQiPiece::RED) ? 9-m_move.to.x : m_move.to.x+1);
    }
    return retv;
  }

  DecoratedMove toDecoratedMove() const {
    XiangQiPiece p = m_ref[m_move.from];
    int order = 0;
    int num = 1;
    for(int i=0;i<10;i++) {
      Point f(m_move.from.x, i);
      if(f != m_move.from && m_ref[f] == p) {
        XiangQiMove m(f, m_move.to-m_move.from+f, false);
        if(m_ref.testMove(m)) {
          order = ((i < m_move.to.y) != (p.color() == XiangQiPiece::RED)) ? -1 : 1;
          num++;
        }
      }
    }

    DecoratedMove retv;
    MovePart piece("");
    if(num >= 4 && (order == 0 || order == num-1)) //4/5 pawns case
      piece = MovePart((order==0) ? "rear" : "front", MovePart::Figurine);
    else
      piece = MovePart(p.name(), MovePart::Figurine);

    if(num==1 || (num==3 && order==1) || (num==5 && order==2)) {
      retv += piece;
      retv += MovePart(((p.color() == XiangQiPiece::RED) ? "rn_" : "bn_")
                + QString::number((p.color() == XiangQiPiece::RED) ?
                                                    9-m_move.from.x : m_move.from.x+1),
                                                                        MovePart::Figurine);
    }
    else {
      retv += MovePart((order<num/2) ? "rear" : "front", MovePart::Figurine);
      retv += piece;
    }

    if(m_move.from.x == m_move.to.x) {
      retv += MovePart(((m_move.to.y>m_move.from.y) != (p.color() == XiangQiPiece::RED))
                                ? "advances" : "retreats", MovePart::Figurine);
      retv += MovePart(((p.color() == XiangQiPiece::RED) ? "rn_" : "bn_")
                + QString::number(abs(m_move.to.y-m_move.from.y)), MovePart::Figurine);
    }
    else {
      retv += MovePart((m_move.to.y == m_move.from.y) ? "traverses" :
            ((m_move.to.y>m_move.from.y) != (p.color() == XiangQiPiece::RED)) ? "advances" :
                                                                "retreats", MovePart::Figurine);
      retv += MovePart(((p.color() == XiangQiPiece::RED) ? "rn_" : "bn_")
                + QString::number((p.color() == XiangQiPiece::RED) ? 9-m_move.to.x : m_move.to.x+1),
                                                                               MovePart::Figurine);
    }
    return retv;
  }
};

class XiangQiAnimator : protected CrazyhouseAnimator {
protected:
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
  virtual boost::shared_ptr<MovementAnimation>
    createMovementAnimation(const Element& element, const QPoint& destination);
public:
  XiangQiAnimator(PointConverter* converter, GraphicalPosition* position);
  virtual ~XiangQiAnimator(){}
  virtual AnimationPtr warp(AbstractPosition::Ptr);
  virtual AnimationPtr forward(AbstractPosition::Ptr, const XiangQiMove& move);
  virtual AnimationPtr back(AbstractPosition::Ptr, const XiangQiMove& move);
};

boost::shared_ptr<MovementAnimation>
XiangQiAnimator::createMovementAnimation(const Element& element, const QPoint& destination) {
  if (element.piece()->type() == static_cast<int>(XiangQiPiece::HORSE))
    return boost::shared_ptr<MovementAnimation>(new KnightMovementAnimation(element.sprite(),
                                                       destination, m_anim_rotate, 1.0));
  else
    return boost::shared_ptr<MovementAnimation>(new MovementAnimation(element.sprite(),
                                                                destination, 1.0));
}

XiangQiAnimator::XiangQiAnimator(PointConverter* converter, GraphicalPosition* position)
: CrazyhouseAnimator(converter, position) { }

XiangQiAnimator::AnimationPtr XiangQiAnimator::warp(AbstractPosition::Ptr pos) {
  return CrazyhouseAnimator::warp(pos);
}

XiangQiAnimator::AnimationPtr XiangQiAnimator::forward(AbstractPosition::Ptr pos, const XiangQiMove& move) {
  if (move.dropped())
    return CrazyhouseAnimator::forward(pos, CrazyhouseMove(CrazyhousePiece(WHITE, KING), move.to));
  else
    return CrazyhouseAnimator::forward(pos, CrazyhouseMove(move.from, move.to));
}

XiangQiAnimator::AnimationPtr XiangQiAnimator::back(AbstractPosition::Ptr pos, const XiangQiMove& move) {
  if (move.dropped())
    return CrazyhouseAnimator::back(pos, CrazyhouseMove(CrazyhousePiece(WHITE, KING), move.to));
  else
    return CrazyhouseAnimator::back(pos, CrazyhouseMove(move.from, move.to));
}


class XiangQiVariantInfo {
public:
  typedef XiangQiPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef XiangQiAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static QStringList borderCoords(){
    return QStringList() << "i" << "h" << "g" << "f" << "e" << "d" << "c" << "b" << "a"
                         << "10" << "9" << "8" << "7" << "6" << "5" << "4" << "3" << "2" << "1";
  }
  static int moveListLayout() { return 0; }
  static OptList positionOptions() { return OptList(); }
  static const char *m_name;
  static const char *m_theme_proxy;
};

const char *XiangQiVariantInfo::m_name = "XiangQi";
const char *XiangQiVariantInfo::m_theme_proxy = "XiangQi";


void XiangQiVariantInfo::forallPieces(PieceFunction& f) {
  ChessVariant::forallPieces(f);
}

VariantInfo* XiangQiVariant::static_xiangqi_variant = 0;

VariantInfo* XiangQiVariant::info() {
  if (!static_xiangqi_variant)
    static_xiangqi_variant = new WrappedVariantInfo<XiangQiVariantInfo>;
  return static_xiangqi_variant;
}


template <>
struct MoveFactory<XiangQiVariantInfo> {
  static XiangQiMove createNormalMove(const NormalUserMove& move) {
    return XiangQiMove(move.from, move.to, move.promotionType >= 0);
  }
  static XiangQiMove createDropMove(const XiangQiPiece& dropped, const Point& to) {
    return XiangQiMove(dropped, to);
  }

  static NormalUserMove toNormal(const XiangQiMove& move) {
    return NormalUserMove(move.from, move.to);
  }
};
