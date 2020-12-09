#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "board.h"
#include "boardview.h"
#include "keymappingview.h"
#include "networkview.h"

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget *parent = nullptr);
	void setMenuBar(QMenuBar * bar);

signals:

private:
	QGridLayout * glMain = nullptr;
	Board * board = nullptr;
	BoardView * view = nullptr;
	NetworkView * networkView = nullptr;
    KeyMappingView* keyMappingView = nullptr;
	QMenuBar * menuBar = nullptr;
	QAction * aNetworking = nullptr;
	QAction * aNewBoard = nullptr;
	QAction * aNewTarget = nullptr;
};

#endif // MAINWIDGET_H
