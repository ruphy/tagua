
#include <iostream>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <QWidget>
#include "common.h"
#include "settings.h"
#include "option.h"
#include "luaapi/loader.h"


OptList opts = OptList()
    << OptPtr(new BoolOpt("teto", "Is this C++?", true))
    << OptPtr(new BoolOpt("fufi", "Fufi?", true, OptList()
        << OptPtr(new BoolOpt("gni", "Gni gni?", true, OptList()
            << OptPtr(new ColorOpt("pappa", "Pappa?", Qt::red))
            << OptPtr(new FontOpt("ciccia", "Ciccia!", QFont("Monospace")))
        ))
        << OptPtr(new BoolOpt("gnu", "Gnu gnu?", false))
    ))
    << OptPtr(new IntOpt("ratta", "Ratta:", 5, 0, 10))
    << OptPtr(new ComboOpt("schiatta", "Fai a Paolo:", QStringList()
        << "Incula" << "Impala" << "Spezza" << "Uccidi" << "Tortura" ))
    << OptPtr(new SelectOpt("affa", "Affanculo:", BoolOptList()
        << BoolOptPtr(new BoolOpt("ght", "Case1", true, OptList()
            << OptPtr(new BoolOpt("gni2", "Gni gni?", true))
        ))
        << BoolOptPtr(new BoolOpt("cvb", "Case2", true, OptList()
            << OptPtr(new StringOpt("sheshe", "She She:", "Sho shu?"))
            << OptPtr(new UrlOpt("shesdsshe", "She She:", "Sho shu?"))
        ))
    ));

int main(int argc, char *argv[]) {
  KCmdLineArgs::init(argc, argv, argv[0], NULL, NULL, NULL);
  KApplication app;

  LuaApi::Loader ja(NULL);
  //Settings sets("test.settings");
  ja.runFile("test.lua");
  OptList opts2 = ja.getOptList("options");
  if(ja.error())
    std::cout << ja.errorString() << std::endl;
  else {
    /*sets.qSettings()->beginGroup("o2");
    options_list_load_from_settings(opts2, sets);
    sets.qSettings()->endGroup();*/
    dump_options_list(opts2);
    OptionWidget& w2 = *new OptionWidget(opts2);
    w2.show();
  }
  /*sets.qSettings()->beginGroup("o1");
  options_list_load_from_settings(opts, sets);
  sets.qSettings()->endGroup();*/
  OptionWidget w(opts);
  w.show();
  app.exec();

  /*sets.qSettings()->beginGroup("o1");
  options_list_save_to_settings(opts, sets);
  sets.qSettings()->endGroup();
  sets.qSettings()->beginGroup("o2");
  options_list_save_to_settings(opts2, sets);
  sets.qSettings()->endGroup();*/

  return 0;
}
