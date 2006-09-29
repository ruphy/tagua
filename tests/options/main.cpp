
#include <iostream>
#include <QApplication>
#include <QWidget>
#include "common.h"
#include "option.h"
#include "luaapi/loader.h"


OptList opts = OptList()
    << OptPtr(new BoolOpt("teto", "Teto?", true))
    << OptPtr(new BoolOpt("fufi", "Fufi?", true, OptList()
        << OptPtr(new BoolOpt("gni", "Gni gni?", true, OptList()
            << OptPtr(new ColorOpt("pappa", "Pappa?", Qt::red))
            << OptPtr(new FontOpt("ciccia", "Ciccia!", QFont("Monospace")))
        ))
        << OptPtr(new BoolOpt("gnu", "Gnu gnu?", false))
    ))
    << OptPtr(new IntOpt("ratta", "Ratta:", 5, 0, 10))
    << OptPtr(new ComboOpt("schiatta", "Fai a Iazzi:", QStringList()
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
  QApplication app(argc, argv);

  LuaApi::Loader ja(NULL);
  ja.runFile("test.lua");
  OptList opts2 = ja.getOptList("options");
  if(ja.error())
    std::cout << ja.errorString() << std::endl;
  else {
    dump_options_list(opts2);
    OptionWidget& w2 = *new OptionWidget(opts2);
    w2.show();
  }
  OptionWidget w(opts);
  w.show();
  return app.exec();
}
