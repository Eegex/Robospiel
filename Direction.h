#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction{north = 0x1, east = 0x2, south = 0x4, west = 0x8};

enum class PlayerAction{none = 0x0, movePlayerNorth = 0b10001,movePlayerEast = 0b10010,movePlayerSouth = 0b10100,movePlayerWest = 0b11000,
                         switchPlayerNorth = 0b100001,switchPlayerEast = 0b100010,switchPlayerSouth = 0b100100,switchPlayerWest = 0b101000,
                        enterBidding = 0b1000001, sendBidding = 0b1000010, clearBidding = 0b1000100,
						giveUp = 0x81, revert = 0x82, freePlay = 0x84};

#endif // DIRECTION_H
