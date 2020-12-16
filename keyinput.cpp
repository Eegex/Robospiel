#include "keyinput.h"

#include <QMetaEnum>
#include <QKeyEvent>

Qt::Key KeyInput::stringToKey(QString string)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    QString name = "Key_"+string;
    Qt::Key key = static_cast<Qt::Key>(metaEnum.keyToValue(name.toStdString().c_str()));
    return key;
}

QString KeyInput::keyToString(Qt::Key key)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    QString asString = metaEnum.valueToKey(key);
    asString.remove(0, 4);
    return asString;
}

KeyInput::KeyInput(QWidget *parent) : QWidget(parent)
{
    input = new QLineEdit(this);
    input->setPlaceholderText(tr("Enter the key! "));
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(input);
    setLayout(layout);
}

void KeyInput::keyReleaseEvent(QKeyEvent* event)
{

    Qt::Key key = static_cast<Qt::Key>(event->key());

    input->setText(KeyInput::keyToString(key));
}

bool KeyInput::hasKey()
{
    return input->text()!="" && getKey()!=Qt::Key_unknown && getKey()!=Qt::Key(0);
}

Qt::Key KeyInput::getKey()
{
    return KeyInput::stringToKey(input->text());
}

void KeyInput::reset()
{
    input->setText("");
}






