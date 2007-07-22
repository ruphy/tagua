/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef OPTION_H
#define OPTION_H

#include <list>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QFont>
#include <QColor>
#include <QApplication>
#include <QList>
#include <QStringList>
#include <QWidget>

class Settings;

class BaseOpt;
class BoolOpt;
class IntOpt;
class StringOpt;
class ColorOpt;
class FontOpt;
class ComboOpt;
class SelectOpt;
class UrlOpt;
typedef boost::shared_ptr<BaseOpt>   OptPtr;
typedef boost::shared_ptr<BoolOpt>   BoolOptPtr;
typedef boost::shared_ptr<IntOpt>    IntOptPtr;
typedef boost::shared_ptr<StringOpt> StringOptPtr;
typedef boost::shared_ptr<ColorOpt>  ColorOptPtr;
typedef boost::shared_ptr<FontOpt>   FontOptPtr;
typedef boost::shared_ptr<ComboOpt>  ComboOptPtr;
typedef boost::shared_ptr<SelectOpt> SelectOptPtr;
typedef boost::shared_ptr<UrlOpt>    UrlOptPtr;
typedef QList<OptPtr>     OptList;
typedef QList<BoolOptPtr> BoolOptList;
typedef boost::shared_ptr<OptList>     OptListPtr;
typedef boost::shared_ptr<BoolOptList> BoolOptListPtr;

template<typename O>
QList<boost::shared_ptr<O> > options_list_duplicate(const QList<boost::shared_ptr<O> >& o) {
  QList<boost::shared_ptr<O> > retv;
  for(int i=0;i<o.size();i++)
    retv << boost::static_pointer_cast<O,BaseOpt>(o[i]->clone());
  return retv;
}

template<typename O1, typename O2>
bool options_list_equals(const QList<boost::shared_ptr<O1> >& o1,
                          const QList<boost::shared_ptr<O2> >& o2) {
  if(o1.size() != o2.size())
    return false;
  for(int i=0;i<o1.size();i++)
  if(!o1[i]->equals(*o2[i]))
    return false;
  return true;
}

void dump_options_list(const OptList& options, int indent = 0);

class BaseOpt {
private:
  QString m_name;
  QString m_label;
public:
  BaseOpt(const QString& name, const QString& label)
    : m_name(name)
    , m_label(label) {}
  virtual ~BaseOpt() {};
  QString name() const { return m_name; }
  QString label() const { return m_label; }
  virtual OptPtr clone() const = 0;
  virtual bool equals(const BaseOpt&) const = 0;
};

class BoolOpt : public BaseOpt {
private:
  friend class OptionWidget;
  friend bool options_list_load_from_settings(OptList& options, const Settings& s);

  bool m_value;
  QList<OptPtr> m_sub_options;
public:
  typedef bool ValueType;
  BoolOpt(const QString& name, const QString& label, bool def = false,
            const QList<OptPtr>& suboptions = QList<OptPtr>())
    : BaseOpt(name, label)
    , m_value(def)
    , m_sub_options(suboptions) {}
  bool value() const { return m_value; }
  void setValue(bool v) { m_value = v; }
  OptList subOptions() { return m_sub_options; }
  virtual OptPtr clone() const {
    BoolOpt *o = new BoolOpt(name(), label(), m_value, options_list_duplicate(m_sub_options) );
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const BoolOpt* o = dynamic_cast<const BoolOpt*>(&_o);
    return o
        && m_value != o->m_value
        && options_list_equals(m_sub_options, o->m_sub_options);
  }
};

class IntOpt : public BaseOpt {
public:
  enum Visualization {
    SpinBox,
    Slider
  };
private:
  int m_min;
  int m_max;
  int m_value;
  Visualization m_visualization;
public:
  typedef int ValueType;
  IntOpt(const QString& name, const QString& label, int def, int min, int max, Visualization visualization)
    : BaseOpt(name, label)
    , m_min(min)
    , m_max(max)
    , m_value(def)
    , m_visualization(visualization) {}
  int min() const { return m_min; }
  int max() const { return m_max; }
  int value() const { return m_value; }
  Visualization visualization() const { return m_visualization; }
  void setValue(int v) { m_value = v; }
  virtual OptPtr clone() const {
    IntOpt *o = new IntOpt(name(), label(), m_value, m_min, m_max, m_visualization);
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const IntOpt* o = dynamic_cast<const IntOpt*>(&_o);
    return o
        && m_min != o->m_min
        && m_max != o->m_max
        && m_value != o->m_value
        && m_visualization != o->m_visualization;
  }
};

class StringOpt : public BaseOpt {
private:
  QString m_value;
public:
  typedef QString ValueType;
  StringOpt(const QString& name, const QString& label, QString def = QString())
    : BaseOpt(name, label)
    , m_value(def) {}
  QString value() const { return m_value; }
  void setValue(QString v) { m_value = v; }
  virtual OptPtr clone() const {
    StringOpt *o = new StringOpt(name(), label(), m_value);
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const StringOpt* o = dynamic_cast<const StringOpt*>(&_o);
    return o
        && m_value != o->m_value;
  }
};

class UrlOpt : public BaseOpt {
private:
  QString m_value;
public:
  typedef QString ValueType;
  UrlOpt(const QString& name, const QString& label, QString def = QString() )
    : BaseOpt(name, label)
    , m_value(def) {}
  QString value() const { return m_value; }
  void setValue(QString v) { m_value = v; }
  virtual OptPtr clone() const {
    UrlOpt *o = new UrlOpt(name(), label(), m_value);
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const UrlOpt* o = dynamic_cast<const UrlOpt*>(&_o);
    return o
        && m_value != o->m_value;
  }
};

class ColorOpt : public BaseOpt {
private:
  QColor m_value;
public:
  typedef QColor ValueType;
  ColorOpt(const QString& name, const QString& label, QColor def = Qt::black)
    : BaseOpt(name, label)
    , m_value(def) {}
  QColor value() const { return m_value; }
  void setValue(QColor v) { m_value = v; }
  virtual OptPtr clone() const {
    ColorOpt *o = new ColorOpt(name(), label(), m_value);
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const ColorOpt* o = dynamic_cast<const ColorOpt*>(&_o);
    return o
        && m_value != o->m_value;
  }
};

class FontOpt : public BaseOpt {
private:
  QFont m_value;
public:
  typedef QFont ValueType;
  FontOpt(const QString& name, const QString& label, QFont def = QApplication::font())
    : BaseOpt(name, label)
    , m_value(def) {}
  QFont value() const { return m_value; }
  void setValue(QFont v) { m_value = v; }
  virtual OptPtr clone() const {
    FontOpt *o = new FontOpt(name(), label(), m_value);
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const FontOpt* o = dynamic_cast<const FontOpt*>(&_o);
    return o
        && m_value != o->m_value;
  }
};

class ComboOpt : public BaseOpt {
private:
  QStringList m_values;
  int         m_selected;
public:
  typedef int ValueType;
  ComboOpt(const QString& name, const QString& label, const QStringList& values, int selected = 0)
    : BaseOpt(name, label)
    , m_values(values)
    , m_selected(selected) {}
  int value() const { return m_selected; }
  int selected() const { return m_selected; }
  void setSelected(int v) { m_selected = v; }
  QStringList values() const { return m_values; }
  virtual OptPtr clone() const {
    ComboOpt *o = new ComboOpt(name(), label(), m_values, m_selected);
    return OptPtr(o);
  }
  virtual bool equals(const BaseOpt& _o) const {
    const ComboOpt* o = dynamic_cast<const ComboOpt*>(&_o);
    return o
        && m_values == o->m_values
        && m_selected != o->m_selected;
  }
};

class SelectOpt : public BaseOpt {
private:
  friend class OptionWidget;
  friend bool options_list_load_from_settings(OptList& options, const Settings& s);

  QList<BoolOptPtr> m_options;
  int m_selected;

public:
  typedef int ValueType;
  SelectOpt(const QString& name, const QString& label, QList<BoolOptPtr> options,
                                                                  int selected = 0)
    : BaseOpt(name, label)
    , m_options(options) {
    setSelected(selected);
  }
  int value() const { return m_selected; }
  int selected() const { return m_selected; }
  void setSelected(int s) {
    m_selected = std::min(std::max(s, 0), m_options.size()-1);
    for(int i=0;i<m_options.size();i++)
      m_options[i]->setValue(i==m_selected);
  }
  BoolOptList options(){ return m_options; }
  virtual OptPtr clone() const {
    return OptPtr(new SelectOpt(name(), label(), options_list_duplicate(m_options), m_selected));
  }
  virtual bool equals(const BaseOpt& _o) const {
    const SelectOpt* o = dynamic_cast<const SelectOpt*>(&_o);
    return o
        && m_selected != o->m_selected
        && options_list_equals(m_options, o->m_options);
  }
};



template<typename O>
boost::shared_ptr<O> options_list_find(const OptList& o, const QString& name) {
  for(int i=0;i<o.size();i++)
  if(o[i]->name() == name)
    if(boost::shared_ptr<O> retv = boost::dynamic_pointer_cast<O, BaseOpt>(o[i]))
      return retv;
  return boost::shared_ptr<O>();
}

template<typename O>
typename O::ValueType options_list_find(const OptList& o, const QString& name,
                                              const typename O::ValueType& def) {
  for(int i=0;i<o.size();i++)
  if(o[i]->name() == name)
    if(boost::shared_ptr<O> retv = boost::dynamic_pointer_cast<O, BaseOpt>(o[i]))
      return retv->value();
  return def;
}

bool options_list_load_from_settings(OptList&, const Settings& s);
void options_list_save_to_settings(const OptList&, Settings s);


class OptionWidget : public QWidget {
  Q_OBJECT
  bool m_changed;
  bool m_dont_fire;
  QList<OptPtr> m_options;
  void setupOptionWidget(QWidget*, OptList&, bool indent = false);
  void setOptionWidgetValues(QWidget*, OptList&);
  void notifyChange();

  friend class OptCheckBox;
  friend class OptRadioButton;
  friend class OptSpinBox;
  friend class OptSlider;
  friend class OptLineEdit;
  friend class OptColorButton;
  friend class OptFontRequester;
  friend class OptUrlRequester;
  friend class OptComboBox;

public:
  OptionWidget(const OptList& options, QWidget* parent = NULL);
  void setValues(OptList& newopts);

signals:
  void changed(const OptList& options);
};

#endif //OPTION_H
