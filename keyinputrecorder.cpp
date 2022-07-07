#include "keyinputrecorder.h"

#include <QKeyEvent>

QMetaEnum KeyInputRecorder::metaEnum = QMetaEnum::fromType<Qt::Key>();

/**
 * @brief KeyInputRecorder::stringToKey convert enum to its name
 * @param string has to be a string that matches an enum name (without the "Key_" prefix
 * @return the corresponding enum value
 */
Qt::Key KeyInputRecorder::stringToKey(QString string)
{
	QString name = "Key_"+string;
	Qt::Key key = static_cast<Qt::Key>(metaEnum.keyToValue(name.toStdString().c_str()));
	return key;
}

/**
 * @brief KeyInputRecorder::keyToString
 * @param key
 * @return the appropriate name to the enum (without the "Key_" prefix)
 */
QString KeyInputRecorder::keyToString(Qt::Key key)
{
	QString asString = metaEnum.valueToKey(key);
	asString.remove(0, 4);
	return asString;
}

//modified QLineEdit to capture key events
KeyInputRecorder::KeyInputRecorder(QWidget* parent) : QLineEdit(parent)
{
	setPlaceholderText(tr("Enter the key! "));

	QString maxString=placeholderText();
	for(int i = 0; i<metaEnum.keyCount(); i++)
	{
		QString name = keyToString(static_cast<Qt::Key>(metaEnum.value(i)));
		if(name.length()>maxString.length())
		{
			maxString=name;
		}
	}

	if(inputWidth==-1)
	{
		QFontMetrics* fontInfo = new QFontMetrics(font());
		inputWidth =fontInfo->boundingRect(maxString+"...").width();
	}
}

void KeyInputRecorder::keyReleaseEvent(QKeyEvent* event)
{
	Qt::Key key = static_cast<Qt::Key>(event->key());
	currentKey = key;
	setText(keyToString(currentKey));
}

Qt::Key KeyInputRecorder::getKey()
{
	return currentKey;
}

void KeyInputRecorder::reset()
{
	currentKey = Qt::Key_unknown;
	setText("");
}

int KeyInputRecorder::getInputWidth()
{
	return inputWidth;
}
