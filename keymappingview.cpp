#include "keymappingview.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMetaEnum>
#include <QPushButton>
#include <QDebug>
#include <QSet>
#include <QTimer>

KeyMappingView::KeyMappingView(QWidget *parent, QVector<KeyMapping*> mappings) : QWidget(parent)
{
    completeMappings(mappings);

    grid = new QGridLayout(this);
    for(int i=0; i<allMappings.length(); i++)
    {
        QLabel* name = new QLabel(this);
        mappingLabels.append(name);
        switch(allMappings.at(i)->getAction())
        {
            case PlayerAction::movePlayerNorth:
                name->setText(tr("move up"));
                break;
            case PlayerAction::movePlayerEast:
                name->setText(tr("move right"));
                break;
            case PlayerAction::movePlayerSouth:
                name->setText(tr("move down"));
                break;
            case PlayerAction::movePlayerWest:
                name->setText(tr("move right"));
                break;
            case PlayerAction::switchPlayerNorth:
                name->setText(tr("select player above the current"));
                break;
            case PlayerAction::switchPlayerEast:
                name->setText(tr("select player to the right of the current"));
                break;
            case PlayerAction::switchPlayerSouth:
                name->setText(tr("select player below the current"));
                break;
            case PlayerAction::switchPlayerWest:
                name->setText(tr("select player to the left of the current"));
                break;
            case PlayerAction::enterBidding:
                name->setText(tr("enter bidding (focus the input field)"));
                break;
            case PlayerAction::sendBidding:
                name->setText(tr("send bidding"));
                break;
            case PlayerAction::clearBidding:
                name->setText(tr("delete bidding"));
                break;
            case PlayerAction::giveUp:
                name->setText(tr("give up"));
                break;
            case PlayerAction::revert:
                name->setText(tr("undo the last move"));
                break;
            default:
                name->setText(tr("Sorry, something went wrong :("));
        }
        grid->addWidget(name, i, 0);
        QHBoxLayout* rowContainer = new QHBoxLayout();
        hBoxRows.append(rowContainer);
        QVector<QLabel*>* innerVector = new QVector<QLabel*>();
        keyLabels.append(innerVector);
        for(int j=0; j<allMappings.at(i)->getKeys().length(); j++)
        {
            insertKeyIntoUI(i, j);
        }

        grid->addLayout(rowContainer, i, 1);
        grid->addLayout(getAddGroup(i), i, 2);

    }
    setLayout(grid);
    setWindowTitle(tr("Key Mappings"));

    constructorHasEnded=true;
    checkMappings();
}

void KeyMappingView::completeMappings(QVector<KeyMapping*> mappings)
{
    QVector<Qt::Key> usedKeys;
    for(KeyMapping* mapping:mappings)
    {
        usedKeys.append(mapping->getKeys());
    }
    for(KeyMapping mapping: stdMappings)
    {
        bool exists =false;
        for(int i=0; i<mappings.length(); i++)
        {
            if(mappings.at(i)->getAction()==mapping.getAction())
            {
                exists=true;
                allMappings.append(new KeyMapping(*mappings.takeAt(i)));
                break;
            }
        }
        if(!exists)
        {
            if(usedKeys.contains(mapping.getKeys().at(0)))
            {
                allMappings.append(new KeyMapping(mapping.getAction(), getFreeKey(&usedKeys)));
            }
            else
            {
                allMappings.append(new KeyMapping(mapping));
            }
        }
    }
    checkMappings();

}

void KeyMappingView::checkMappings()
{
    if(constructorHasEnded)
    {
        bool valid = true;
        QSet<Qt::Key> usedKeys;
        for(int i=0; i<allMappings.length(); i++)
        {
            bool rowValid = true;
            for(int j=0; j<allMappings.at(i)->getKeys().length(); j++)
            {
                if(usedKeys.contains(allMappings.at(i)->getKeys().at(j)))
                {
                    valid = false;
                    rowValid = false;
                    for(int k=0; k<i; k++)
                    {
                        for(int l=0; l<allMappings.at(k)->getKeys().length(); l++)
                        {
                            if(allMappings.at(i)->getKeys().at(j)==allMappings.at(k)->getKeys().at(l))
                            {
                                mappingLabels.at(i)->setStyleSheet("QLabel { color : red; font-weight: bold;}");
                                mappingLabels.at(k)->setStyleSheet("QLabel { color : red; font-weight: bold;}");
                                keyLabels.at(i)->at(j)->setStyleSheet("QLabel { color : red; font-weight: bold;}");
                                keyLabels.at(k)->at(l)->setStyleSheet("QLabel { color : red; font-weight: bold;}");

                            }
                        }
                    }
                }
                else
                {
                    usedKeys.insert(allMappings.at(i)->getKeys().at(j));
                    keyLabels.at(i)->at(j)->setStyleSheet("");
                }
            }
            if(rowValid)
            {
                mappingLabels.at(i)->setStyleSheet("");
            }
        }
        if(valid)
        {
            emit newValidKeyMappings(allMappings);
        }
    }
}

Qt::Key KeyMappingView::getFreeKey(QVector<Qt::Key>* usedKeys)
{
    //letters
    for(int i=65; i<=90; i++)
    {
        Qt::Key key = static_cast<Qt::Key>(i);
        if(!usedKeys->contains(key))
        {
            usedKeys->append(key);
            return key;
        }
    }

    //numbers
    for(int i=48; i<=57; i++)
    {
        Qt::Key key = static_cast<Qt::Key>(i);
        if(!usedKeys->contains(key))
        {
            usedKeys->append(key);
            return key;
        }
    }

    return static_cast<Qt::Key>(65);
}

QVBoxLayout* KeyMappingView::getAddGroup(int row)
{
    QVBoxLayout* layout = new QVBoxLayout();
    KeyInput* input = new KeyInput();


    QPushButton* btnAddKey = new QPushButton(tr("Add"), this);
    connect(btnAddKey, &QAbstractButton::pressed, this, [=]()->void{

        if(input->hasKey() && allMappings.at(row)->addKey(input->getKey()))
        {
            insertKeyIntoUI(row, allMappings.at(row)->getKeys().length()-1);
            checkMappings();
        }
        input->reset();

    });
    addBtns.insert(row, btnAddKey);
    layout->addWidget(input);
    layout->addWidget(btnAddKey);
    return layout;
}

void KeyMappingView::insertKeyIntoUI(int row, int col)
{
    QVBoxLayout* layout = new QVBoxLayout();

    QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Key>();
    Qt::Key key = static_cast<Qt::Key>(allMappings.at(row)->getKeys().at(col));
    QString asString = metaEnum.valueToKey(key);
    asString.remove(0, 4);
    QLabel* label = new QLabel(asString, this);
    keyLabels.at(row)->append(label);
    layout->addWidget(label);

    QPushButton* deleteBtn = new QPushButton(tr("Delete"), this);
    connect(deleteBtn, &QAbstractButton::pressed, this, [=]()->void{
        keyLabels.at(row)->removeAll(label);

        layout->deleteLater();
        label->deleteLater();
        deleteBtn->deleteLater();

        allMappings.at(row)->removeKey(static_cast<Qt::Key>(QKeySequence(label->text())[0]));
        checkMappings();
    });
    layout->addWidget(deleteBtn);

    hBoxRows.at(row)->addLayout(layout);
}
