#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include "client.h"
#include "server.h"

#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QMouseEvent>

class NetworkView : public QWidget
{
	Q_OBJECT
public:
	explicit NetworkView(QWidget *parent = nullptr);
	~NetworkView();
	void toChoiceMenu();
private:

	static QString defaultServer;
	bool allowClientAndServer=false;

	QLayout* layout = nullptr;
	QPushButton* btnClient = nullptr;
	QPushButton* btnServer = nullptr;
	QPushButton* btnOffline = nullptr;

	QWidget* serverContainer = nullptr;
	QVBoxLayout* gridServer = nullptr;
	QLineEdit* leServerAddress = nullptr;
	QLineEdit* leServerPort = nullptr;
	QLabel* serverStatus = nullptr;
	QLineEdit* leMessageToClients = nullptr;

	QWidget* clientContainer = nullptr;
	QVBoxLayout* gridClient = nullptr;
	QLineEdit* leClientAddress = nullptr;
	QLineEdit* leClientPort = nullptr;
	QLabel* clientStatus = nullptr;
	QLineEdit* leMessageToServer = nullptr;

signals:
	void leaderboardOnline();
	void leaderboardOffline();

private slots:
	void addServer();
	void addClient();

};

#endif // NETWORKVIEW_H
