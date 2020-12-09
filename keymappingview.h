#ifndef KEYMAPPINGVIEW_H
#define KEYMAPPINGVIEW_H

#include "keymapping.h"

#include <QGridLayout>
#include <QMap>
#include <QPushButton>
#include <QVector>
#include <QWidget>

class KeyMappingView : public QWidget
{
    Q_OBJECT
public:
    KeyMappingView(QWidget *parent=nullptr, QVector<KeyMapping*> mappings = QVector<KeyMapping*>());
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
    QMap<int, QPushButton*> addBtns;

    Qt::Key getFreeKey(QVector<Qt::Key> *usedKeys);
    Qt::Key getFreeKey();
    void completeMappings(QVector<KeyMapping*> mappings);
    QPushButton* getAddBtn(int row);

    void checkMappings();
signals:
    void keyMappingChanged();
    void addBtnPressed(int index);

private slots:
    void addKeyToMapping(int index);
};

#endif // KEYMAPPINGVIEW_H
