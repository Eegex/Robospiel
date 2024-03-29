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
#include <QSpacerItem>
//#include <QSplitter>
#include "user.h"
#include "board.h"
#include "boardeditor.h"
#include "settingsdialog.h"
#include "gamecontroll.h"
#include "LeaderBoardWidget.h"
#include "stackwidget.h"

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget *parent = nullptr);
	void setMenuBar(QMenuBar * bar);
	void enableServerSwitchBtn(bool boolean);
	void enableIdle(bool boolean);
	void handleServerSwitch();


protected slots:
	void keyPressEvent(QKeyEvent * event);

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

	QGridLayout * glMain = nullptr;
	BoardView * view = nullptr;
	BoardEditor * edit = nullptr;
	QLCDNumber * lcd = nullptr;
	StackWidget* stack = nullptr;
	QAction * aServerSwitch = nullptr;
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
	QAction * aNewPlayers = nullptr;
	QAction * aNewAll = nullptr;
	QAction * aGoToIdle = nullptr;

	QMenu * mImportExport = nullptr;
	QAction * aSaveBoard = nullptr;
	QAction * aLoadBoard = nullptr;

	QSpacerItem* topSpacer = nullptr;
	QSpacerItem* bottomSpacer = nullptr;

	void initializeView(Board *b, QVector<KeyMapping *> *m);
	void connectView(BoardView * view);


	//int testCounter=0;
};

#endif // MAINWIDGET_H
