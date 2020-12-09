#include "keyinput.h"

#include <QMetaEnum>

KeyInput::KeyInput(Qt::Key key, QWidget *parent) : QWidget(parent)
{
    QVector<QString> possibleKeys;
    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    for(int i=0; i<metaEnum.keyCount(); i++)
    {
        possibleKeys.append(QKeySequence(metaEnum.value(i)).toString());
    }

    layout = new QVBoxLayout(this);
    selection = new QComboBox(this);
    selection->addItems(possibleKeys.toList());
    selection->setCurrentText(QKeySequence(key).toString());
    connect(selection, &QComboBox::currentTextChanged, this, [=]()->void{
        emit changedKey(selection->currentText());
    });
    layout->addWidget(selection);

    deleteBtn = new QPushButton(tr("Delete"), this);
    connect(deleteBtn, &QAbstractButton::pressed, this, [=]()->void{
        emit deletedKey(selection->currentText());
        this->deleteLater();
    });
    layout->addWidget(deleteBtn);



}
