#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QLCDNumber>
#include <QPalette>
#include <QUuid>
#include <QWidgetAction>
#include <QAction>
#include "user.h"
#include "board.h"
#include "boardeditor.h"
#include "networkview.h"
#include "settingsdialog.h"
#include "gamecontroll.h"
#include "LeaderBoardWidget.h"

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget *parent = nullptr);
	void setMenuBar(QMenuBar * bar);
	void addExistingUser(User *user);
private slots:
	void updateTimer(int remaining);
	void changeBidding(int bidding, QUuid id);
	void changeOnlyBidding(int bidding);
	void addUser(struct UserData * newUser);
    void calculateGameStatus(PlayerAction action);
    void calculateNewPlayer(int moves);
	void editBoard();
	void createBoard();
private:
	QVector<User*> users;
    int currentMoves = 0; //What the fuck
	QGridLayout * glMain = nullptr;
	GameControll * game = nullptr;
	BoardView * view = nullptr;
	BoardEditor * edit = nullptr;
	NetworkView * networkView = nullptr;
	LeaderBoardWidget * leaderboard = nullptr;
	QLCDNumber * lcd = nullptr;
	QMenuBar * menuBar = nullptr;
	QAction * aNetworking = nullptr;
	QAction * aEditBoard = nullptr;
    QAction * aNextTarget = nullptr;
	QAction * aSettings = nullptr;
    QMenu * mNewGame = nullptr;
    QMenu *mNewStuff = nullptr;
	QWidgetAction * waHeight = nullptr;
	QSpinBox * sbHeight = nullptr;
	QWidgetAction * waWidth = nullptr;
	QSpinBox * sbWidth = nullptr;
	QWidgetAction * waPlayer = nullptr;
	QSpinBox * sbPlayer = nullptr;
	QAction * aNewBoard = nullptr;

    QAction * aNewWalls = nullptr;
    QAction * aNewSeeker = nullptr;
    QAction * aNewPlayers = nullptr;
    QAction * aNewAll = nullptr;
    QAction * aNewTarget = nullptr;
};

#endif // MAINWIDGET_H
