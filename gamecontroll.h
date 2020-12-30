#ifndef GAMECONTROLL_H
#define GAMECONTROLL_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "user.h"
#include "keymapping.h"
#include "board.h"
#include "Direction.h"


class GameControll : public QObject
{
	Q_OBJECT
public:
	enum class Phase{idle, search, countdown, presentation, freeplay};
	explicit GameControll(QObject *parent = nullptr);
	Board * createBoard(int width, int height, int playerNumber);
    //bool triggerAction(PlayerAction action, QString user); //String will need to be ID of user
    bool triggerAction(PlayerAction action, QUuid userID);
    Board * getBoard() const;

	QVector<KeyMapping*> * getMapping();

public slots:
    void setMapping(QVector<KeyMapping *> mapping);
	void nextTarget();
	void remakeBoard();

    void activePlayerChanged(int playerNumber);
private:
    Phase currentPhase = Phase::search; //freeplay
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
    //QString activeUser;
    QUuid activeUserID;
	QTimer countdown;
	int timeLeft;
signals:
	void actionTriggered(PlayerAction action);
	void time(int secs);

private slots:
	void updateTimer();
	bool switchPhase(GameControll::Phase phase);
};

#endif // GAMECONTROLL_H
