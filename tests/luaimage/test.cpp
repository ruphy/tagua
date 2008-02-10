#include <QApplication>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QWidget>
#include "loader/theme.h"

using namespace Loader;

#if 0
Image do_something(LoadingContext* x) {
  Image i(300, 300);
  i.clear();
  i.rotate(20);
  i.translate(50,-30);
  i.drawImage(x, QRectF(100,100,200,200), "data/white_rook.png");
  i.drawSVG(x, QRectF(0,0,200,200), "data/wk.svg");
  i.resetMatrix();
  /*i.drawGlyph(x, QRectF(100,0,200,200), "tests_image/Enquil.ttf", 0x006B,
                        QColor(255,0,0,128), QColor(0,128,128,96), 2.0);*/
  i.drawGlyph(x, QRectF(100,0,200,200), "data/Enquil.ttf", 0x006B,
                        Qt::blue, Qt::red, 5.0, false);
  Image i2(150, 150);
  i2.clear();
  if(!i2.drawGlyph(x, QRectF(0,0,150,150), "data/Enquil.ttf", 0x0071,
                        Qt::NoBrush, Qt::white)) {
    i2.drawLine(QPointF(10,10),QPointF(140,140), Qt::white, 15);
    i2.drawLine(QPointF(10,140),QPointF(140,10), Qt::white, 15);
  }
  i.drawImage(QRectF(50,50,200,200), i2.createShadow(10, Qt::green) );
  return i;
}
#endif

void mkWidget(QImage img)
{
  QPainter p;
  p.begin(&img);
  p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
  for(int i=0;i<img.width();i+=16)
  for(int j=0;j<img.height();j+=16)
    p.fillRect(i, j, 16, 16, (i+j)%32 ? Qt::gray : Qt::white);
  p.end();

  QWidget& widget = *new QWidget;
  widget.setFixedSize(img.size());

  QPalette pal = widget.palette();
  pal.setBrush( QPalette::Window, QPixmap::fromImage(img) );
  widget.setPalette(pal);
  widget.setAutoFillBackground(true);

  widget.show();
}


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  //kDebug()()() << "Va a cagare " << QDir::cleanPath("pqr/abc/../xyz") << std::endl;

#if 0
  LoadingContext* x = new LoadingContext;
  LuaLoader::Api ape(x);
  ape.runFile("maintest.lua");

  mkWidget(ape.getImage("puppamelo", 200, 800));
  x->dump();
  delete x;
#endif

  Theme p("maintest.lua");
  p.refSize(250);
  mkWidget(p.getPixmap("puppamelo",250).toImage());
  p.unrefSize(250);

  return app.exec();
}
