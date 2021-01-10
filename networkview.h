#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include "client.h"
#include "server.h"

#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

class NetworkView : public QWidget
{
	Q_OBJECT
public:
	explicit NetworkView(QWidget *parent = nullptr);
	~NetworkView();
private:

	bool allowClientAndServer=false;

	QGridLayout* layout;
	QButtonGroup* choiceGroup;
	QRadioButton* btnServer;
	QWidget* serverContainer;
	QGridLayout* gridServer;
	QLineEdit* leServerAddress;
	QLineEdit* leServerPort;
	QLabel* serverStatus;
	QLineEdit* leMessageToClients;

	QRadioButton* btnClient;
	QWidget* clientContainer;
	QGridLayout* gridClient;
	QLineEdit* leClientAddress;
	QLineEdit* leClientPort;
	QLabel* clientStatus;
	QLineEdit* leMessageToServer;

signals:
    void leaderboradOnline();

private slots:
	void addServer();
	void addClient();
	void sendToClients();
	void sendToServer();
};

#endif // NETWORKVIEW_H
