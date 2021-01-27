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
    void enableTimerSkip(bool boolean);
private slots:
	void updateTimer(int remaining);
	void editBoard();
	void createBoard();
	void updateGuide(const QString & txt);
    void enableMenus(bool boolean);
private:
	QLabel * dlGuide = new QLabel(this);
	QGridLayout * glMain = nullptr;
	GameControll * game = nullptr;
	BoardView * view = nullptr;
	BoardEditor * edit = nullptr;
	NetworkView * networkView = nullptr;
	LeaderBoardWidget * leaderboard = nullptr;
	QLCDNumber * lcd = nullptr;
    QPushButton * skipBtn = nullptr;
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
    QAction * aGoToIdle = nullptr;

};

#endif // MAINWIDGET_H
