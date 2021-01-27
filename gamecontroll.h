#ifndef GAMECONTROLL_H
#define GAMECONTROLL_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "user.h"
#include "settingsdialog.h"
#include "keymapping.h"
#include "board.h"
#include "Direction.h"


class GameControll : public QObject
{
	Q_OBJECT
public:
	enum class Phase{idle, search, countdown, presentation, freeplay};
	explicit GameControll(QObject *parent = nullptr);
	void load();
	Board * setBoard(Board *newBoard);
	//bool triggerAction(PlayerAction action, QString user); //String will need to be ID of user
	bool triggerAction(PlayerAction action, QUuid userID);
	Board * getBoard() const;
	QVector<KeyMapping*> * getMapping();
	Phase getCurrentPhase() const;
	SettingsDialog * getSettingsDialog();
	bool showTopBidding();
	void triggerActionsWithData(PlayerAction action, User *user);
	QJsonObject toJSON();
    static GameControll *fromJSON(QJsonObject json);
    void endTimer();
public slots:
	void showSettings();
	void setMapping(QVector<KeyMapping *> mapping);
	void nextTarget();
	void remakeBoard();
	QUuid getActiveUserID();
	void setActiveUserID(QUuid id);
	void activePlayerChanged(int playerNumber);
    void setIdle();
private:
	Phase currentPhase = Phase::idle; //freeplay
	SettingsDialog * settings = nullptr;
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	//QString activeUser;
	QUuid activeUserID;
	QTimer countdown;
	int timeLeft;
	void triggerActionsWithData(PlayerAction action, QJsonObject data=QJsonObject());
	void sendToServer(PlayerAction a);
signals:
	void actionTriggeredWithData(PlayerAction action, QJsonObject additionalData);
	void actionTriggered(PlayerAction action);
	void time(int secs);
	void colorsChanged();
	void newRound();
	void biddingDone();
	void newOnlineUser(User* user);
	void updateGuide(const QString & txt);
    void enableMenus(bool boolean);
    void enableTimerSkip(bool boolean);
private slots:
	void updateTimer();
    bool switchPhase(GameControll::Phase phase);
	void exeQTAction(QJsonObject data);
	void sendToServerWithData(PlayerAction a, QJsonObject info);
};

#endif // GAMECONTROLL_H
