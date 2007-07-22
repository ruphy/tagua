/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "option_p.h"


OptCheckBox::OptCheckBox(BoolOptPtr opt, OptionWidget *owner, QWidget *parent)
: QCheckBox(opt->label(), parent)
, m_owner(owner)
, m_opt(opt) {
  setChecked(m_opt->value());
  connect(this, SIGNAL(toggled(bool)), this, SLOT(setOpt(bool)));
}

void OptCheckBox::setOpt(bool b) {
  m_opt->setValue(b);
  m_owner->notifyChange();
}

OptRadioButton::OptRadioButton(SelectOptPtr opt, int i, OptionWidget *owner, QWidget *parent)
: QRadioButton(opt->m_options[i]->label(), parent)
, m_owner(owner)
, m_opt(opt)
, m_index(i) {
  setChecked(m_opt->m_options[i]->value());
  connect(this, SIGNAL(toggled(bool)), this, SLOT(setOpt(bool)));
}

void OptRadioButton::setOpt(bool b) {
  if(b) {
    m_opt->setSelected(m_index);
    m_owner->notifyChange();
  }
}

OptSpinBox::OptSpinBox(IntOptPtr opt, OptionWidget *owner, QWidget *parent)
: QSpinBox(parent)
, m_owner(owner)
, m_opt(opt) {
  setMinimum(m_opt->min());
  setMaximum(m_opt->max());
  setValue(m_opt->value());
  connect(this, SIGNAL(valueChanged(int)), this, SLOT(setOpt(int)));
}

void OptSpinBox::setOpt(int i) {
  m_opt->setValue(i);
  m_owner->notifyChange();
}

OptLineEdit::OptLineEdit(StringOptPtr opt, OptionWidget *owner, QWidget *parent)
: QLineEdit(parent)
, m_owner(owner)
, m_opt(opt) {
  setText(m_opt->value());
  connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setOpt(const QString&)));
}

void OptLineEdit::setOpt(const QString& s) {
  m_opt->setValue(s);
  m_owner->notifyChange();
}

OptUrlRequester::OptUrlRequester(UrlOptPtr opt, OptionWidget *owner, QWidget *parent)
: KUrlRequester(parent)
, m_owner(owner)
, m_opt(opt) {
  setUrl(m_opt->value());
  connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setOpt(const QString&)));
}

void OptUrlRequester::setOpt(const QString& s) {
  m_opt->setValue(s);
  m_owner->notifyChange();
}

OptComboBox::OptComboBox(ComboOptPtr opt, OptionWidget *owner, QWidget *parent)
: QComboBox(parent)
, m_owner(owner)
, m_opt(opt) {
  addItems(m_opt->m_values);
  setCurrentIndex(m_opt->selected());
  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(setOpt(int)));
}

void OptComboBox::setOpt(int c) {
  m_opt->setSelected(c);
  m_owner->notifyChange();
}

OptColorButton::OptColorButton(ColorOptPtr opt, OptionWidget *owner, QWidget *parent)
: KColorButton(parent)
, m_owner(owner)
, m_opt(opt) {
  setColor(m_opt->value());
  connect(this, SIGNAL(changed(const QColor&)), this, SLOT(setOpt(const QColor&)));
}

void OptColorButton::setOpt(const QColor& c) {
  m_opt->setValue(c);
  m_owner->notifyChange();
}

OptFontRequester::OptFontRequester(FontOptPtr opt, OptionWidget *owner, QWidget *parent)
: KFontRequester(parent)
, m_owner(owner)
, m_opt(opt) {
  setFont(m_opt->value());
  connect(this, SIGNAL(fontSelected(const QFont&)), this, SLOT(setOpt(const QFont&)));
}

void OptFontRequester::setOpt(const QFont& f) {
  m_opt->setValue(f);
  m_owner->notifyChange();
}
