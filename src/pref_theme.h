/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREFERENCES__THEME_H
#define PREFERENCES__THEME_H

#include <set>
#include <map>
#include <boost/shared_ptr.hpp>
#include <QDateTime>
#include <QDir>
#include "option.h"
#include "ui_pref_theme.h"

class VariantInfo;
class Settings;


class PrefTheme : public QWidget
                , private Ui::PrefTheme {
Q_OBJECT
  friend class PrefThemeCategory;
  typedef class PrefThemeCategory Category;

  class ThemeInfo {
  public:
    QString file_name;
    QString name;
    QString description;
    QStringList variants;
    QDateTime last_modified;

    ThemeInfo() {}
    ThemeInfo(const QString& f, const QString& n,
                  const QString& d, const QStringList& v, const QDateTime& t)
    : file_name(f)
    , name(n)
    , description(d)
    , variants(v)
    , last_modified(t) {}
  };

  typedef QList<ThemeInfo> ThemeInfoList;
  typedef std::map<QString, class PrefThemeCategory> CategoryMap;

  std::map<QString, boost::shared_ptr<OptList> > m_new_theme_options;
  CategoryMap m_categories;

  OptList              get_file_options(const QString&);
  static ThemeInfoList to_theme_info_list(const QStringList& files, const Settings& s);
  static void          update_list_view(QListWidget* list, const ThemeInfoList& themes,
                                    QString variant, QString settings);

private slots:
  void variantChanged();

public:

  PrefTheme(QWidget *parent = 0);
  ~PrefTheme();

  enum ThemeType {
    Pieces,
    Squares,
    Figurines
  };

  static QString getBestTheme(VariantInfo* vi, ThemeType type = Pieces);

  void apply();
};


class PrefThemeCategory : public QObject {
Q_OBJECT

public:
  friend class PrefTheme;
  PrefTheme* m_parent;
  QString    m_id;
  PrefTheme::ThemeInfoList m_themes;
  std::map<QString, QString> m_new_themes;
  std::map<QString, bool> m_new_use_def;

  QLayout *m_opt_layout;
  QWidget *m_opt_widget;

  QListWidget *m_list;
  QWidget *m_widget;
  QLabel *m_label;
  QCheckBox *m_check;

  PrefThemeCategory()
    : QObject()
    , m_parent(NULL)
    , m_opt_layout(NULL)
    , m_opt_widget(NULL) {
  }
  PrefThemeCategory(const PrefThemeCategory& k)
    : QObject()
    , m_parent(k.m_parent)
    , m_themes(k.m_themes)
    , m_new_themes(k.m_new_themes)
    , m_new_use_def(k.m_new_use_def)
    , m_opt_layout(k.m_opt_layout)
    , m_opt_widget(k.m_opt_widget)
    , m_list(k.m_list)
    , m_widget(k.m_widget)
    , m_label(k.m_label)
    , m_check(k.m_check) {
  }

public slots:
  void themeChanged();
  void themeChecked(bool ck);
};


#endif //PREFERENCES__THEME_H
