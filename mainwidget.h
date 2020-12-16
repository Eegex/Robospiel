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
#include "board.h"
#include "boardview.h"
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

signals:

private slots:
	void updateTimer(int remaining);
private:
	QGridLayout * glMain = nullptr;
	GameControll * game = nullptr;
	BoardView * view = nullptr;
	SettingsDialog * settings = nullptr;
	NetworkView * networkView = nullptr;
	LeaderBoardWidget * leaderboard = nullptr;
	QLCDNumber * lcd = nullptr;
	QMenuBar * menuBar = nullptr;
	QAction * aNetworking = nullptr;
	QAction * aNewBoard = nullptr;
	QAction * aNewTarget = nullptr;
	QAction * aSettings = nullptr;
};

#endif // MAINWIDGET_H
