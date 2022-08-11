#ifndef GAMECONTROLL_H
#define GAMECONTROLL_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QMediaPlayer>
#if QT_VERSION_MAJOR == 6
	#include <QAudioOutput>
#endif
#include "LeaderBoardWidget.h"
#include "user.h"
#include "settingsdialog.h"
#include "keymapping.h"
#include "board.h"
#include "Direction.h"
#include "networkdebugger.h"

enum strategy {points, bid};
/*!
 * \brief The GameControll class macht eigentlich alles
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
		QString line;
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
	static void showGuide(const QStringList & texts);
	static QString getLocalUserName();
	static void initializeConnections();
	static QVector<User*>* getUsers();
	static void addUser(User *user);
	static functionPointer getActionWhenAnimationEnded();
	static void setActionWhenAnimationEnded(functionPointer function);
	static void addDefaultUsers();
	static void clearUsers();
	static void triggerAction(PlayerAction action);
	static void addTransmission(QJsonObject transmission);
	static void disableAnnoyingSounds();
	static User *getLocalUser();
	void sortBy(strategy strategy);
	static int getVoteCounter();
	static int getVoteThreshold();
	void resetAndNextTarget();
	void resetForNextUser();
	void handleUserGivingUp();
	void letUserPlayFree(const QUuid & userId);
	void decideIfUserCanPlayFree(QUuid userId);
	bool localUserIsActiveUser();
	void updateVoteNumbers();
	void setPhase(GameControll::Phase phase);
	bool localUserIsServer();
	bool localUserIsClient();
	static QString phaseAsString(Phase phase);

	void setPlayerInPowerLabel(User *user);
	static void initiateServerSwitch();
	static User *initializeUser(User *user);
	static LeaderBoardWidget* getLeaderboard();
	static int getUserIndexById(QUuid id);

	static bool votekickActive();
	static void requestDisconnect();
public slots:
	static void startNetworkDebugger();
	void calculateWinner();
	void showSettings();
	void setMapping(QVector<KeyMapping *> mapping);
	void nextTarget();
	void remakeBoard();
	QUuid getActiveUserID();
	void setActiveUserID(const QUuid & id);
	static void saveBoard();
	static void loadBoard();

private:
	NetworkModel * nwModel = nullptr;
	NetworkDebugger * debugger = nullptr;
	static GameControll instance;
	QTimer guideTimer;
	QRandomGenerator * r = nullptr;
	QVector<GuideLine> guideList;
	QVector<User*> users;
	LeaderBoardWidget * leaderboard = nullptr;
	Phase currentPhase = Phase::idle; //freeplay
	SettingsDialog * settings = nullptr;
	QVector<KeyMapping*> mapping;
	Board * board = nullptr;
	QUuid activeUserID;
	QTimer countdown;
	int timeLeft;
	functionPointer actionWhenAnimationEnded = nullptr; //ATTENTION! When you store a new method in this variable, you have to add it in GameControll::toJSON() and GameControll::adaptFromJSON()!!!
	int searchTime = 60;
	QMediaPlayer * player;
	#if QT_VERSION_MAJOR == 6
		QAudioOutput* audioOutput = new QAudioOutput();
	#endif

	explicit GameControll(QObject *parent = nullptr);
	void sendToServer(PlayerAction a);
	User *getUserById(QUuid id);
	User *getNextUser(QUuid lastUserId);


	bool boardBlocked = false; // das wird bestimmt keine Probleme machen
	bool hasSkipped = false;
	static void updateRandomGenerator(int seed);
	int voteCounter = 0;
	int voteThreshold = 0;
	void resetVotes();
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
	void enableActionBtn(bool boolean);
	void enableServerSwitchBtn(bool boolean);
	void enableIdleBtn(bool boolean);
	void newBoard(Board* b);
	void updateActionButtonText();
	void focusBoard();
	void updateMoves(int moves);
	void setBoardEnabled(bool enabled);
	void disconnectFromServer();

private slots:
	void updateTimer();
	bool switchPhase(GameControll::Phase phase);
	void exeQTAction(QJsonObject data);
	void sendToServerWithData(PlayerAction a, QJsonObject info);
	void calculateGameStatus();
	void changeBidding(int bidding, QUuid id);
	void nextGuide();
	void syncBoard();
};

#endif // GAMECONTROLL_H
