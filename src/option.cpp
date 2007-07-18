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
#include "settings.h"
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

OptionWidget::OptionWidget(const OptList& options, QWidget* parent)
: QWidget(parent)
, m_dont_fire(false)
, m_options(options) {
  setupOptionWidget(this, m_options);
}

void OptionWidget::notifyChange() {
  if(m_dont_fire)
    m_changed = true;
  else {
    dump_options_list(m_options);
    emit changed(m_options);
  }
}

void OptionWidget::setValues(OptList& newopts) {
  m_changed = false;
  m_dont_fire = true;
  setOptionWidgetValues(this, newopts);
  m_dont_fire = false;
  if(m_changed)
    notifyChange();
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
      cb->setObjectName(qPrintf("%02d_check",i));
      layout->addWidget(cb, lpos++, left, 1, 2);
      if(o->m_sub_options.size()) {
        QWidget *w = new QWidget(widget);
        w->setObjectName(qPrintf("%02d_check_sub",i));
        setupOptionWidget(w, o->m_sub_options, true);
        w->setEnabled(o->value());
        QObject::connect(cb, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
        layout->addWidget(w, lpos++, left, 1, 2);
      }
    }
    else if(boost::shared_ptr<SelectOpt> o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      QGroupBox *gb = new QGroupBox(o->label(), widget);
      gb->setObjectName(qPrintf("%02d_group",i));
      QVBoxLayout *vbox = new QVBoxLayout(gb);
      //vbox->setMargin(0);
      for(int j=0;j<o->m_options.size();j++) {
        BoolOptPtr so = o->m_options[j];
        OptRadioButton *rb = new OptRadioButton(o, j, this, gb);
        rb->setObjectName(qPrintf("%02d_radio_%02d",i,j));
        vbox->addWidget(rb);
        if(so->m_sub_options.size()) {
          QWidget *w = new QWidget(widget);
          w->setObjectName(qPrintf("%02d_radio_%02d_sub",i,j));
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
      sb->setObjectName(qPrintf("%02d_spin",i));
      layout->addWidget(sb, lpos++, right);
    }
    else if(boost::shared_ptr<StringOpt> o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptLineEdit *ow = new OptLineEdit(o, this, widget);
      ow->setObjectName(qPrintf("%02d_line",i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<UrlOpt> o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptUrlRequester *ow = new OptUrlRequester(o, this, widget);
      ow->setObjectName(qPrintf("%02d_url",i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<ComboOpt> o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptComboBox *ow = new OptComboBox(o, this, widget);
      ow->setObjectName(qPrintf("%02d_combo",i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<ColorOpt> o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptColorButton *ow = new OptColorButton(o, this, widget);
      ow->setObjectName(qPrintf("%02d_color",i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(boost::shared_ptr<FontOpt> o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptFontRequester *ow = new OptFontRequester(o,this, widget);
      ow->setObjectName(qPrintf("%02d_font",i));
      layout->addWidget(ow, lpos++, right);
    }
    else
      ERROR("Unknown option of type " << prettyTypeName(typeid(*_o).name()));
  }

  if(indent)
    layout->addItem(new QSpacerItem(25, 1, QSizePolicy::Minimum, QSizePolicy::Minimum),
                                                                          0, 0, lpos, 1);
}

void OptionWidget::setOptionWidgetValues(QWidget* widget, OptList& newopts) {
  for(int i=0;i<newopts.size();i++) {
    OptPtr _o = newopts[i];
    if(boost::shared_ptr<BoolOpt> o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      OptCheckBox *cb = widget->findChild<OptCheckBox*>(qPrintf("%02d_check",i));
      if(!cb) goto fail;
      cb->setChecked(o->value());

      if(o->m_sub_options.size()) {
        QWidget *w = widget->findChild<QWidget*>(qPrintf("%02d_check_sub",i));
        if(!w) goto fail;
        setOptionWidgetValues(w, o->m_sub_options);
      }
    }
    else if(boost::shared_ptr<SelectOpt> o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      QGroupBox *gb = widget->findChild<QGroupBox*>(qPrintf("%02d_group",i));
      if(!gb) goto fail;

      for(int j=0;j<o->m_options.size();j++) {
        BoolOptPtr so = o->m_options[j];
        OptRadioButton *rb = gb->findChild<OptRadioButton*>(qPrintf("%02d_radio_%02d",i,j));
        if(!rb) goto fail;
        rb->setChecked(so->value());

        if(so->m_sub_options.size()) {
          QWidget *w = widget->findChild<QWidget*>(qPrintf("%02d_radio_%02d_sub",i,j));
          if(!w) goto fail;
          setOptionWidgetValues(w, so->m_sub_options);
        }
      }
    }
    else if(boost::shared_ptr<IntOpt> o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      OptSpinBox *sb = widget->findChild<OptSpinBox*>(qPrintf("%02d_spin",i));
      if(!sb) goto fail;
      sb->setValue(o->value());
    }
    else if(boost::shared_ptr<StringOpt> o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      OptLineEdit *ow = widget->findChild<OptLineEdit*>(qPrintf("%02d_line",i));
      if(!ow) goto fail;
      ow->setText(o->value());
    }
    else if(boost::shared_ptr<UrlOpt> o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      OptUrlRequester *ow = widget->findChild<OptUrlRequester*>(qPrintf("%02d_url",i));
      if(!ow) goto fail;
      ow->setUrl(o->value());
    }
    else if(boost::shared_ptr<ComboOpt> o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      OptComboBox *ow = widget->findChild<OptComboBox*>(qPrintf("%02d_combo",i));
      if(!ow) goto fail;
      ow->setCurrentIndex(o->m_selected);
    }
    else if(boost::shared_ptr<ColorOpt> o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      OptColorButton *ow = widget->findChild<OptColorButton*>(qPrintf("%02d_color",i));
      if(!ow) goto fail;
      ow->setColor(o->value());
    }
    else if(boost::shared_ptr<FontOpt> o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      OptFontRequester *ow = widget->findChild<OptFontRequester*>(qPrintf("%02d_font",i));
      if(!ow) goto fail;
      ow->setFont(o->value());
    }
    else
      ERROR("Unknown option of type " << prettyTypeName(typeid(*_o).name()));
  }
  return;
fail:
  ERROR("Options/Widget mismatch!");
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
    else {
      std::cout << "dump_options_list: Error, unknown option of type "
          << prettyTypeName(typeid(*_o).name()) << std::endl;
    }
  }
  if(!indent)std::cout << "---- end dump ----" << std::endl;
}

bool options_list_load_from_settings(OptList& options, const Settings& s) {
  bool retv = false;
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(boost::shared_ptr<BoolOpt> o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      Settings bool_group = s.group(o->name());
      retv |= options_list_load_from_settings(o->m_sub_options, bool_group);
      bool newval = bool_group.flag("value", o->value());
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(boost::shared_ptr<IntOpt> o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      int newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(boost::shared_ptr<StringOpt> o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      QString newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(boost::shared_ptr<UrlOpt> o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      QString newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(boost::shared_ptr<ColorOpt> o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      QColor newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(boost::shared_ptr<FontOpt> o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      QFont newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(boost::shared_ptr<ComboOpt> o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      int newval = s[o->name()] | o->selected();
      retv |= (newval != o->selected());
      o->setSelected( newval );
    }
    else if(boost::shared_ptr<SelectOpt> o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      OptList l;
      for(int i=0;i<o->m_options.size();i++)
        l <<  o->m_options[i];
      Settings sel_group = s.group(o->name());
      retv |= options_list_load_from_settings(l, sel_group.group("sel-options"));
      int newval = sel_group["value"] | o->selected();
      retv |= (newval != o->selected());
      o->setSelected( newval );
    }
    else
      ERROR("option of type " << prettyTypeName(typeid(*_o).name()) );
  }
  return retv;
}

void options_list_save_to_settings(const OptList& options, Settings s) {
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(boost::shared_ptr<BoolOpt> o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      Settings bool_group = s.group(o->name());
      options_list_save_to_settings(o->m_sub_options, bool_group);
      bool_group.setFlag("value", o->value());
    }
    else if(boost::shared_ptr<IntOpt> o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(boost::shared_ptr<StringOpt> o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(boost::shared_ptr<UrlOpt> o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(boost::shared_ptr<ColorOpt> o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(boost::shared_ptr<FontOpt> o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(boost::shared_ptr<ComboOpt> o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(boost::shared_ptr<SelectOpt> o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      OptList l;
      for(int i=0;i<o->m_options.size();i++)
        l <<  o->m_options[i];
      Settings sel_group = s.group(o->name());
      options_list_save_to_settings(l, sel_group.group("sel-options"));
      sel_group["value"] = o->value();
    }
    else
      ERROR("option of type " << prettyTypeName(typeid(*_o).name()) );
  }
}
