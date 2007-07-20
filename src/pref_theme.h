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
#include "ui_pref_theme_page.h"

class VariantInfo;
class Settings;
class QAction;

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
  typedef std::map<QString, Category*> CategoryMap;

  std::map<QString, boost::shared_ptr<OptList> > m_new_theme_options;
  CategoryMap m_categories;

  OptList       get_file_options(const QString&, bool reload_defaults = false);
  static ThemeInfoList to_theme_info_list(const QStringList& files, const Settings& s);
  static void          update_list_view(QListWidget* list, const ThemeInfoList& themes,
                                    QString variant, QString settings);
  static void read_theme_info(PrefTheme::ThemeInfo& info, const QString& desktopFile);
private slots:
  void variantChanged();

public:
  PrefTheme(QWidget *parent = 0);
  ~PrefTheme();

  void apply();

  static QString getBestTheme(VariantInfo* vi, const QString&);
};


class PrefThemeCategory : public QWidget
                        , private Ui::PrefThemePage {
Q_OBJECT

public:
  friend class PrefTheme;
  QAction*   m_reset;
  PrefTheme* m_parent;
  QString    m_id;
  PrefTheme::ThemeInfoList m_themes;
  std::map<QString, QString> m_new_themes;
  std::map<QString, bool> m_new_use_def;

  QLayout *m_opt_layout;
  QWidget *m_opt_widget;

  PrefThemeCategory(QWidget* parent, PrefTheme* owner);

public slots:
  void reset();
  void themeChanged();
  void themeChecked(bool ck);
};


#endif //PREFERENCES__THEME_H
