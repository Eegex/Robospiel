#ifndef KEYINPUT_H
#define KEYINPUT_H

#include <QComboBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMetaEnum>

class KeyInput : public QWidget
{
	Q_OBJECT


	void keyReleaseEvent(QKeyEvent *event);
	QLineEdit * input = nullptr;
	static QMetaEnum metaEnum;
public:
	explicit KeyInput(QWidget *parent = nullptr);
	static int inputWidth;

	Qt::Key getKey();
	bool hasKey();
	void reset();
	static Qt::Key stringToKey(QString string);
	static QString keyToString(Qt::Key key);
signals:

};

#endif // KEYINPUT_H
