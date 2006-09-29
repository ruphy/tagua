/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "common.h"
#include "option.h"
#include "option_p.h"

OptCheckBox::OptCheckBox(boost::shared_ptr<BoolOpt> opt, OptionWidget *owner, QWidget *parent)
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

OptRadioButton::OptRadioButton(boost::shared_ptr<SelectOpt> opt, int i, OptionWidget *owner, QWidget *parent)
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

OptSpinBox::OptSpinBox(boost::shared_ptr<IntOpt> opt, OptionWidget *owner, QWidget *parent)
: QSpinBox(parent)
, m_owner(owner)
, m_opt(opt) {
  setValue(m_opt->value());
  setMinimum(m_opt->min());
  setMaximum(m_opt->max());
  connect(this, SIGNAL(valueChanged(int)), this, SLOT(setOpt(int)));
}

void OptSpinBox::setOpt(int i) {
  m_opt->setValue(i);
  m_owner->notifyChange();
}

OptLineEdit::OptLineEdit(boost::shared_ptr<StringOpt> opt, OptionWidget *owner, QWidget *parent)
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

OptUrlRequester::OptUrlRequester(boost::shared_ptr<UrlOpt> opt, OptionWidget *owner, QWidget *parent)
: UrlRequester(parent)
, m_owner(owner)
, m_opt(opt) {
  setUrl(m_opt->value());
  connect(this, SIGNAL(urlSelected(const QString&)), this, SLOT(setOpt(const QString&)));
}

void OptUrlRequester::setOpt(const QString& s) {
  m_opt->setValue(s);
  m_owner->notifyChange();
}

OptComboBox::OptComboBox(boost::shared_ptr<ComboOpt> opt, OptionWidget *owner, QWidget *parent)
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

OptColorButton::OptColorButton(boost::shared_ptr<ColorOpt> opt, OptionWidget *owner, QWidget *parent)
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

OptFontRequester::OptFontRequester(boost::shared_ptr<FontOpt> opt, OptionWidget *owner, QWidget *parent)
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

OptionWidget::OptionWidget(OptList& options, QWidget* parent)
: QWidget(parent)
, m_options(options) {
  setupOptionWidget(this, m_options);
}

void OptionWidget::notifyChange() {
  dump_options_list(m_options);
  emit changed(m_options);
}

void OptionWidget::setupOptionWidget(QWidget* widget, OptList& options, bool indent) {
  QGridLayout *layout = new QGridLayout(widget);

  if(indent)
    layout->setMargin(0);

  int left = indent ? 1 : 0;
  int right = indent ? 2 : 1;
  int lpos = 0;
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(boost::shared_ptr<BoolOpt> o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      OptCheckBox *cb = new OptCheckBox(o, this, widget);
      layout->addWidget(cb, lpos++, left, 1, 2);
      if(o->m_sub_options.size()) {
        QWidget *w = new QWidget(widget);
        setupOptionWidget(w, o->m_sub_options, true);
        w->setEnabled(o->value());
        QObject::connect(cb, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
        layout->addWidget(w, lpos++, left, 1, 2);
      }
    }
    else if(boost::shared_ptr<SelectOpt> o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      QGroupBox *gb = new QGroupBox(o->label(), widget);
      QVBoxLayout *vbox = new QVBoxLayout(gb);
      //vbox->setMargin(0);
      for(int j=0;j<o->m_options.size();j++) {
        BoolOptPtr so = o->m_options[j];
        OptRadioButton *rb = new OptRadioButton(o, j, this, gb);
        vbox->addWidget(rb);
        if(so->m_sub_options.size()) {
          QWidget *w = new QWidget(widget);
          setupOptionWidget(w, so->m_sub_options, true);
          w->setEnabled(so->value());
          QObject::connect(rb, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
          vbox->addWidget(w);
        }
      }
      layout->addWidget(gb, lpos++, left, 1, 2);
    }
    else if(boost::shared_ptr<IntOpt> o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptSpinBox *sb = new OptSpinBox(o, this, widget);
      layout->addWidget(sb, lpos++, right);
    }
    else if(boost::shared_ptr<StringOpt> o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptLineEdit *ow = new OptLineEdit(o, this, widget);
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<UrlOpt> o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptUrlRequester *ow = new OptUrlRequester(o, this, widget);
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<ComboOpt> o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptComboBox *ow = new OptComboBox(o, this, widget);
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<ColorOpt> o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptColorButton *ow = new OptColorButton(o, this, widget);
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<FontOpt> o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptFontRequester *ow = new OptFontRequester(o,this, widget);
      layout->addWidget(ow, lpos++, right);
    }
    else {

    }
  }

  if(indent)
    layout->addItem(new QSpacerItem(25, 1, QSizePolicy::Minimum, QSizePolicy::Minimum),
                                                                          0, 0, lpos, 1);
}

void dump_options_list(OptList& options, int indent) {
  if(!indent)std::cout << "---- begin dump ----" << std::endl;
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(boost::shared_ptr<BoolOpt> o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << (o->value()?"[X]":"[ ]") << " " << o->label() << std::endl;
      dump_options_list(o->m_sub_options, indent+1);
    }
    else if(boost::shared_ptr<SelectOpt> o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << "[group] " << o->label() << std::endl;
      for(int j=0;j<o->m_options.size();j++) {
        BoolOptPtr so = o->m_options[j];
        for(int j=0;j<indent+1;j++)std::cout << "  ";
        std::cout << (so->value()?"(*)":"( )") << " " << so->label() << std::endl;
        if(so->m_sub_options.size())
          dump_options_list(so->m_sub_options, indent+2);
      }
    }
    else if(boost::shared_ptr<IntOpt> o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << o->label() << " |" << o->value() << "|" << std::endl;
    }
    else if(boost::shared_ptr<StringOpt> o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << o->label() << " [" << o->value() << "]" << std::endl;
    }
    else if(boost::shared_ptr<UrlOpt> o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << o->label() << " {" << o->value() << "}" << std::endl;
    }
    else if(boost::shared_ptr<ComboOpt> o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << "[combo] " << o->label() << std::endl;
      for(int k=0;k<o->m_values.size();k++) {
        for(int j=0;j<indent+1;j++)std::cout << "  ";
        if(k==o->selected())
          std::cout << " *<" << o->m_values[k] << ">*" << std::endl;
        else
          std::cout << "  <" << o->m_values[k] << ">" << std::endl;
      }
    }
    else if(boost::shared_ptr<ColorOpt> o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << o->label() << " {" << o->value().name() << "}" << std::endl;
    }
    else if(boost::shared_ptr<FontOpt> o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)std::cout << "  ";
      std::cout << o->label() << " {" << o->value().toString() << "}" << std::endl;
    }
    else {

    }
  }
  if(!indent)std::cout << "---- end dump ----" << std::endl;
}

#include "option.moc"
#include "option_p.moc"
