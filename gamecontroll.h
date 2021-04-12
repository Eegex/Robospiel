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

enum strategy {points, bid};
class GameControll : public QObject
{
	Q_OBJECT
public:
	static GameControll& getInstance();

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

    static void initializeConnections();
    void addUser(User *user);

    void sortBy(strategy strategy);
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
	Phase currentPhase = Phase::idle; //freeplay
	SettingsDialog * settings = nullptr;
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	QUuid activeUserID;
	QTimer countdown;
	int timeLeft;

	explicit GameControll(QObject *parent = nullptr);
	void sendToServer(PlayerAction a);
    User *getUserById(QUuid id);
    User *getNextUser(QUuid lastUserId);
    int getUserIndexById(QUuid id);
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
	void calculateGameStatus();
	void changeBidding(int bidding, QUuid id);
	void changeOnlyBidding(int bidding);
    //void addOfflineUser(struct UserData * newUser);
};

#endif // GAMECONTROLL_H
