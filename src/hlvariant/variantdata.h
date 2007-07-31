#ifndef HLVARIANTS__VARIANTDATA_H
#define HLVARIANTS__VARIANTDATA_H

template <typename Variant>
struct VariantData {
  typedef typename Variant::MoveGenerator MoveGenerator;
  typedef typename MoveGenerator::LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename GameState::Move Move;
  typedef typename Board::Piece Piece;
  
  typedef typename Variant::Serializer Serializer;
};

#endif // HLVARIANTS__VARIANTDATA_H

