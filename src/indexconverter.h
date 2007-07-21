/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef INDEXCONVERTER_H
#define INDEXCONVERTER_H

class IndexConverter {
public:
  virtual ~IndexConverter() { };
  
  virtual QPoint toReal(int index) const = 0;
  virtual int toLogical(const QPoint& p) const = 0;
};

#endif // INDEXCONVERTER_H
