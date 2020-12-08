#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction{north = 0b1, east = 0b10, south = 0b100, west = 0b1000};

enum PlayerAction{none = 0x0, movement = 0x10, movePlayerNorth = 0x11,movePlayerEast = 0x12,movePlayerSouth = 0x14,movePlayerWest = 0x18,
						playerSwitch = 0x20, switchPlayerNorth = 0x21,switchPlayerEast = 0x22,switchPlayerSouth = 0x24,switchPlayerWest = 0x28,
						bidding = 0x40, enterBidding = 0x41, sendBidding = 0x42, clearBidding = 0x44,
						other = 0x80, giveUp = 0x81, revert = 0x82, freePlay = 0x84};

#endif // DIRECTION_H
