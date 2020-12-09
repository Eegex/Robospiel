#ifndef GAMECONTROLL_H
#define GAMECONTROLL_H

#include <QObject>
#include "keymapping.h"
#include "board.h"
#include "Direction.h"

class GameControll : public QObject
{
	Q_OBJECT
public:
	explicit GameControll(QObject *parent = nullptr);
	Board * createBoard(int width, int height, int playerNumber);
	bool triggerAction(PlayerAction action, QString user);
	Board * getBoard() const;

    QVector<KeyMapping*> * getMapping();

public slots:
	void nextTarget();
	void remakeBoard();

private:
    QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	QString activeUser;
signals:
	void actionTriggered(PlayerAction action);

};

#endif // GAMECONTROLL_H
