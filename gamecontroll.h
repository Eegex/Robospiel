#ifndef GAMECONTROLL_H
#define GAMECONTROLL_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "LeaderBoardWidget.h"
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
    bool showTopBidding();
    bool triggerAction(PlayerAction action, QUuid userID);
    void triggerActionsWithData(PlayerAction action, QJsonObject data);

	Board * setBoard(Board *newBoard);
    Board * getBoard() const;
	QVector<KeyMapping*> * getMapping();
    User * getMinBid();
	Phase getCurrentPhase() const;
	SettingsDialog * getSettingsDialog();
	QJsonObject toJSON();
	static GameControll *fromJSON(QJsonObject json);
	void setLeaderboard(LeaderBoardWidget * value);

public slots:
	void calculateWinner(int moves);
	void showSettings();
	void setMapping(QVector<KeyMapping *> mapping);
	void nextTarget();
	void remakeBoard();
	QUuid getActiveUserID();
	void setActiveUserID(QUuid id);
    void addOnlineUser(User *user);

private:
	QVector<User*> users;
	LeaderBoardWidget * leaderboard = nullptr;
	Phase currentPhase = Phase::idle; //freeplay
	SettingsDialog * settings = nullptr;
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	//QString activeUser;
	QUuid activeUserID;
	QTimer countdown;
	int timeLeft;
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
private slots:
	void updateTimer();
	bool switchPhase(GameControll::Phase phase);
	void exeQTAction(QJsonObject data);
	void sendToServerWithData(PlayerAction a, QJsonObject info);
	void calculateGameStatus();
	void changeBidding(int bidding, QUuid id);
	void changeOnlyBidding(int bidding);
    void addOfflineUser(struct UserData * newUser);
//	void initializeUser();
};

#endif // GAMECONTROLL_H
