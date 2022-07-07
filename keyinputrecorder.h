#ifndef KEYINPUTRECORDER_H
#define KEYINPUTRECORDER_H

#include <QLineEdit>
#include <QMetaEnum>

class KeyInputRecorder : public QLineEdit
{
	Q_OBJECT
public:
	KeyInputRecorder(QWidget* parent = nullptr);
	Qt::Key getKey();
	void reset();

	int getInputWidth();

	static Qt::Key stringToKey(QString string);
	static QString keyToString(Qt::Key key);

protected:
	void keyReleaseEvent(QKeyEvent* event);
private:
	Qt::Key currentKey = Qt::Key::Key_0;
	int inputWidth = -1;


	static QMetaEnum metaEnum;

};

#endif // KEYINPUTRECORDER_H
