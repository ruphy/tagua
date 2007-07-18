#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include "canvas.h"


class Puppas : public Canvas
{
    CanvasItem *ppp;
    QPoint delta;
public:
    Puppas(QWidget *p = NULL) : Canvas(p), ppp(NULL) {}
    virtual ~Puppas(){}
    virtual void keyPressEvent ( QKeyEvent * event )
    {
        if(event->key() == Qt::Key_Tab)
            (*items())[0]->raise();
    }
    virtual void mousePressEvent ( QMouseEvent * event )
    {
        ppp = itemAt(event->pos());
        if(ppp)
        {
            delta = ppp->pos() - event->pos();
            if(event->button() == Qt::LeftButton)
                ppp->raise();
        }
    }
    virtual void mouseMoveEvent ( QMouseEvent * event )
    {
        if(ppp)
          ppp->moveTo(event->pos() + delta );
    }
};

QPicture make_pic()
{
    QPicture pic;
    QPainter p(&pic);
    p.setPen(Qt::yellow);
    p.setBrush(Qt::darkGreen);
    p.drawEllipse(0,0,200,100);

    return pic;
}

/*QPixmap tp(QPixmap px)
{
    QPixmap retv(px.size);
    QPainter p(retv);

    return retv;
}*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Canvas *c = new Puppas();
    c->resize(500,500);

    CanvasGroup *cg = new CanvasGroup( c );
    cg->moveTo(0,0);
    cg->show();

    CanvasPixmap *p = new CanvasPixmap( QPixmap(
        "/home/monge/chess_pieces/out/img_fantasy/kboard_theme/white_rook.png"), cg );
    p->moveTo(0,0);
    p->show();

    CanvasPixmap *p2 = new CanvasPixmap( QPixmap(
        "/home/monge/chess_pieces/out/img_skulls/kboard_theme/black_bishop.png"), cg );
    p2->moveTo(200,50);
    p2->show();

    CanvasPixmap *p3 = new CanvasPixmap( QPixmap(
        "/home/monge/chess_pieces/out/img_freak/kboard_theme/white_knight.png"), c );
    p3->moveTo(100,250);
    p3->show();

    CanvasPixmap *p4 = new CanvasPixmap( QPixmap(
        "/home/monge/chess_pieces/out/img_spatial/kboard_theme/black_king.png"), c );
    p4->moveTo(20,150);
    p4->show();

    CanvasPixmap *p5 = new CanvasPixmap( QPixmap(
        "/home/monge/chess_pieces/out/img_prmi/kboard_theme/white_queen.png"), c );
    p5->moveTo(300,200);
    p5->show();

    CanvasTiledPixmap *p6 = new CanvasTiledPixmap( QPixmap(
        "/home/monge/chess_pieces/out/icons/20/r.png"),
        QSize(200,150), QPoint(0,0), false, c );
    p6->moveTo(200,100);
    p6->show();

    CanvasRectangle *p7 = new CanvasRectangle( QColor(255,0,0,192), QSize(200,150), c );
    p7->moveTo(50,350);
    p7->show();
/*
    CanvasRectangle *p8a = new CanvasRectangle( QColor(128,128,0,255), QSize(1,200), c );
    p8a->moveTo(200,0);
    p8a->show();

    CanvasRectangle *p8b = new CanvasRectangle( QColor(0,128,0,255), QSize(300,1), c );
    p8b->moveTo(100,80);
    p8b->show();
*/
    CanvasText *p8 = new CanvasText( "Fuck", QColor(0,0,255,192),
                                    QFont("Sans", 70, QFont::Bold),
                                    CanvasText::HCenter, CanvasText::VCenter, c );
    p8->moveTo(200,80);
    p8->show();

    CanvasPicture *p9 = new CanvasPicture( make_pic(), c );
    p9->moveTo(200,100);
    p9->show();

    c->show();

    return app.exec();
}
