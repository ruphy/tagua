/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLINE_H
#define HLINE_H

#include <QTextCharFormat>
#include <vector>
#include <QString>
#include <QColor>

class HLineExecutor {
public:
  virtual ~HLineExecutor() { }
  virtual void writeChunk(const QString&) = 0;
  virtual void setFormat(const QTextCharFormat&) = 0;
};

class HLine {
  class Region {
    int m_end;
    QTextCharFormat m_format;
  public:
    Region(int end, QTextCharFormat format)
    : m_end(end)
    , m_format(format) { }
    
    int end() const { return m_end; }
    const QTextCharFormat& format() const { return m_format; }
    QTextCharFormat& format() { return m_format; }
    
    void setEnd(int index) { m_end = index; }
    void setFormat(const QTextCharFormat& format) { m_format = format; }
  };
  struct Format {
    std::pair<bool, bool> m_bold;
    std::pair<bool, bool> m_italic;
    std::pair<bool, QColor> m_color;
    Format()
    : m_bold(false, false)
    , m_italic(false, false)
    , m_color(false, QColor()) { }
    void applyTo(QTextCharFormat& fmt) const;
  };
  
  std::vector<Region> m_regions;
  QString m_text;
  
  int begin(uint index) const;
  uint findRegion(int index) const;
  uint splitRegion(int index);
  void setFormat(int begin, int end, const Format&);
public:
  HLine(const QString& text, const QTextCharFormat& baseFormat);
  HLine(const HLine& other);
  
  const QTextCharFormat& getFormat(int index) const;
  
  void setBold(int begin, int end, bool value);
  void setItalic(int begin, int end, bool value);
  void setColor(int begin, int end, const QColor& color);
  QString mid(int begin, int end) const;
  int length() const { return m_text.length(); }
  HLine* extract(int begin, int end) const;
  HLine* append(const HLine& other, int begin, int end) const;
  
  void dump() const;
  
  void run(HLineExecutor&) const;
};

#endif // HLINE_H
