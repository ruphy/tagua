/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "hline.h"
#include <KDebug>

class TrivialExecutor : public HLineExecutor {
public:
  virtual ~TrivialExecutor() {
    kDebug();
  }

  virtual void writeChunk(const QString& text) {
    kDebug() << text << "|";
  }

  virtual void setFormat(const QTextCharFormat&) { }
};

void HLine::Format::applyTo(QTextCharFormat& fmt) const {
  if (m_bold.first) {
    fmt.setFontWeight(m_bold.second ? QFont::Bold : QFont::Normal);
  }
  if (m_italic.first) {
    fmt.setFontItalic(m_italic.second);
  }
  if (m_color.first)
    fmt.setForeground(m_color.second);
}

HLine::HLine(const QString& text, const QTextCharFormat& baseFormat)
: m_text(text) {
  m_regions.push_back(Region(text.length(), baseFormat));
}

HLine::HLine(const HLine& other)
: m_regions(other.m_regions)
, m_text(other.m_text) { }

uint HLine::findRegion(int index) const {
  if (index >= m_text.length()) return m_regions.size();
  uint r = 0;
  while (index >= m_regions[r].end())
    r++;
  Q_ASSERT(r < m_regions.size());
  return r;
}

int HLine::begin(uint r) const {
  if (r == 0) return 0;
  else return m_regions[r - 1].end();
}

uint HLine::splitRegion(int index) {
  if (index >= m_text.length()) return m_regions.size();
  int r = findRegion(index);
  if (begin(r) != index) {
    m_regions.insert(
      m_regions.begin() + r + 1,
      m_regions[r]);
    m_regions[r].setEnd(index);
    return r + 1;
  }
  else return r;
}

const QTextCharFormat& HLine::getFormat(int index) const {
  return m_regions[findRegion(index)].format();
}

void HLine::setFormat(int begin, int end, const Format& format) {
  Q_ASSERT(begin < m_text.length());
  Q_ASSERT(end <= m_text.length());

  if (begin >= end) return;
  int r1 = splitRegion(begin);
  int r2 = splitRegion(end);

  for (int r = r1; r < r2; r++) {
    QTextCharFormat fmt = m_regions[r].format();
    format.applyTo(fmt);
    m_regions[r].setFormat(fmt);
  }
}

void HLine::setBold(int begin, int end, bool value) {
  Format format;
  format.m_bold = std::make_pair(true, value);
  setFormat(begin, end, format);
}

void HLine::setItalic(int begin, int end, bool value) {
  Format format;
  format.m_italic = std::make_pair(true, value);
  setFormat(begin, end, format);
}

void HLine::setColor(int begin, int end, const QColor& color) {
  Format format;
  format.m_color = std::make_pair(true, color);
  setFormat(begin, end, format);
}

QString HLine::mid(int begin, int end) const {
  return m_text.mid(begin, end - begin);
}

HLine* HLine::extract(int begin, int end) const {
  HLine* res = new HLine(*this);
  int r1 = res->splitRegion(begin);
  int r2 = res->splitRegion(end);

  // adjust text
  res->m_text = mid(begin, end);

  // remove superfluous regions
  res->m_regions.erase(
    res->m_regions.begin() + r2,
    res->m_regions.end());
  res->m_regions.erase(
    res->m_regions.begin(),
    res->m_regions.begin() + r1);

  // adjust regions
  for (uint i = 0; i < res->m_regions.size(); i++) {
    res->m_regions[i].setEnd(res->m_regions[i].end() - begin);
  }

  return res;
}

HLine* HLine::append(const HLine& other, int begin, int end) const {
  HLine* res = new HLine(*this);

  int old_r = res->m_regions.size();
  int old_length = res->m_text.length();

  // append text
  res->m_text += other.mid(begin, end);

  // add regions
  int r1 = other.findRegion(begin);
  int r2 = other.findRegion(end);
  std::copy(other.m_regions.begin() + r1,
       other.m_regions.begin() + r2,
       std::back_insert_iterator<std::vector<Region> >(res->m_regions));

  // adjust regions
  for (uint i = old_r; i < res->m_regions.size(); i++) {
    int new_end = old_length + res->m_regions[i].end() - begin;
    res->m_regions[i].setEnd(new_end);
  }

  return res;
}

void HLine::dump() const {
  TrivialExecutor executor;
  run(executor);
}

void HLine::run(HLineExecutor& executor) const {
  int begin = 0;
  for (uint i = 0; i < m_regions.size(); i++) {
    int end = m_regions[i].end();
    executor.setFormat(m_regions[i].format());
    executor.writeChunk(mid(begin, end));
    begin = end;
  }
}

