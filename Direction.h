#ifndef DIRECTION_H
#define DIRECTION_H

#include <QString>
#include <QColor>

enum class Direction{north = 0x1, east = 0x2, south = 0x4, west = 0x8};

enum PlayerAction{none = 0x0, movement = 0b10000, movePlayerNorth = 0b10001,movePlayerEast = 0x12,movePlayerSouth = 0x14,movePlayerWest = 0x18,
                        playerSwitch = 0x20, switchPlayerNorth = 0b100001,switchPlayerEast = 0x22,switchPlayerSouth = 0x24,switchPlayerWest = 0x28,
						bidding = 0x40, enterBidding = 0x41, sendBidding = 0x42, clearBidding = 0x44,
                        other = 128, giveUp = 129, revert = 130, revertToBeginning= 132, freePlay = 136,
                        user = 0x100, newUser = 0x101, changedUsername = 0x102, changedUserColor = 0x104};


struct UserData{
    QString name;
    QColor colour;
};
#endif // DIRECTION_H
