/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef OPTION_P_H
#define OPTION_P_H

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include "kurlrequester.h"
#include "kcolorbutton.h"
#include "kfontrequester.h"
#include "option.h"

class OptCheckBox : public QCheckBox {
  Q_OBJECT
  OptionWidget *m_owner;
  BoolOptPtr m_opt;

public:
  OptCheckBox(BoolOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(bool);
};

class OptRadioButton : public QRadioButton {
  Q_OBJECT
  OptionWidget *m_owner;
  SelectOptPtr m_opt;
  int m_index;

public:
  OptRadioButton(SelectOptPtr opt, int i, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(bool);
};

class OptSpinBox : public QSpinBox {
  Q_OBJECT
  OptionWidget *m_owner;
  IntOptPtr m_opt;

public:
  OptSpinBox(IntOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(int);
};

class OptSlider : public QSlider {
  Q_OBJECT
  OptionWidget *m_owner;
  IntOptPtr m_opt;

public:
  OptSlider(IntOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(int);
};

class OptLineEdit : public QLineEdit {
  Q_OBJECT
  OptionWidget *m_owner;
  StringOptPtr m_opt;

public:
  OptLineEdit(StringOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QString&);
};

class OptUrlRequester : public KUrlRequester {
  Q_OBJECT
  OptionWidget *m_owner;
  UrlOptPtr m_opt;

public:
  OptUrlRequester(UrlOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QString&);
};

class OptComboBox : public QComboBox {
  Q_OBJECT
  OptionWidget *m_owner;
  ComboOptPtr m_opt;

public:
  OptComboBox(ComboOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(int);
};

class OptColorButton : public KColorButton {
  Q_OBJECT
  OptionWidget *m_owner;
  ColorOptPtr m_opt;

public:
  OptColorButton(ColorOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QColor&);
};

class OptFontRequester : public KFontRequester {
  Q_OBJECT
  OptionWidget *m_owner;
  FontOptPtr m_opt;

public:
  OptFontRequester(FontOptPtr opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QFont&);
};

#endif //OPTION_P_H
