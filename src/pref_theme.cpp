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
#include "settings.h"
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

QStringList PrefTheme::find_lua_files(const QDir& d) {
  QStringList files = d.entryList(QStringList()<<"*.lua", QDir::Files);
  QStringList dirs = d.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

  QStringList retv;
  for(int i=0;i<files.size();i++)
    retv << d.filePath(files[i]);

  for(int i=0;i<dirs.size();i++)
    retv += find_lua_files(d.filePath(dirs[i]));

  return retv;
}

PrefTheme::ThemeInfoList PrefTheme::to_theme_info_list(const QStringList& files, Settings& s) {
  std::map<QString, ThemeInfo> cache;

  int size = s.qSettings()->beginReadArray("Themes");
  for (int i = 0; i < size; i++) {
    s.qSettings()->setArrayIndex(i);
    QString f = s["file_name"].value<QString>();
    cache.insert( std::make_pair(f, ThemeInfo( f,
                  s["name"].value<QString>(),
                  s["description"].value<QString>(),
                  (s["variants"] | QStringList()).value<QStringList>(),
                  s["last_modified"].value<QDateTime>() ) ) );
  }
  s.qSettings()->endArray();

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
      QString name = l.getName();
      if(name.isEmpty()) {
        allluafiles << ThemeInfo(files[i], QString(), QString(), QStringList(), lm );
        continue;
      }

      ThemeInfo t(files[i], name, l.getDescription(), l.getVariants(), lm);
      retv << t;
      allluafiles << t;
    }
  }

  if(updated) {
    s.qSettings()->beginWriteArray("Themes");
    for(int i=0;i<allluafiles.size();i++) {
      s.qSettings()->setArrayIndex(i);
      s["file_name"]     = allluafiles[i].file_name;
      s["name"]          = allluafiles[i].name;
      s["description"]   = allluafiles[i].description;
      s["variants"]      = allluafiles[i].variants;
      s["last_modified"] = allluafiles[i].last_modified;
    }
    s.qSettings()->endArray();
  }

  return retv;
}

OptList PrefTheme::get_file_options(const QString& f) {
  if(boost::shared_ptr<OptList> o = m_new_theme_options[f])
    return *o;

  LuaApi::Loader l(NULL);
  l.runFile(f);

  boost::shared_ptr<OptList> o = boost::shared_ptr<OptList>(new OptList(l.getOptions()));
  settings.qSettings()->beginGroup("LuaSettings/"+QString::number(qHash(f)));
  options_list_load_from_settings(*o, settings);
  settings.qSettings()->endGroup();

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

  QString themeDir = data_dir() + "/themes/";

  Settings s(".kboard_config_cache");
  s.qSettings()->beginGroup("Pieces");
  m_pieces_themes = to_theme_info_list(find_lua_files(themeDir+"Pieces"), s);
  s.qSettings()->endGroup();
  s.qSettings()->beginGroup("Squares");
  m_squares_themes = to_theme_info_list(find_lua_files(themeDir+"Squares"), s);
  s.qSettings()->endGroup();

  const Variant::Variants& all = Variant::allVariants();
  for(Variant::Variants::const_iterator it = all.begin(); it != all.end(); ++it)
    comboVariant->addItem(it->first, QVariant(it->first));

  variantChanged();
}

PrefTheme::~PrefTheme() {

}

void PrefTheme::apply() {

  for(std::map<QString, QString>::iterator it = m_new_piece_themes.begin();
                                            it != m_new_piece_themes.end(); ++it)
    settings["Variants/"+it->first+"/PiecesTheme"] = it->second;
  for(std::map<QString, bool>::iterator it = m_new_use_def_pieces.begin();
                                            it != m_new_use_def_pieces.end(); ++it)
    settings["Variants/"+it->first+"/UseDefPieces"] = it->second;
  for(std::map<QString, QString>::iterator it = m_new_square_themes.begin();

                                            it != m_new_square_themes.end(); ++it)
    settings["Variants/"+it->first+"/SquaresTheme"] = it->second;
  for(std::map<QString, bool>::iterator it = m_new_use_def_squares.begin();
                                            it != m_new_use_def_squares.end(); ++it)
    settings["Variants/"+it->first+"/UseDefSquares"] = it->second;

  for(std::map<QString, boost::shared_ptr<OptList> >::iterator it = m_new_theme_options.begin();
          it != m_new_theme_options.end(); ++it) {
    settings.qSettings()->beginGroup("LuaSettings/"+QString::number(qHash(it->first)));
    options_list_save_to_settings(*it->second, settings);
    settings.qSettings()->endGroup();
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
  bool ck = vname != vproxy;
  checkSquares->setVisible(ck);
  checkPieces->setVisible(ck);
  if(ck) {
    bool dp = m_new_use_def_pieces.count(vname) ? m_new_use_def_pieces[vname]
                 : (settings["Variants/"+vname+"/UseDefPieces"] | true).value<bool>();
    bool ds = m_new_use_def_squares.count(vname) ? m_new_use_def_squares[vname]
                 : (settings["Variants/"+vname+"/UseDefSquares"] | true).value<bool>();
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
          : (settings["Variants/"+vname+"/PiecesTheme"] | QString()).value<QString>();
  QString sth = m_new_square_themes.count(vname) ? m_new_square_themes[vname]
          : (settings["Variants/"+vname+"/SquaresTheme"] | QString()).value<QString>();
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

QString PrefTheme::getBestTheme(VariantInfo* vi, bool squares) {
  QString type = squares ? "Squares" : "Pieces";
  QString v = vi->name();
  if(v != vi->themeProxy() &&
      (settings["Variants/"+v+"/UseDef"+type] | true).value<bool>() )
    v = vi->themeProxy();
  if(settings["Variants/"+v+"/"+type+"Theme"])
    return settings["Variants/"+v+"/"+type+"Theme"].value<QString>();

  QString themeDir = data_dir() + "/themes/";

  Settings s(".kboard_config_cache");
  s.qSettings()->beginGroup("Pieces");
  ThemeInfoList themes = to_theme_info_list(find_lua_files(themeDir+type), s);
  s.qSettings()->endGroup();

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

  if(!retv.isEmpty())
    settings["Variants/"+v+"/"+type+"Theme"] = retv;
  return retv;
}

#include "pref_theme.moc"
