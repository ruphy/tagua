/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef __KBOARD_FWD_H__
#define __KBOARD_FWD_H__

class AbstractMove;
class AbstractPosition;
class AbstractPiece;
class AbstractAnimator;
class VariantInfo;

typedef boost::shared_ptr<class AbstractMove> MovePtr;
typedef boost::shared_ptr<class AbstractPosition> PositionPtr;
typedef boost::shared_ptr<class AbstractPiece> PiecePtr;
typedef boost::shared_ptr<class AbstractAnimator> AnimatorPtr;
typedef boost::shared_ptr<class VariantInfo> VariantPtr;

#endif //__KBOARD_FWD_H__
