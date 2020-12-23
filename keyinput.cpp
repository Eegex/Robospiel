#include "keyinput.h"

#include <QMetaEnum>
#include <QKeyEvent>

QMetaEnum KeyInput::metaEnum = QMetaEnum::fromType<Qt::Key>();
int KeyInput::inputWidth=-1;
Qt::Key KeyInput::stringToKey(QString string)
{
    QString name = "Key_"+string;
    Qt::Key key = static_cast<Qt::Key>(metaEnum.keyToValue(name.toStdString().c_str()));
    return key;
}

QString KeyInput::keyToString(Qt::Key key)
{
    QString asString = metaEnum.valueToKey(key);
    asString.remove(0, 4);
    return asString;
}

KeyInput::KeyInput(QWidget *parent) : QWidget(parent)
{
    input = new QLineEdit(this);
    input->setPlaceholderText(tr("Enter the key! "));
    QString maxString=input->placeholderText();
    for(int i = 0; i<KeyInput::metaEnum.keyCount(); i++)
    {
        QString name = KeyInput::keyToString(static_cast<Qt::Key>(KeyInput::metaEnum.value(i)));
        if(name.length()>maxString.length())
        {
            maxString=name;
        }
    }

    if(KeyInput::inputWidth==-1)
    {
        QFontMetrics* fontInfo = new QFontMetrics(input->font());
        KeyInput::inputWidth =fontInfo->boundingRect(maxString+"...").width();
    }

    input->setFixedWidth(KeyInput::inputWidth);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
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






