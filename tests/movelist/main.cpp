#include <kapplication.h>
#include <kcmdlineargs.h>
#include "movelist_textual.h"

int main(int argc, char* argv[]) {
  KCmdLineArgs::init(argc, argv, argv[0], 0, KLocalizedString(), NULL);
  KApplication app;

  MoveList::Textual w;
  w.setMove(Index(1), 0, "e4");
  w.setMove(Index(2), 1, "e5");
  w.setComment(Index(2), "A good answer");
  w.setMove(Index(2).next(0,1), 0, "Nf3");
  w.setVComment(Index(2), 0, "Sadly, many people still play this move");
  w.setComment(Index(2).next(0,1), "This move is much worse than f4");
  w.setMove(Index(2).next(0,2), 1, "Nc6");
  w.setMove(Index(2).next(0,3), 0, "Bb5");
  w.setMove(Index(2).next(0,4), 1, "Nf6");
  w.setMove(Index(2).next(0,3).next(0,1), 1, "f5");
  w.setVComment(Index(2).next(0,3), 0, "Here is the confutation of Bb5");
  w.setComment(Index(2).next(0,3).next(0,1), "White is hopeless");
  w.setMove(Index(2).next(1,1), 0, "Nc3");
  w.setVComment(Index(2), 1, "Another playable move");
  w.setComment(Index(2).next(1,1), "Ok?");
  w.setMove(Index(3), 0, "f4");
  w.setMove(Index(4), 1, "Bc5");
  w.select(Index(1));
  w.select(Index(4));
  //w.remove(Index(2).next(0,1));
  w.show();

  return app.exec();
}
