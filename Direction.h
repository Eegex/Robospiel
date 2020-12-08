#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction{north = 0b1, east = 0b10, south = 0b100, west = 0b1000};

enum class PlayerAction{none = 0x0, movePlayerNorth = 0x11,movePlayerEast = 0x12,movePlayerSouth = 0x14,movePlayerWest = 0x18,
						 switchPlayerNorth = 0x21,switchPlayerEast = 0x22,switchPlayerSouth = 0x24,switchPlayerWest = 0x28,
						enterBidding = 0x41, sendBidding = 0x42, clearBidding = 0x44,
						giveUp = 0x81, revert = 0x82, freePlay = 0x84};

#endif // DIRECTION_H
