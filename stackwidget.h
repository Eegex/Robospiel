#ifndef STACKWIDGET_H
#define STACKWIDGET_H

#include "LeaderBoardWidget.h"

#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QWidget>

class StackWidget : public QWidget
{
	Q_OBJECT
public:
	explicit StackWidget(QWidget *parent = nullptr);
	QSize sizeHint() const;
private:
	QIntValidator intValidator;
	QString defaultServer = "localhost";
	int defaultPort = 8050;


	QVBoxLayout* layout = nullptr;
	QStackedLayout* stack = nullptr;

	QSizePolicy btnSizePolicy;
	QPushButton* backButton = nullptr;

	//Network Auswahl
	void createNetworkSelection();
	QVBoxLayout* networkSelection = nullptr;
	QPushButton* btnClient = nullptr;
	QPushButton* btnServer = nullptr;
	QPushButton* btnOffline = nullptr;

	//start client
	void createClientLayout();
	QVBoxLayout* clientLayout = nullptr;
	QLineEdit* leClientAddress = nullptr;
	QLineEdit* leClientPort = nullptr;
	QLabel* clientStatus = nullptr;
	QLineEdit* leMessageToServer = nullptr;
	QPushButton* btnStartClient = nullptr;

	//start server
	void createServerLayout();
	QVBoxLayout* serverLayout = nullptr;
	QLineEdit* leServerAddress = nullptr;
	QLineEdit* leServerPort = nullptr;
	QLabel* serverStatus = nullptr;
	QLineEdit* leMessageToClients = nullptr;
	QPushButton* btnStartServer = nullptr;

	//Leaderboard
	LeaderBoardWidget* leaderboard = nullptr;
	void handleLeaderboardOnline();
	void handleLeaderboardOffline();
	void disconnectFromServer();

};

#endif // STACKWIDGET_H
