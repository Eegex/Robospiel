#ifndef USERVIEW_H
#define USERVIEW_H

#include "LeaderBoardWidget.h"
#include "offlineleaderboardwidget.h"
#include "onlineleaderboardwidget.h"
#include <QWidget>
#include "networkview.h"
#include <QAction>

/*!
 * \brief The UserView class is a Container for NetworkView and online/offline-LeaderboardWidget
 */
class UserView : public QWidget
{
	Q_OBJECT
public:
	explicit UserView(QPushButton *actionBtn, QAction *serverSwitchBtn, QWidget *parent = nullptr);
	LeaderBoardWidget *getLeaderboard() const;

	static void disconnectFromServer();
private:
	static UserView* instance;
	LeaderBoardWidget* leaderboard = nullptr;
	NetworkView* network = nullptr;
	QGridLayout* layout = nullptr;
	QPushButton* btnBack = new QPushButton(tr("Back"));

signals:

};

#endif // USERVIEW_H
