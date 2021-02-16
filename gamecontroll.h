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
	static GameControll& getInstance();
	QVector<User*> sortedUsers;
	enum class Phase{idle, search, countdown, presentation, freeplay};

	static void load();
	static bool showTopBidding();
	static void triggerAction(PlayerAction action, QUuid userID);
	static void triggerActionsWithData(PlayerAction action, QJsonObject data);
	static void endTimer(); //?
	static Board * setBoard(Board *newBoard);
	static Board * getBoard();
	static QVector<KeyMapping*> * getMapping();
	static User * getMinBid();
	static Phase getCurrentPhase();
	static SettingsDialog * getSettingsDialog();
	static QJsonObject toJSON();
	static void adaptFromJSON(QJsonObject json);
	static void setLeaderboard(LeaderBoardWidget * value);
	static void addOnlineUser(User *user);
	static void initializeUser();
	void sortUsers(unsigned int strategy, QVector<User*>* sortedUsers);

	static void initializeConnections();
public slots:
	void calculateWinner(int moves);
	void showSettings();
	void setMapping(QVector<KeyMapping *> mapping);
	void nextTarget();
	void remakeBoard();
	QUuid getActiveUserID();
	void setActiveUserID(QUuid id);
	void setIdle();

private:
	static GameControll instance;
	QVector<User*> users;
	LeaderBoardWidget * leaderboard = nullptr;
	Phase currentPhase = Phase::freeplay; //freeplay
	SettingsDialog * settings = nullptr;
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	QUuid activeUserID;
	QTimer countdown;
	int timeLeft;
	int searchTime=60;

	explicit GameControll(QObject *parent = nullptr);
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
	void newBoard(Board* b);
private slots:
	void updateTimer();
	bool switchPhase(GameControll::Phase phase);
	void exeQTAction(QJsonObject data);
	void sendToServerWithData(PlayerAction a, QJsonObject info);
	void calculateGameStatus();
	void changeBidding(int bidding, QUuid id);
	void changeOnlyBidding(int bidding);
	void addOfflineUser(struct UserData * newUser);
};

#endif // GAMECONTROLL_H
