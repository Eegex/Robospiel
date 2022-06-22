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
#include "userview.h"

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget *parent = nullptr);
	void setMenuBar(QMenuBar * bar);
	void enableActionBtn(bool boolean);
	void enableServerSwitchBtn(bool boolean);
	void enableIdle(bool boolean);
	void updateActionBtnText();
	void handleActionButtonRelease();
	void handleServerSwitch();

protected slots:
	void keyPressEvent(QKeyEvent * event);
	void handleKeyPress(int key);

private slots:
	void updateTimer(int remaining);
	void editBoard();
	void updatePlayerMaximum(int i);
	void createBoard();
	void updateGuide(const QString & txt);
	void enableMenus(bool boolean);
	void enableBoard(bool enabled);
	void focusBoard();

private:
	QLabel * dlGuide = new QLabel(this);
	QLabel * dlSteps = new QLabel(this);
	QGridLayout * glMain = nullptr;
	BoardView * view = nullptr;
	BoardEditor * edit = nullptr;
	UserView * userView = nullptr;
	QLCDNumber * lcd = nullptr;
	QPushButton * actionBtn = nullptr; //aka. Vote-Btn
	QPushButton * serverSwitchBtn = nullptr;
	QMenuBar * menuBar = nullptr;
	QAction * aNetworking = nullptr;
	QAction * aEditBoard = nullptr;
	QAction * aNextTarget = nullptr;
	QAction * aResetPoints = nullptr;
	QAction * aSettings = nullptr;
	QAction * aDebugger = nullptr;
	QMenu * mNewGame = nullptr;
	QMenu * mNewStuff = nullptr;
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

	QMap<GameControll::Phase, QString> actionBtnTexts;

	void initializeView(Board *b, QVector<KeyMapping *> *m);
	void connectView(BoardView * view);

	bool tooltipDir = false;

	//int testCounter=0;
};

#endif // MAINWIDGET_H
