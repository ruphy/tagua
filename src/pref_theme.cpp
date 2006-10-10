/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QListWidgetItem>
#include <kstandarddirs.h>
#include "global.h"
#include "luaapi/loader.h"
#include "variants/variants.h"
#include "kboard.h"
#include "pref_theme.h"

class PrefTheme::ThemeInfo {
public:
  QString file_name;
  QString name;
  QString description;
  QStringList variants;
  QDateTime last_modified;

  ThemeInfo(const QString& f, const QString& n,
                const QString& d, const QStringList& v, const QDateTime& t)
  : file_name(f)
  , name(n)
  , description(d)
  , variants(v)
  , last_modified(t) {}
};

PrefTheme::ThemeInfoList PrefTheme::to_theme_info_list(const QStringList& files, const Settings& s) {
  std::map<QString, ThemeInfo> cache;

  SettingArray themes = s.group("themes").array("theme");
  foreach (Settings s_theme, themes) {
    QStringList variants;
    SettingArray s_variants = s_theme.group("variants").array("variant");
    foreach (Settings s_var, s_variants) {
      variants.append(s_var["name"].value<QString>());
    }
    QString f = s_theme["file-name"].value<QString>();
    cache.insert(std::make_pair(f, ThemeInfo(f,
                 s_theme["name"].value<QString>(),
                 s_theme["description"].value<QString>(),
                 variants,
                 QDateTime::fromString(s_theme["last-modified"].value<QString>()))));
  }

  ThemeInfoList allluafiles;
  ThemeInfoList retv;
  bool updated = false;
  for(int i=0;i<files.size();i++) {
    QDateTime lm = QFileInfo(files[i]).lastModified();
    std::map<QString, ThemeInfo>::iterator it = cache.find(files[i]);

    if(it != cache.end() && it->second.last_modified == lm) {
      if(!it->second.name.isEmpty())
        retv << it->second;
    }
    else {
      updated = true;
      LuaApi::Loader l(NULL);
      l.runFile(files[i]);

      if(l.error()) {
        allluafiles << ThemeInfo(files[i], QString(), QString(), QStringList(), lm );
        std::cout << " --> Error loading " << files[i] << std::endl
              << l.errorString() << std::endl;
        continue;
      }

      QString name = l.getString("name");
      if(name.isEmpty()) {
        allluafiles << ThemeInfo(files[i], QString(), QString(), QStringList(), lm );
        continue;
      }

      ThemeInfo t(files[i], name, l.getString("description"), l.getStringList("variants"), lm);
      retv << t;
      allluafiles << t;
    }
  }

  if(updated) {
    SettingArray themes = s.group("themes").newArray("theme");

    for (int i = 0; i < allluafiles.size(); i++) {
      Settings s_theme = themes.append();
      s_theme["file-name"]     = allluafiles[i].file_name;
      s_theme["name"]          = allluafiles[i].name;
      s_theme["description"]   = allluafiles[i].description;
      {
        SettingArray variants = s_theme.group("variants").newArray("variant");
        foreach (QString v, allluafiles[i].variants) {
          Settings s_var = variants.append();
          s_var["name"] = v;
        }
      }
      s_theme["last-modified"] = allluafiles[i].last_modified.toString();
    }
  }

  return retv;
}

OptList PrefTheme::get_file_options(const QString& f) {
  if(boost::shared_ptr<OptList> o = m_new_theme_options[f])
    return *o;

  LuaApi::Loader l(NULL);
  l.runFile(f);

  boost::shared_ptr<OptList> o = boost::shared_ptr<OptList>(new OptList(l.getOptList("options")));
  SettingMap<QString> s_lua = settings.group("lua-settings").map<QString>("entry", "file-name");
  Settings entry = s_lua.insert(f);
  options_list_load_from_settings(*o, entry.group("options"));

  m_new_theme_options[f] = o;
  return *o;
}

PrefTheme::PrefTheme(QWidget *parent)
: QWidget(parent)
, m_pieces_opt_widget(NULL)
, m_squares_opt_widget(NULL) {
  setupUi(this);
  listPieces->setSelectionMode(QAbstractItemView::SingleSelection);
  listSquares->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(listPieces, SIGNAL(itemSelectionChanged()), this, SLOT(piecesThemeChanged()));
  connect(listSquares, SIGNAL(itemSelectionChanged()), this, SLOT(squaresThemeChanged()));
  connect(checkPieces, SIGNAL(toggled(bool)), this, SLOT(piecesThemeChecked(bool)));
  connect(checkSquares, SIGNAL(toggled(bool)), this, SLOT(squaresThemeChecked(bool)));
  connect(comboVariant, SIGNAL(currentIndexChanged(int)), this, SLOT(variantChanged()));
  m_pieces_opt_layout = new QHBoxLayout(widgetPieces);
  m_pieces_opt_layout->setMargin(0);
  m_squares_opt_layout = new QHBoxLayout(widgetSquares);
  m_squares_opt_layout->setMargin(0);

  MasterSettings s(".kboard_config_cache");

  KStandardDirs* dirs = KGlobal::dirs();
  m_pieces_themes = to_theme_info_list(
    dirs->findAllResources("appdata", "themes/Pieces/*.lua", true),
    s.group("pieces"));
  m_squares_themes = to_theme_info_list(
    dirs->findAllResources("appdata", "themes/Squares/*.lua", true),
    s.group("squares"));

  const Variant::Variants& all = Variant::allVariants();
  for(Variant::Variants::const_iterator it = all.begin(); it != all.end(); ++it)
    comboVariant->addItem(it->first, QVariant(it->first));

  variantChanged();
}

PrefTheme::~PrefTheme() {

}

void PrefTheme::apply() {
  SettingMap<QString> variants = settings.group("variants").map<QString>("variant", "name");

  for(std::map<QString, QString>::iterator it = m_new_piece_themes.begin();
                                            it != m_new_piece_themes.end(); ++it) {
    Settings var = variants.insert(it->first);
    var["piece-theme"] = it->second;
  }
  for(std::map<QString, bool>::iterator it = m_new_use_def_pieces.begin();
                                            it != m_new_use_def_pieces.end(); ++it) {
    Settings var = variants.insert(it->first);
    var["use-def-pieces"] = it->second;
  }
  for(std::map<QString, QString>::iterator it = m_new_square_themes.begin();
                                            it != m_new_square_themes.end(); ++it) {
    Settings var = variants.insert(it->first);
    var["square-theme"] = it->second;
  }
  for(std::map<QString, bool>::iterator it = m_new_use_def_squares.begin();
                                            it != m_new_use_def_squares.end(); ++it) {
    Settings var = variants.insert(it->first);
    var["use-def-squares"] = it->second;
  }

  for(std::map<QString, boost::shared_ptr<OptList> >::iterator it = m_new_theme_options.begin();
          it != m_new_theme_options.end(); ++it) {
    SettingMap<QString> s_lua = settings.group("lua-settings").map<QString>("entry", "file-name");
    Settings entry = s_lua.insert(it->first);
    options_list_save_to_settings(*it->second, entry.group("options"));
  }
}

void PrefTheme::update_list_view(QListWidget* list, const ThemeInfoList& themes,
                                    QString variant, QString settings) {
  list->clear();

  int selected = 0;
  QListWidgetItem *prev_sel = 0;

  for(int i=0;i<themes.size();i++) {
    bool ok1 = themes[i].variants.contains(variant, Qt::CaseInsensitive);
    bool ok2 = themes[i].variants.contains(variant+"[default]", Qt::CaseInsensitive);
    if(!ok1 && !ok2)
      continue;

    int ok = themes[i].file_name == settings ? 3 : ok2 ? 2 : 1;
    QListWidgetItem *w = new QListWidgetItem(themes[i].name, list);

    w->setData(Qt::UserRole, i);
    if(ok > selected) {
      if(prev_sel)
        list->setItemSelected(prev_sel, false);
      list->setItemSelected(w, true);
      prev_sel = w;
      selected = ok;
    }
  }
}

void PrefTheme::variantChanged() {
  QString c = comboVariant->itemData(comboVariant->currentIndex()).toString();
  VariantInfo *vi = Variant::variant(c);
  if(!vi) {
    checkPieces->hide();
    listPieces->clear();
    listPieces->setEnabled(false);
    labelPieces->setText(QString());
    labelPieces->setEnabled(false);
    checkSquares->hide();
    listSquares->clear();
    listSquares->setEnabled(false);
    labelSquares->setText(QString());
    labelSquares->setEnabled(false);
    return;
  }

  QString vname = vi->name();
  QString vproxy = vi->themeProxy();
  SettingMap<QString> variants = settings.group("variants").map<QString>("variant", "name");
  Settings var = variants.insert(vname);

  bool ck = vname != vproxy;
  checkSquares->setVisible(ck);
  checkPieces->setVisible(ck);
  if(ck) {
    bool dp = m_new_use_def_pieces.count(vname) ? m_new_use_def_pieces[vname]
                 : (var["use-def-pieces"] | true).value();
    bool ds = m_new_use_def_squares.count(vname) ? m_new_use_def_squares[vname]
                 : (var["use-def-squares"] | true).value();
    checkPieces->setText("Same as "+vproxy);
    checkPieces->setChecked(dp);
    listPieces->setEnabled(!dp);
    labelPieces->setEnabled(!dp);
    checkSquares->setText("Same as "+vproxy);
    checkSquares->setChecked(ds);
    listSquares->setEnabled(!ds);
    labelSquares->setEnabled(!ds);
  }
  else {
    listPieces->setEnabled(true);
    labelPieces->setEnabled(true);
    listSquares->setEnabled(true);
    labelSquares->setEnabled(true);
  }

  QString pth = m_new_piece_themes.count(vname) ? m_new_piece_themes[vname]
          : (var["piece-theme"] | QString()).value();
  QString sth = m_new_square_themes.count(vname) ? m_new_square_themes[vname]
          : (var["square-theme"] | QString()).value();
  update_list_view(listPieces, m_pieces_themes, vproxy, pth);
  update_list_view(listSquares, m_squares_themes, vproxy, sth);
}

void PrefTheme::piecesThemeChanged() {
  QList<QListWidgetItem *> l = listPieces->selectedItems();
  if(!l.isEmpty()) {
    int i = l[0]->data(Qt::UserRole).toInt();
    if(i>=0 && i<m_pieces_themes.size()) {
      labelPieces->setText(m_pieces_themes[i].description);

      QString c = comboVariant->itemData(comboVariant->currentIndex()).toString();
      VariantInfo *vi = Variant::variant(c);
      if(vi)
        m_new_piece_themes[vi->name()] = m_pieces_themes[i].file_name;

      if(m_pieces_opt_widget) {
        delete m_pieces_opt_widget;
        m_pieces_opt_widget = NULL;
      }
      OptList ol = get_file_options(m_pieces_themes[i].file_name);
      if(ol.size() != 0) {
        m_pieces_opt_widget = new OptionWidget(ol, widgetPieces);
        m_pieces_opt_layout->addWidget(m_pieces_opt_widget);
      }
      return;
    }
  }
  labelPieces->setText(QString());
}

void PrefTheme::piecesThemeChecked(bool ck) {
  listPieces->setEnabled(!ck);
  labelPieces->setEnabled(!ck);

  QString c = comboVariant->itemData(comboVariant->currentIndex()).toString();
  VariantInfo *vi = Variant::variant(c);
  if(vi)
    m_new_use_def_pieces[vi->name()] = ck;
}

void PrefTheme::squaresThemeChanged() {
  QList<QListWidgetItem *> l = listSquares->selectedItems();
  if(!l.isEmpty()) {
    int i = l[0]->data(Qt::UserRole).toInt();
    if(i>=0 && i<m_squares_themes.size()) {
      labelSquares->setText(m_squares_themes[i].description);

      QString c = comboVariant->itemData(comboVariant->currentIndex()).toString();
      VariantInfo *vi = Variant::variant(c);
      if(vi)
        m_new_square_themes[vi->name()] = m_squares_themes[i].file_name;

      if(m_squares_opt_widget) {
        delete m_squares_opt_widget;
        m_squares_opt_widget = NULL;
      }
      OptList ol = get_file_options(m_squares_themes[i].file_name);
      if(ol.size() != 0) {
        m_squares_opt_widget = new OptionWidget(ol, widgetSquares);
        m_squares_opt_layout->addWidget(m_squares_opt_widget);
      }
      return;
    }
  }
  labelSquares->setText(QString());
}

void PrefTheme::squaresThemeChecked(bool ck) {
  listSquares->setEnabled(!ck);
  labelSquares->setEnabled(!ck);

    QString c = comboVariant->itemData(comboVariant->currentIndex()).toString();
    VariantInfo *vi = Variant::variant(c);
  if(vi)
    m_new_use_def_squares[vi->name()] = ck;
}

QString PrefTheme::getBestTheme(VariantInfo* vi, ThemeType type) {
  QString group = type == Pieces ? "pieces" :
                  type == Squares ? "squares" : "figurines";
  QString pattern = type == Pieces ? "themes/Pieces/*.lua" :
                  type == Squares ? "themes/Squares/*.lua" : "themes/Figurines/*.lua";
  QString tag = group + "-theme";
  QString deftag = "use-def-" + group;
  QString v = vi->name();
  SettingMap<QString> variants = settings.group("variants").map<QString>("variant", "name");
  Settings var = variants.insert(v);
  if (v != vi->themeProxy() &&
      (var[deftag] | true) )
    v = vi->themeProxy();

  if (var[tag])
    return var[tag].value<QString>();

  MasterSettings s(".kboard_config_cache.xml");
  KStandardDirs* dirs = KGlobal::dirs();
  ThemeInfoList themes = to_theme_info_list(dirs->findAllResources("appdata", pattern, true), s.group(group));

  int best = 0;
  QString retv;
  for(int i=0;i<themes.size();i++) {
    bool ok1 = themes[i].variants.contains(v, Qt::CaseInsensitive);
    bool ok2 = themes[i].variants.contains(v+"[default]", Qt::CaseInsensitive);
    if(!ok1 && !ok2)
      continue;

    int ok = ok2 ? 2 : 1;
    if(ok > best) {
      retv = themes[i].file_name;
      best = ok;
    }
  }

  if (!retv.isEmpty())
    var[tag] = retv;
  return retv;
}

#include "pref_theme.moc"
