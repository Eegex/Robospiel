#include "keyinput.h"

#include <QMetaEnum>
#include <QKeyEvent>

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
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    Qt::Key key = static_cast<Qt::Key>(event->key());
    QString asString = metaEnum.valueToKey(key);
    input->setText(asString.remove(0, 4));
}

bool KeyInput::hasKey()
{
    return input->text()!="" && getKey()!=Qt::Key_unknown && getKey()!=Qt::Key(0);
}

Qt::Key KeyInput::getKey()
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    QString name = "Key_"+input->text();
    Qt::Key key = static_cast<Qt::Key>(metaEnum.keyToValue(name.toStdString().c_str()));
    return key;
}

void KeyInput::reset()
{
    input->setText("");
}






