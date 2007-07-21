/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
// */

#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QListWidgetItem>

#include <KDesktopFile>
#include <KStandardDirs>

#include "mastersettings.h"
#include "luaapi/loader.h"
#include "variants/variants.h"
#include "tagua.h"
#include "pref_theme.h"

void PrefTheme::read_theme_info(ThemeInfo& info, const QString& desktopFile) {
  info.desktopFile = desktopFile;
  if (!KDesktopFile::isDesktopFile(desktopFile)) {
    return;
  }
    
  KDesktopFile theme(desktopFile);
  KConfigGroup themeData = theme.desktopGroup();
  
  info.name = theme.readName();
  info.description = theme.readComment();
  info.description.replace("|", "\n");
  info.variants = themeData.readEntry("X-Tagua-Variants").split(QRegExp("\\s*,\\s*"));
  info.file_name = themeData.readEntry("X-Tagua-Script");
  if (info.file_name.isEmpty()) {
    // use a file with the same name as the .desktop, but with as .lua extension
    info.file_name = desktopFile;
    info.file_name.replace(QRegExp("\\.desktop$"), ".lua");
  }
  else {
    QFileInfo filePath(info.file_name);
    QFileInfo desktopPath(desktopFile);
    if (!filePath.isAbsolute()) {
    // the file is relative to the .desktop directory
      info.file_name = QDir(desktopPath.path()).filePath(info.file_name);
    }
  }
}

PrefTheme::ThemeInfoList PrefTheme::to_theme_info_list(const QStringList& files, const Settings& s) {
  std::cout << "about to examine " << files.size() << " desktop files" << std::endl;
  std::map<QString, ThemeInfo> cache;
  
  SettingArray themes = s.group("themes").array("theme");
  foreach (Settings s_theme, themes) {
    ThemeInfo info = ThemeInfo::fromSettings(s_theme);
    cache[info.desktopFile] = info;
  }

  ThemeInfoList allluafiles;
  ThemeInfoList retv;
  bool updated = false;
  
  for(int i = 0; i < files.size(); i++) {
    QDateTime lm = QFileInfo(files[i]).lastModified();
    std::map<QString, ThemeInfo>::iterator it = cache.find(files[i]);

    if (it != cache.end() && it->second.last_modified == lm) {
      if (!it->second.name.isEmpty())
        retv << it->second;
    }
    else {
      updated = true;
      
      ThemeInfo info;
      info.last_modified = lm;
      read_theme_info(info, files[i]);
      retv << info;
      allluafiles << info;
      
      if (info.name.isEmpty()) {
        ERROR("No name property in " << files[i]);
      }
    }
  }

  if(updated) {
    SettingArray themes = s.group("themes").newArray("theme");

    for (int i = 0; i < allluafiles.size(); i++) {
      Settings s_theme = themes.append();
      allluafiles[i].save(s_theme);
    }
  }
  
  return retv;
}

OptList PrefTheme::get_file_options(const QString& f, bool reload_defaults) {
  if(!reload_defaults)
  if(boost::shared_ptr<OptList> o = m_new_theme_options[f])
    return *o;

  LuaApi::Loader l;
  l.runFile(f);

  boost::shared_ptr<OptList> o(new OptList(l.getValue<OptList>("options", 0, NULL, true)));
  if(l.error()) {
    ERROR(l.errorString());
    l.clearError();
  }

  if(!reload_defaults) {
    SettingMap<QString> s_lua = settings().group("lua-settings").map<QString>("entry", "file-name");
    Settings entry = s_lua.insert(f);
    options_list_load_from_settings(*o, entry.group("options"));

    m_new_theme_options[f] = o;
  }
  return *o;
}

PrefTheme::PrefTheme(QWidget *parent)
: QWidget(parent) {
  setupUi(this);

  Category *c;

  c = new Category(NULL, this);
  m_categories["pieces"] = c;
  tabWidget->addTab(c, "&Pieces");

  c = new Category(NULL, this);
  m_categories["squares"] = c;
  tabWidget->addTab(c, "&Squares");

  c = new Category(NULL, this);
  m_categories["controls"] = c;
  tabWidget->addTab(c, "&Controls");

  MasterSettings s(".tagua_config_cache.xml");
  connect(comboVariant, SIGNAL(currentIndexChanged(int)), this, SLOT(variantChanged()));

  for(CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
    Category* c = cit->second;

    c->m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(c->m_list, SIGNAL(itemSelectionChanged()), c, SLOT(themeChanged()));
    connect(c->m_check, SIGNAL(toggled(bool)), c, SLOT(themeChecked(bool)));
    c->m_opt_layout = new QHBoxLayout(c->m_widget);
    c->m_opt_layout->setMargin(0);

    c->m_themes = to_theme_info_list(
      KGlobal::dirs()->findAllResources("appdata", "themes/"+cit->first+"/*.desktop", KStandardDirs::Recursive),
      s.group(cit->first)
    );
    
    std::cout << "loaded " << c->m_themes.size() << " themes" << std::endl;
  }

  const Variant::Variants& all = Variant::allVariants();
  for(Variant::Variants::const_iterator it = all.begin(); it != all.end(); ++it)
    comboVariant->addItem(it->first, QVariant(it->first));

  variantChanged();
}

PrefTheme::~PrefTheme() {

}

void PrefTheme::apply() {
  SettingMap<QString> variants = settings().group("variants").map<QString>("variant", "name");

  for(CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
    Category* c = cit->second;

    for(std::map<QString, QString>::iterator it = c->m_new_themes.begin();
                                              it != c->m_new_themes.end(); ++it) {
      Settings var = variants.insert(it->first);
      var[cit->first+"-theme"] = it->second;
    }
    for(std::map<QString, bool>::iterator it = c->m_new_use_def.begin();
                                              it != c->m_new_use_def.end(); ++it) {
      Settings var = variants.insert(it->first);
      var[cit->first+"-use-def"] = it->second;
    }
  }

  for(std::map<QString, boost::shared_ptr<OptList> >::iterator it = m_new_theme_options.begin();
          it != m_new_theme_options.end(); ++it) {
    SettingMap<QString> s_lua = settings().group("lua-settings").map<QString>("entry", "file-name");
    Settings entry = s_lua.insert(it->first);
    options_list_save_to_settings(*it->second, entry.group("options"));
  }
}

void PrefTheme::update_list_view(QListWidget* list, const ThemeInfoList& themes,
                                    QString variant, QString settings_theme) {
  list->clear();

  int selected = 0;
  QListWidgetItem *prev_sel = 0;

  for (int i = 0; i < themes.size(); i++) {
    int ok = 0;
    ok = themes[i].variants.contains("any", Qt::CaseInsensitive) ? 1 : ok;
    ok = themes[i].variants.contains("any[default]", Qt::CaseInsensitive) ? 2 : ok;
    ok = themes[i].variants.contains(variant, Qt::CaseInsensitive) ? 3 : ok;
    ok = themes[i].variants.contains(variant+"[default]", Qt::CaseInsensitive) ? 4 : ok;
    if (!ok)
      continue;
    ok = (themes[i].desktopFile == settings_theme) ? 5 : ok;
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
    for(CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
      Category* c = cit->second;

      c->m_check->hide();
      c->m_list->clear();
      c->m_list->setEnabled(false);
      c->m_label->setText(QString());
      c->m_label->setEnabled(false);
    }

    return;
  }

  QString vname = vi->name();
  QString vproxy = vi->themeProxy();
  SettingMap<QString> variants = settings().group("variants").map<QString>("variant", "name");
  Settings var = variants.insert(vname);
  bool ck = vname != vproxy;

  for(CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
    Category* c = cit->second;

    c->m_check->setVisible(ck);
    if(ck) {
      bool d = c->m_new_use_def.count(vname) ? c->m_new_use_def[vname]
                                : (var[cit->first+"-use-def"] | true).value();
      c->m_check->setText("Same as "+vproxy);
      c->m_check->setChecked(d);
      c->m_list->setEnabled(!d);
      c->m_label->setEnabled(!d);
    }
    else {
      c->m_list->setEnabled(true);
      c->m_label->setEnabled(true);
    }

    QString th = c->m_new_themes.count(vname) ? c->m_new_themes[vname]
                                : (var[cit->first+"-theme"] | QString()).value();
    update_list_view(c->m_list, c->m_themes, vproxy, th);
  }
}

ThemeInfo PrefTheme::getBestTheme(VariantInfo* vi, const QString& category) {
  QString tag = category + "-theme";
  QString deftag = category + "-use-def";
  QString v = vi->name();
  SettingMap<QString> variants = settings().group("variants").map<QString>("variant", "name");
  if (v != vi->themeProxy() &&
      (variants.insert(v)[deftag] | true) )
    v = vi->themeProxy();

  Settings var = variants.insert(v);
  if (var[tag]) {
    // there is a theme in the settings, so pick this
    ThemeInfo res;
    read_theme_info(res, var[tag].value<QString>());
    return res;
  }

  MasterSettings s(".tagua_config_cache.xml");
  KStandardDirs* dirs = KGlobal::dirs();
  ThemeInfoList themes = to_theme_info_list(dirs->findAllResources("appdata", "themes/"+category+"/*.desktop",
                                                  KStandardDirs::Recursive), s.group(category));
  std::cout << "found " << themes.size() << " themes" << std::endl;

  int best = 0;
  ThemeInfo* retv;
  for(int i=0;i<themes.size();i++) {
    int ok = 0;
    ok = themes[i].variants.contains("any", Qt::CaseInsensitive) ? 1 : ok;
    ok = themes[i].variants.contains("any[default]", Qt::CaseInsensitive) ? 2 : ok;
    ok = themes[i].variants.contains(v, Qt::CaseInsensitive) ? 3 : ok;
    ok = themes[i].variants.contains(v+"[default]", Qt::CaseInsensitive) ? 4 : ok;
    if (!ok)
      continue;

    if (ok > best) {
      retv = &themes[i];
      best = ok;
    }
  }

  if (*retv)
    var[tag] = retv->desktopFile;
    
  return *retv;
}

PrefThemeCategory::PrefThemeCategory(QWidget* parent, PrefTheme* owner)
  : QWidget(parent)
  , m_parent(owner)
  , m_opt_layout(NULL)
  , m_opt_widget(NULL) {
  setupUi(this);
  m_reset = new QAction(KIcon("eraser"), "&Reset to default", this);
  m_reset->setShortcut(Qt::CTRL+Qt::Key_Z);
  connect(m_reset, SIGNAL(triggered()), this, SLOT(reset()));
  m_resetButton->setDefaultAction(m_reset);
  m_resetButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void PrefThemeCategory::reset() {
  if(!m_opt_widget)
    return;

  QList<QListWidgetItem *> l = m_list->selectedItems();
  if(l.isEmpty())
    return;

  int i = l[0]->data(Qt::UserRole).toInt();
  if (i >= 0 && i < m_themes.size()) {
    OptList ol = m_parent->get_file_options(m_themes[i].file_name, true);
    qobject_cast<OptionWidget*>(m_opt_widget)->setValues(ol);
  }
}

void PrefThemeCategory::themeChanged() {
  QList<QListWidgetItem *> l = m_list->selectedItems();
  if(!l.isEmpty()) {
    int i = l[0]->data(Qt::UserRole).toInt();
    if(i>=0 && i<m_themes.size()) {
      m_label->setText(m_themes[i].description);

      QString c = m_parent->comboVariant->itemData(m_parent->comboVariant->currentIndex()).toString();
      VariantInfo *vi = Variant::variant(c);
      if(vi)
        m_new_themes[vi->name()] = m_themes[i].desktopFile;

      if(m_opt_widget) {
        delete m_opt_widget;
        m_opt_widget = NULL;
      }
      OptList ol = m_parent->get_file_options(m_themes[i].file_name);
      if(ol.size() != 0) {
        m_opt_widget = new OptionWidget(ol, m_widget);
        m_opt_layout->addWidget(m_opt_widget);
        m_reset->setEnabled(true);
      }
      else
        m_reset->setEnabled(false);
      return;
    }
  }
  m_label->setText(QString());
}

void PrefThemeCategory::themeChecked(bool ck) {
  m_list->setEnabled(!ck);
  m_label->setEnabled(!ck);

  QString c = m_parent->comboVariant->itemData(m_parent->comboVariant->currentIndex()).toString();
  VariantInfo *vi = Variant::variant(c);
  if(vi)
    m_new_use_def[vi->name()] = ck;
}

