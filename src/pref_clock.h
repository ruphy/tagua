/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREF_CLOCK_H
#define PREF_CLOCK_H

#include <QDialog>

class QSpinBox;
class QPushButton;
class Clock;
class QBoxLayout;

class PrefClock : public QDialog {
Q_OBJECT
  QSpinBox* m_timeSpin;
  QSpinBox* m_captionSpin;
  QSpinBox* m_decsSpin;
  QSpinBox* m_playerSpin;
  QPushButton* m_background_button;
  
  QPushButton* m_ok;
  QPushButton* m_cancel;
  
  Clock* m_preview;
  
  void createSpin(QBoxLayout* parent, const QString& labelText, QSpinBox*& spin);
  void load();
private slots:
  void setBackgroundColor();
public:
  PrefClock();
  
  void apply();
};

#endif // PREF_CLOCK_H
