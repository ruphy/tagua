/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <KDebug>
#include "settings.h"
#include "common.h"
#include "option.h"
#include "option_p.h"


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
    changed(m_options);
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
    if(BoolOptPtr o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      OptCheckBox *cb = new OptCheckBox(o, this, widget);
      cb->setObjectName(QString("%02d_check").arg(i));
      layout->addWidget(cb, lpos++, left, 1, 2);
      if(o->m_sub_options.size()) {
        QWidget *w = new QWidget(widget);
        w->setObjectName(QString("%02d_check_sub").arg(i));
        setupOptionWidget(w, o->m_sub_options, true);
        w->setEnabled(o->value());
        QObject::connect(cb, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
        layout->addWidget(w, lpos++, left, 1, 2);
      }
    }
    else if(SelectOptPtr o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      QGroupBox *gb = new QGroupBox(o->label(), widget);
      gb->setObjectName(QString("%02d_group").arg(i));
      QVBoxLayout *vbox = new QVBoxLayout(gb);
      //vbox->setMargin(0);
      for(int j=0;j<o->m_options.size();j++) {
        BoolOptPtr so = o->m_options[j];
        OptRadioButton *rb = new OptRadioButton(o, j, this, gb);
        rb->setObjectName(QString("%02d_radio_%02d").arg(i).arg(j));
        vbox->addWidget(rb);
        if(so->m_sub_options.size()) {
          QWidget *w = new QWidget(widget);
          w->setObjectName(QString("%02d_radio_%02d_sub").arg(i).arg(j));
          setupOptionWidget(w, so->m_sub_options, true);
          w->setEnabled(so->value());
          QObject::connect(rb, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
          vbox->addWidget(w);
        }
      }
      layout->addWidget(gb, lpos++, left, 1, 2);
    }
    else if (IntOptPtr o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      switch (o->visualization()) {
      case IntOpt::SpinBox:
        {
          OptSpinBox *sb = new OptSpinBox(o, this, widget);
          sb->setObjectName(QString("%02d_spin").arg(i));
          layout->addWidget(sb, lpos++, right);
          break;
        }
      case IntOpt::Slider:
        {
          OptSlider *sl = new OptSlider(o, this, widget);
          sl->setObjectName(QString("%02d_slid").arg(i));
          layout->addWidget(sl, lpos++, right);
          break;
        }
      }
    }
    else if(StringOptPtr o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptLineEdit *ow = new OptLineEdit(o, this, widget);
      ow->setObjectName(QString("%02d_line").arg(i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(UrlOptPtr o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptUrlRequester *ow = new OptUrlRequester(o, this, widget);
      ow->setObjectName(QString("%02d_url").arg(i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(ComboOptPtr o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptComboBox *ow = new OptComboBox(o, this, widget);
      ow->setObjectName(QString("%02d_combo").arg(i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(ColorOptPtr o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptColorButton *ow = new OptColorButton(o, this, widget);
      ow->setObjectName(QString("%02d_color").arg(i));
      layout->addWidget(ow, lpos++, right);
    }
    else if(FontOptPtr o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      layout->addWidget(new QLabel(o->label()), lpos, left);
      OptFontRequester *ow = new OptFontRequester(o,this, widget);
      ow->setObjectName(QString("%02d_font").arg(i));
      layout->addWidget(ow, lpos++, right);
    }
    else
      kError() << "Unknown option of type" << prettyTypeName(typeid(*_o).name());
  }

  if(indent)
    layout->addItem(new QSpacerItem(25, 1, QSizePolicy::Minimum, QSizePolicy::Minimum),
                                                                          0, 0, lpos, 1);
}

void OptionWidget::setOptionWidgetValues(QWidget* widget, OptList& newopts) {
  for(int i=0;i<newopts.size();i++) {
    OptPtr _o = newopts[i];
    if(BoolOptPtr o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      OptCheckBox *cb = widget->findChild<OptCheckBox*>(QString("%02d_check").arg(i));
      if(!cb) goto fail;
      cb->setChecked(o->value());

      if(o->m_sub_options.size()) {
        QWidget *w = widget->findChild<QWidget*>(QString("%02d_check_sub").arg(i));
        if(!w) goto fail;
        setOptionWidgetValues(w, o->m_sub_options);
      }
    }
    else if(SelectOptPtr o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      QGroupBox *gb = widget->findChild<QGroupBox*>(QString("%02d_group").arg(i));
      if(!gb) goto fail;

      for(int j=0;j<o->m_options.size();j++) {
        BoolOptPtr so = o->m_options[j];
        OptRadioButton *rb = gb->findChild<OptRadioButton*>(QString("%02d_radio_%02d").arg(i).arg(j));
        if(!rb) goto fail;
        rb->setChecked(so->value());

        if(so->m_sub_options.size()) {
          QWidget *w = widget->findChild<QWidget*>(QString("%02d_radio_%02d_sub").arg(i).arg(j));
          if(!w) goto fail;
          setOptionWidgetValues(w, so->m_sub_options);
        }
      }
    }
    else if(IntOptPtr o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      switch (o->visualization()) {
      case IntOpt::SpinBox:
        {
          OptSpinBox *sb = widget->findChild<OptSpinBox*>(QString("%02d_spin").arg(i));
          if(!sb) goto fail;
          sb->setValue(o->value());
          break;
        }
      case IntOpt::Slider:
        {
          OptSlider *sl = widget->findChild<OptSlider*>(QString("%02d_slid").arg(i));
          if(!sl) goto fail;
          sl->setValue(o->value());
          break;
        }
      }
    }
    else if(StringOptPtr o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      OptLineEdit *ow = widget->findChild<OptLineEdit*>(QString("%02d_line").arg(i));
      if(!ow) goto fail;
      ow->setText(o->value());
    }
    else if(UrlOptPtr o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      OptUrlRequester *ow = widget->findChild<OptUrlRequester*>(QString("%02d_url").arg(i));
      if(!ow) goto fail;
      ow->setUrl(o->value());
    }
    else if(ComboOptPtr o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      OptComboBox *ow = widget->findChild<OptComboBox*>(QString("%02d_combo").arg(i));
      if(!ow) goto fail;
      ow->setCurrentIndex(o->selected());
    }
    else if(ColorOptPtr o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      OptColorButton *ow = widget->findChild<OptColorButton*>(QString("%02d_color").arg(i));
      if(!ow) goto fail;
      ow->setColor(o->value());
    }
    else if(FontOptPtr o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      OptFontRequester *ow = widget->findChild<OptFontRequester*>(QString("%02d_font").arg(i));
      if(!ow) goto fail;
      ow->setFont(o->value());
    }
    else
      kError() << "Unknown option of type" << prettyTypeName(typeid(*_o).name());
  }
  return;
fail:
  kError() << "Options/Widget mismatch!";
}

void dump_options_list(const OptList& options, int indent) {
  if(!indent)kDebug() << "---- begin dump ----";
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(BoolOptPtr o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << (o->value()?"[X]":"[ ]") << " " << o->label();
      dump_options_list(o->subOptions(), indent+1);
    }
    else if(IntOptPtr o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << o->label() << " |" << o->value() << "|";
    }
    else if(StringOptPtr o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << o->label() << " [" << o->value() << "]";
    }
    else if(UrlOptPtr o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << o->label() << " {" << o->value() << "}";
    }
    else if(ColorOptPtr o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << o->label() << " {" << o->value().name() << "}";
    }
    else if(FontOptPtr o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << o->label() << " {" << o->value().toString() << "}";
    }
    else if(ComboOptPtr o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << "[combo] " << o->label();
      for(int k=0;k<o->values().size();k++) {
        for(int j=0;j<indent+1;j++)kDebug() << "  ";
        if(k==o->selected())
          kDebug() << " *<" << o->values()[k] << ">*";
        else
          kDebug() << "  <" << o->values()[k] << ">";
      }
    }
    else if(SelectOptPtr o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      for(int j=0;j<indent;j++)kDebug() << "  ";
      kDebug() << "[group] " << o->label();
      for(int j=0;j<o->options().size();j++) {
        BoolOptPtr so = o->options()[j];
        for(int j=0;j<indent+1;j++)kDebug() << "  ";
        kDebug() << (so->value()?"(*)":"( )") << " " << so->label();
        if(so->subOptions().size())
          dump_options_list(so->subOptions(), indent+2);
      }
    }
    else {
      kDebug() << "dump_options_list: Error, unknown option of type "
          << prettyTypeName(typeid(*_o).name());
    }
  }
  if(!indent)kDebug() << "---- end dump ----";
}

bool options_list_load_from_settings(OptList& options, const Settings& s) {
  bool retv = false;
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(BoolOptPtr o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      Settings bool_group = s.group(o->name());
      retv |= options_list_load_from_settings(o->m_sub_options, bool_group);
      bool newval = bool_group.flag("value", o->value());
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(IntOptPtr o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      int newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(StringOptPtr o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      QString newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(UrlOptPtr o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      QString newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(ColorOptPtr o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      QColor newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(FontOptPtr o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      QFont newval = s[o->name()] | o->value();
      retv |= (newval != o->value());
      o->setValue( newval );
    }
    else if(ComboOptPtr o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      int newval = s[o->name()] | o->selected();
      retv |= (newval != o->selected());
      o->setSelected( newval );
    }
    else if(SelectOptPtr o =
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
      kError() << "option of type" << prettyTypeName(typeid(*_o).name());
  }
  return retv;
}

void options_list_save_to_settings(const OptList& options, Settings s) {
  for(int i=0;i<options.size();i++) {
    OptPtr _o = options[i];
    if(BoolOptPtr o =
            boost::dynamic_pointer_cast<BoolOpt,BaseOpt>(_o)) {
      Settings bool_group = s.group(o->name());
      options_list_save_to_settings(o->subOptions(), bool_group);
      bool_group.setFlag("value", o->value());
    }
    else if(IntOptPtr o =
            boost::dynamic_pointer_cast<IntOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(StringOptPtr o =
            boost::dynamic_pointer_cast<StringOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(UrlOptPtr o =
            boost::dynamic_pointer_cast<UrlOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(ColorOptPtr o =
            boost::dynamic_pointer_cast<ColorOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(FontOptPtr o =
            boost::dynamic_pointer_cast<FontOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(ComboOptPtr o =
            boost::dynamic_pointer_cast<ComboOpt,BaseOpt>(_o)) {
      s[o->name()] = o->value();
    }
    else if(SelectOptPtr o =
            boost::dynamic_pointer_cast<SelectOpt,BaseOpt>(_o)) {
      OptList l;
      for(int i=0;i<o->options().size();i++)
        l <<  o->options()[i];
      Settings sel_group = s.group(o->name());
      options_list_save_to_settings(l, sel_group.group("sel-options"));
      sel_group["value"] = o->value();
    }
    else
      kError() << "option of type" << prettyTypeName(typeid(*_o).name());
  }
}
