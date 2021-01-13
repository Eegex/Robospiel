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

class NetworkView : public QWidget
{
	Q_OBJECT
public:
	explicit NetworkView(QWidget *parent = nullptr);
	~NetworkView();
private:
    void toChoiceMenu();

	bool allowClientAndServer=false;

    QLayout* layout;
    QPushButton* btnClient;
    QPushButton* btnServer;
    QPushButton* btnOffline;

	QWidget* serverContainer;
	QGridLayout* gridServer;
	QLineEdit* leServerAddress;
	QLineEdit* leServerPort;
    QLabel* serverStatus;
	QLineEdit* leMessageToClients;

	QWidget* clientContainer;
	QGridLayout* gridClient;
	QLineEdit* leClientAddress;
	QLineEdit* leClientPort;
	QLabel* clientStatus;
	QLineEdit* leMessageToServer;

signals:
    void leaderboradOnline();
    void leaderboradOffline();

private slots:
	void addServer();
	void addClient();
//	void sendToClients();
//    void sendToServer();

};

#endif // NETWORKVIEW_H
