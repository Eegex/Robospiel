#include "keymappingview.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMetaEnum>
#include <QPushButton>
#include <QDebug>
#include <QSet>

KeyMappingView::KeyMappingView(QWidget *parent, QVector<KeyMapping*> mappings) : QWidget(parent)
{
    completeMappings(mappings);

    grid = new QGridLayout(this);
    for(int i=0; i<allMappings.length(); i++)
    {
        QLabel* name = new QLabel(this);
        labels.append(name);
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
        QVector<KeyInput*> innerVector;
        for(int j=0; j<allMappings.at(i)->getKeys().length(); j++)
        {
            KeyInput* input = new KeyInput(allMappings.at(i)->getKeys().at(j));
            innerVector.append(input);
            connect(input, &KeyInput::deletedKey, this, [=](QString selection)->void{
                allMappings.at(i)->removeKeyByIndex(j);
                checkMappings();
            });
            connect(input, &KeyInput::changedKey, this, [=](QString selection)->void{
                QKeySequence sequence = QKeySequence::fromString(selection);
                int enumAsInt = sequence[0];
                allMappings.at(i)->replaceKeyAt(j, static_cast<Qt::Key>(enumAsInt));
                checkMappings();
            });

            grid->addWidget(input, i, j+1);
        }
        inputs.append(innerVector);
        grid->addWidget(getAddBtn(i), i, allMappings.at(i)->getKeys().length()+1);

    }
    setLayout(grid);
    setWindowTitle(tr("Key Mappings"));
    connect(this, SIGNAL(addBtnPressed(int)), this, SLOT(addKeyToMapping(int)));

    constructorHasEnded=true;
    checkMappings();
}

void KeyMappingView::addKeyToMapping(int index)
{
    KeyMapping* m = allMappings.at(index);
    Qt::Key key = getFreeKey();
    bool keyWasNew = m->addKey(key);
    if(keyWasNew)
    {

        grid->removeWidget(addBtns.value(index));
        KeyInput* input = new KeyInput(key, this);
        grid->addWidget(input, index, allMappings.at(index)->getKeys().length());

        grid->addWidget(addBtns.value(index), index, allMappings.at(index)->getKeys().length()+1);
        checkMappings();
    }


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
                    qDebug()<<"found duplicate";
                    valid = false;
                    rowValid = false;
                    for(int k=0; k<i; k++)
                    {
                        for(int l=0; l<allMappings.at(k)->getKeys().length(); l++)
                        {
                            if(allMappings.at(i)->getKeys().at(j)==allMappings.at(k)->getKeys().at(l))
                            {
                                labels.at(i)->setStyleSheet("QLabel { color : red; font-weight: bold;}");
                                labels.at(k)->setStyleSheet("QLabel { color : red; font-weight: bold;}");
                                inputs.at(i).at(j)->setStyleSheet("QComboBox { color : red; }");
                                inputs.at(k).at(l)->setStyleSheet("QComboBox { color : red; }");

                            }
                        }
                    }
                }
                else
                {
                    usedKeys.insert(allMappings.at(i)->getKeys().at(j));
                    inputs.at(i).at(j)->setStyleSheet("");
                }
            }
            if(rowValid)
            {
                labels.at(i)->setStyleSheet("");
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

Qt::Key KeyMappingView::getFreeKey()
{
    QVector<Qt::Key> usedKeys;
    for(KeyMapping* mapping:allMappings)
    {
        usedKeys.append(mapping->getKeys());
    }
    return getFreeKey(&usedKeys);
}

QPushButton* KeyMappingView::getAddBtn(int row)
{
    QPushButton* btnAddKey = new QPushButton(tr("Add"), this);
    connect(btnAddKey, &QAbstractButton::pressed, this, [=]()->void{
        emit addBtnPressed(row);
    });
    addBtns.insert(row, btnAddKey);
    return btnAddKey;
}
