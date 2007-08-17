/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "poolinfo.h"
#include "gameinfo.h"
#include "variants.h"

QRegExp PoolInfo::s_pattern("^<b1>\\s+game\\s+(\\d+)" //game num
                            "\\s+white\\s+\\[([QKBNRP]*)\\]"  // white pieces
                            "\\s+black\\s+\\[([QKBNRP]*)\\]"  // black pieces
                            "(?:\\s+<\\-\\s+([WB][QKBNRP]))?" // added piece
                            );

PoolInfo::PoolInfo(const std::map<int, ICSGameData>& games, const QString& str)
: m_valid(false)
, m_pos_index(-1) {

//   if (s_pattern.indexIn(str) != 0)
//     return;
//     
//   m_game_num = s_pattern.cap(1).toInt();
//   std::map<int, ICSGameData>::const_iterator gi = games.find(m_game_num);
//   if (gi == games.end())
//     return;
// 
//   QString var = !gi->second.variant.isEmpty() ? gi->second.variant : QString("chess");
//   VariantInfo* variant = Variant::variant(GameInfo::variantCode(var));
//   m_pos_index = gi->second.index;
//   
//   m_pool = 
//BROKEN
#if 0
  if (s_pattern.indexIn(str) != 0)
    return;

  m_game_num = s_pattern.cap(1).toInt();
  std::map<int, ICSGameData>::const_iterator gi = games.find(m_game_num);

  if(gi == games.end())
    return;

  QString var = !gi->second.variant.isEmpty() ? gi->second.variant : QString("chess");
  VariantInfo* variant = Variant::variant(GameInfo::variantCode(var));
  m_pos_index = gi->second.index;

  m_pool = AbstractPosition::PoolPtr(new AbstractPosition::AbstractPool);
  for(int i=0;i<2;i++) {
    QString cap = s_pattern.cap(i+2);
    int color = (i==0) ? WHITE : BLACK;

    for(int j=0;j<cap.length();j++) {
      int type = variant->type(cap.at(j));
      AbstractPiece::Ptr p = variant->createPiece(color, type);
      (*m_pool)[p]++;
    }
  }

  QString added = s_pattern.cap(4);
  if(!added.isEmpty()) {
    int color = (added.at(0) == 'W') ? WHITE : BLACK;
    int type = variant->type(added.at(1));
    m_added_piece = variant->createPiece(color, type);
  }
  m_valid = true;
#endif
}
