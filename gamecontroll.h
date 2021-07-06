#ifndef GAMECONTROLL_H
#define GAMECONTROLL_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QMediaPlayer>
#include "LeaderBoardWidget.h"
#include "user.h"
#include "settingsdialog.h"
#include "keymapping.h"
#include "board.h"
#include "Direction.h"
#include "networkdebugger.h"

enum strategy {points, bid};
/**
 * @brief The GameControll class macht eigentlich alles
 */
class GameControll : public QObject
{
	Q_OBJECT
public:
	typedef void(GameControll::*functionPointer)();
	//One Line used in the Guide-System
	struct GuideLine
	{
		GuideLine(const QString & line,int duration) : line(line), duration(duration)
		{}
		const QString line;
		int duration;
	};
	enum class Phase{idle, search, countdown, presentation, freeplay};

	static GameControll& getInstance();



	static void load();
	static bool showTopBidding();
	static void triggerActionWithData(PlayerAction action, QJsonObject data);
	static void endTimer(); //?
	static Board * setBoard(Board *newBoard);
	static Board * getBoard();
	static QVector<KeyMapping*> * getMapping();
	static const User * getMinBid();
	static Phase getCurrentPhase();
	static SettingsDialog * getSettingsDialog();
	static QJsonObject toJSON();
	static void adaptFromJSON(QJsonObject json);
	static void setLeaderboard(LeaderBoardWidget * value);
	static void addOnlineUser(User *user);
	static User * initializeUser();
	static void showGuide(const QStringList & texts);
	static QString getLocalUserName();
	static void initializeConnections();
	static QVector<User*>* getUsers();
	static void addUser(User *user);

	static void sortBy(strategy strategy);

	static functionPointer getActionWhenAnimationEnded();
	static void setActionWhenAnimationEnded(functionPointer function);
	static void addDefaultUsers();
	static void triggerAction(PlayerAction action);
	static void addTransmission(QJsonObject transmission);
	static void disableAnnoyingSounds();
	static User *getLocalUser();
public slots:
	static void startNetworkDebugger();
	void calculateWinner();
	void showSettings();
	void setMapping(QVector<KeyMapping *> mapping);
	void nextTarget();
	void remakeBoard();
	QUuid getActiveUserID();
	void setActiveUserID(QUuid id);


private:
	NetworkModel * nwModel = nullptr;
	NetworkDebugger * debugger = nullptr;
	static GameControll instance;
	QTimer guideTimer;
	QRandomGenerator * r = nullptr;
	QVector<GuideLine> guideList;
	QVector<User*> users;
	LeaderBoardWidget * leaderboard = nullptr;
	Phase currentPhase = Phase::freeplay; //freeplay
	SettingsDialog * settings = nullptr;
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	QUuid activeUserID;
	QTimer countdown;
	int timeLeft;
	functionPointer actionWhenAnimationEnded = nullptr; //ATTENTION! When you store a new method in this variable, you have to add it in GameControll::toJSON() and GameControll::adaptFromJSON()!!!
	int searchTime = 60;
	QMediaPlayer * player;

	explicit GameControll(QObject *parent = nullptr);
	void sendToServer(PlayerAction a);
	User *getUserById(QUuid id);
	User *getNextUser(QUuid lastUserId);
	int getUserIndexById(QUuid id);
	void resetForNextUser();
	void resetAndNextTarget();
	bool localUserIsActiveUser();
    bool hasSkipped = 0;
	static void updateRandomGenerator(int seed);
	int skipCounter = 0;
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
	void updateSkip(int current, int all);
	void focusBoard();

private slots:
	void updateTimer();
	bool switchPhase(GameControll::Phase phase);
	void exeQTAction(QJsonObject data);
	void sendToServerWithData(PlayerAction a, QJsonObject info);
	void calculateGameStatus();
	void changeBidding(int bidding, QUuid id);
	void nextGuide();
};

#endif // GAMECONTROLL_H
