/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
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
#include <KDebug>
#include <KStandardDirs>

#include "foreach.h"
#include "mastersettings.h"
#include "luaapi/loader.h"
#include "variants.h"
#include "tagua.h"
#include "pref_theme.h"


PrefTheme::ThemeInfoList PrefTheme::to_theme_info_list(const QStringList& files, const Settings& s) {
  //kDebug() << "about to examine " << files.size() << " desktop files";
  std::map<QString, ThemeInfo> cached_info;

  SettingArray themes = s.group("themes").array("theme");
  foreach (Settings s_theme, themes) {
    ThemeInfo info = ThemeInfo::fromSettings(s_theme);
    cached_info[info.desktopFile] = info;
  }

  ThemeInfoList all_themes;
  bool updated = false;

  for(int i = 0; i < files.size(); i++) {
    QFileInfo file_info(files[i]);
    std::map<QString, ThemeInfo>::iterator it = cached_info.find(files[i]);

    if (it != cached_info.end()
        && file_info.exists()
        && it->second.last_modified == file_info.lastModified() ) {
      all_themes << it->second;
      cached_info.erase(it);
    }
    else {
      updated = true;

      ThemeInfo theme_info = ThemeInfo::fromDesktopFile(files[i]);
      all_themes << theme_info;

      if (theme_info.name.isEmpty()) {
        kError() << "No name property in" << files[i];
      }
    }
  }

  if(!cached_info.empty())
    updated = true;

  /* rewrite the cached configuration */
  if(updated) {
    SettingArray themes = s.group("themes").newArray("theme");

    for (int i = 0; i < all_themes.size(); i++) {
      Settings s_theme = themes.append();
      all_themes[i].save(s_theme);
    }
  }

  return all_themes;
}


OptList PrefTheme::get_file_options(const QString& f, bool reload_defaults) {
  //kDebug() << "get file options for " << f;

  if(!reload_defaults) {
    std::map<QString, OptList>::iterator it = m_new_theme_options.find(f);

    if(it != m_new_theme_options.end())
      return it->second;
  }

  LuaApi::Loader lua_context;
  lua_context.runFile(f);

  if(lua_context.error()) {
    kError() << lua_context.errorString();
    lua_context.clearError();

    m_new_theme_options[f] = OptList();
    return OptList();
  }

  OptList o = lua_context.getValue<OptList>("options", 0, NULL, true);
  if(lua_context.error()) {
    kError() << lua_context.errorString();
    lua_context.clearError();
  }

  if(!reload_defaults) {
    SettingMap<QString> s_lua = settings().group("lua-settings").map<QString>("entry", "file-name");
    Settings entry = s_lua.insert(f);
    options_list_load_from_settings(o, entry.group("options"));

    m_new_theme_options[f] = o;
  }
  return o;
}


int PrefTheme::theme_ok_for_variant(const ThemeInfo& theme_info, const QString& variant_name) {
  if(theme_info.variants.contains(variant_name+"[default]", Qt::CaseInsensitive))
    return 4;
  if(theme_info.variants.contains(variant_name, Qt::CaseInsensitive))
    return 3;
  if(theme_info.variants.contains("any[default]", Qt::CaseInsensitive))
    return 2;
  if(theme_info.variants.contains("any", Qt::CaseInsensitive))
    return 1;
  return 0;
}


PrefTheme::PrefTheme(const QString& currentVariant, QWidget *parent)
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

  MasterSettings cached_theme_info("tagua_config_cache.xml");
  connect(comboVariant, SIGNAL(currentIndexChanged(int)), this, SLOT(variantChanged()));

  for(CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
    Category* c = cit->second;

    c->m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(c->m_list, SIGNAL(itemSelectionChanged()), c, SLOT(themeChanged()));
    connect(c->m_check, SIGNAL(toggled(bool)), c, SLOT(themeChecked(bool)));
    c->m_opt_layout = new QHBoxLayout(c->m_widget);
    c->m_opt_layout->setMargin(0);

    c->m_themes = to_theme_info_list(
      KGlobal::dirs()->findAllResources("appdata",
                                        "themes/"+cit->first+"/*.desktop",
                                        KStandardDirs::Recursive),
      cached_theme_info.group(cit->first)
    );

    //kDebug() << "loaded " << c->m_themes.size() << " themes";
  }

  QStringList all = Variants::instance().all();
  int index = 0;
  int current = -1;
  foreach (QString variant, all) {
    if (variant == currentVariant) {
      current = index;
    }
    comboVariant->addItem(variant, QVariant(variant));
    index++;
  }

  if (current != -1)
    comboVariant->setCurrentIndex(current);

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

  for(std::map<QString, OptList>::iterator it = m_new_theme_options.begin();
          it != m_new_theme_options.end(); ++it) {
    SettingMap<QString> s_lua = settings().group("lua-settings").map<QString>("entry", "file-name");
    Settings entry = s_lua.insert(it->first);
    options_list_save_to_settings(it->second, entry.group("options"));
  }
}

void PrefTheme::update_list_view(QListWidget* list, const ThemeInfoList& themes,
                                    QString variant_name, QString settings_theme) {
  list->clear();

  int selected_ok = 0;
  QListWidgetItem *item_to_select = NULL;

  for (int i = 0; i < themes.size(); i++) {
    int ok = theme_ok_for_variant(themes[i], variant_name);
    if (!ok)
      continue;

    ok = (themes[i].desktopFile == settings_theme) ? 5 : ok;
    QListWidgetItem *list_item = new QListWidgetItem(themes[i].name, list);

    list_item->setData(Qt::UserRole, i);
    if(ok > selected_ok) {
      item_to_select = list_item;
      selected_ok = ok;
    }
  }

  if(item_to_select)
    list->setItemSelected(item_to_select, true);
}

void PrefTheme::variantChanged() {
  QString category = comboVariant->itemData(comboVariant->currentIndex()).toString();
  VariantFactory* vi = Variants::instance().getFactory(category);

  if (!vi) {
    for (CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
      Category* c = cit->second;

      c->m_check->hide();
      c->m_list->clear();
      c->m_list->setEnabled(false);
      c->m_label->setText(QString());
      c->m_label->setEnabled(false);
    }

    return;
  }

  QString variant_name = vi->name();
  QString variant_proxy = vi->themeProxy();
  SettingMap<QString> variants = settings().group("variants").map<QString>("variant", "name");
  Settings var = variants.insert(variant_name);
  bool check_proxy = variant_name != variant_proxy;

  for(CategoryMap::iterator cit = m_categories.begin(); cit != m_categories.end(); ++cit) {
    Category* c = cit->second;

    c->m_check->setVisible(check_proxy);
    if(check_proxy) {
      bool use_def = c->m_new_use_def.count(variant_name)
                     ? c->m_new_use_def[variant_name]
                     : (var[cit->first+"-use-def"] | true).value();
      c->m_check->setText("Same as "+variant_proxy);
      c->m_check->setChecked(use_def);
      c->m_list->setEnabled(!use_def);
      c->m_label->setEnabled(!use_def);
    }
    else {
      c->m_list->setEnabled(true);
      c->m_label->setEnabled(true);
    }

    QString settings_theme = c->m_new_themes.count(variant_name)
                              ? c->m_new_themes[variant_name]
                              : (var[cit->first+"-theme"] | QString()).value();
    update_list_view(c->m_list, c->m_themes, variant_proxy, settings_theme);
  }
}

ThemeInfo PrefTheme::getBestTheme(const VariantPtr& vi, const QString& category) {
  QString tag = category + "-theme";
  QString deftag = category + "-use-def";
  QString variant_name = vi->name();
  QString variant_proxy_name = vi->themeProxy();
  SettingMap<QString> variants = settings().group("variants").map<QString>("variant", "name");
  if (variant_name != vi->themeProxy() &&
      (variants.insert(variant_name)[deftag] | true) )
    variant_name = vi->themeProxy();

  Settings var_settings = variants.insert(variant_name);
  if (var_settings[tag] && QFile::exists(var_settings[tag].value<QString>()) ) {

    // there is a theme in the settings, so pick this
    ThemeInfo res = ThemeInfo::fromDesktopFile(var_settings[tag].value<QString>());
    if(theme_ok_for_variant(res, variant_proxy_name))
      return res;
  }

  MasterSettings cached_theme_info("tagua_config_cache.xml");
  KStandardDirs* std_dirs = KGlobal::dirs();
  ThemeInfoList themes = to_theme_info_list(std_dirs->findAllResources("appdata",
                                                                  "themes/" + category + "/*.desktop",
                                                                  KStandardDirs::Recursive ),
                                            cached_theme_info.group(category));

  int best = 0;
  ThemeInfo* retv = 0;
  for(int i = 0; i < themes.size(); i++) {
    int ok = theme_ok_for_variant(themes[i], variant_proxy_name);
    if (!ok)
      continue;

    if (ok > best) {
      retv = &themes[i];
      best = ok;
    }
  }


  if (retv && *retv) {
    var_settings[tag] = retv->desktopFile;
  }

  return retv ? *retv : ThemeInfo();
}

PrefThemeCategory::PrefThemeCategory(QWidget* parent, PrefTheme* owner)
  : QWidget(parent)
  , m_parent(owner)
  , m_opt_layout(NULL)
  , m_opt_widget(NULL) {
  setupUi(this);
  m_reset = new QAction(KIcon("edit-clear"), "&Reset to default", this);
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
      VariantFactory* vi = Variants::instance().getFactory(c);
      if(vi)
        m_new_themes[vi->name()] = m_themes[i].desktopFile;

      if(m_opt_widget) {
        delete m_opt_widget;
        m_opt_widget = NULL;
      }
      OptList ol = m_parent->get_file_options(m_themes[i].file_name);
      if (ol.size() != 0 && m_list->isEnabled()) {
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
  VariantFactory* vi = Variants::instance().getFactory(c);
  if (vi)
    m_new_use_def[vi->name()] = ck;
  themeChanged();
}

