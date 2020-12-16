#ifndef KEYMAPPINGVIEW_H
#define KEYMAPPINGVIEW_H

#include "keyinput.h"
#include "keymapping.h"

#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QVector>
#include <QWidget>

class KeyMappingView : public QWidget
{
    Q_OBJECT
public:
    KeyMappingView(QVector<KeyMapping*> mappings = QVector<KeyMapping*>(), QWidget *parent=nullptr);
    QVector<KeyMapping *> getMapping();
private:
    KeyMapping stdMappings[13] = {KeyMapping(PlayerAction::movePlayerNorth, Qt::Key::Key_W),
                                  KeyMapping(PlayerAction::movePlayerWest, Qt::Key::Key_A),
                                  KeyMapping(PlayerAction::movePlayerSouth, Qt::Key::Key_S),
                                  KeyMapping(PlayerAction::movePlayerEast, Qt::Key::Key_D),

                                  KeyMapping(PlayerAction::switchPlayerNorth, Qt::Key::Key_Up),
                                  KeyMapping(PlayerAction::switchPlayerWest, Qt::Key::Key_Left),
                                  KeyMapping(PlayerAction::switchPlayerSouth, Qt::Key::Key_Down),
                                  KeyMapping(PlayerAction::switchPlayerEast, Qt::Key::Key_Right),

                                  KeyMapping(PlayerAction::enterBidding, Qt::Key::Key_Space),
                                  KeyMapping(PlayerAction::sendBidding, Qt::Key::Key_Enter),
                                  KeyMapping(PlayerAction::clearBidding, Qt::Key::Key_Backspace),
                                  KeyMapping(PlayerAction::giveUp, Qt::Key::Key_G),
                                  KeyMapping(PlayerAction::revert, Qt::Key::Key_Z)};
    QVector<KeyMapping*> allMappings;
    QGridLayout* grid;
    QVector<QLabel*> mappingLabels;
    QVector<QVector<QLabel*>*> keyLabels;
    QVector<QHBoxLayout*> hBoxRows;
    QMap<int, QPushButton*> addBtns;
    bool constructorHasEnded =false;

    Qt::Key getFreeKey(QVector<Qt::Key> *usedKeys);
    void completeMappings(QVector<KeyMapping*> mappings);
    QVBoxLayout *getAddGroup(int row);
    void insertKeyIntoUI(int row, int col);

    void checkMappings();
signals:
    void keyMappingChanged();
    void addBtnPressed(int index);
    void newValidKeyMappings(QVector<KeyMapping*> mappings);
};

#endif // KEYMAPPINGVIEW_H
