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
#include <QDir>
#include "option.h"
#include "ui/ui_pref_theme.h"

class VariantInfo;
class Settings;

class PrefTheme : public QWidget
                , private Ui::PrefTheme {
Q_OBJECT

  class ThemeInfo;
  typedef QList<ThemeInfo> ThemeInfoList;

  ThemeInfoList m_pieces_themes;
  ThemeInfoList m_squares_themes;
  std::map<QString, boost::shared_ptr<OptList> > m_new_theme_options;
  std::map<QString, QString> m_new_piece_themes;
  std::map<QString, QString> m_new_square_themes;
  std::map<QString, bool> m_new_use_def_pieces;
  std::map<QString, bool> m_new_use_def_squares;

  QLayout *m_pieces_opt_layout;
  QLayout *m_squares_opt_layout;
  QWidget *m_pieces_opt_widget;
  QWidget *m_squares_opt_widget;

  OptList              get_file_options(const QString&);
  static QStringList   find_lua_files(const QDir& d);
  static ThemeInfoList to_theme_info_list(const QStringList& files, Settings& s);
  static void          update_list_view(QListWidget* list, const ThemeInfoList& themes,
                                    QString variant, QString settings);

private slots:
  void variantChanged();
  void piecesThemeChanged();
  void squaresThemeChanged();
  void piecesThemeChecked(bool ck);
  void squaresThemeChecked(bool ck);

public:
  PrefTheme(QWidget *parent = 0);
  ~PrefTheme();

  static QString getBestTheme(VariantInfo* vi, bool squares = false);

  void apply();
};

#endif //PREFERENCES__THEME_H
