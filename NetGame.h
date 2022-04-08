#ifndef NETGAME_H
#define NETGAME_H
#include "Board.h"

// 无服务器的
class NetGame : public Board {
    Q_OBJECT
public:
    NetGame();
};

#endif // NETGAME_H
