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
#include "urlrequester.h"
#include "kdewidgets.h"
#include "option.h"

class OptCheckBox : public QCheckBox {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<BoolOpt> m_opt;

public:
  OptCheckBox(boost::shared_ptr<BoolOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(bool);
};

class OptRadioButton : public QRadioButton {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<SelectOpt> m_opt;
  int m_index;

public:
  OptRadioButton(boost::shared_ptr<SelectOpt> opt, int i, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(bool);
};

class OptSpinBox : public QSpinBox {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<IntOpt> m_opt;

public:
  OptSpinBox(boost::shared_ptr<IntOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(int);
};

class OptLineEdit : public QLineEdit {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<StringOpt> m_opt;

public:
  OptLineEdit(boost::shared_ptr<StringOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QString&);
};

class OptUrlRequester : public UrlRequester {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<UrlOpt> m_opt;

public:
  OptUrlRequester(boost::shared_ptr<UrlOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QString&);
};

class OptComboBox : public QComboBox {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<ComboOpt> m_opt;

public:
  OptComboBox(boost::shared_ptr<ComboOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(int);
};

class OptColorButton : public KColorButton {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<ColorOpt> m_opt;

public:
  OptColorButton(boost::shared_ptr<ColorOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QColor&);
};

class OptFontRequester : public KFontRequester {
  Q_OBJECT
  OptionWidget *m_owner;
  boost::shared_ptr<FontOpt> m_opt;

public:
  OptFontRequester(boost::shared_ptr<FontOpt> opt, OptionWidget *owner, QWidget *parent = NULL);

private slots:
  void setOpt(const QFont&);
};

#endif //OPTION_P_H
