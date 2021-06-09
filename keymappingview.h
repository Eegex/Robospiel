#ifndef KEYMAPPINGVIEW_H
#define KEYMAPPINGVIEW_H

#include "keyinput.h"
#include "keymapping.h"

#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QVector>
#include <QScrollArea>
#include <QWidget>

class KeyMappingView : public QWidget
{
	Q_OBJECT
public:
	KeyMappingView(QVector<KeyMapping*> mappings = QVector<KeyMapping*>(), QWidget *parent=nullptr);
	QVector<KeyMapping *> getMapping();

private:
	PlayerAction mappableActions[14] = {PlayerAction::movePlayerNorth,
										PlayerAction::movePlayerWest,
										PlayerAction::movePlayerSouth,
										PlayerAction::movePlayerEast,

										PlayerAction::switchPlayerNorth,
										PlayerAction::switchPlayerWest,
										PlayerAction::switchPlayerSouth,
										PlayerAction::switchPlayerEast,

										PlayerAction::enterBidding,
										PlayerAction::sendBidding,
										PlayerAction::clearBidding,
										PlayerAction::giveUp,
										PlayerAction::revert,
										PlayerAction::revertToBeginning};
	QVector<KeyMapping*> allMappings;
	QGridLayout * grid = nullptr;
	QVBoxLayout * lay = nullptr;
	QVector<QLabel*> mappingLabels;
	QVector<QVector<QLabel*>*> keyLabels;
	QVector<QHBoxLayout*> hBoxRows;
	QMap<int, QPushButton*> addBtns;
	QScrollArea * scroll = nullptr;
	bool constructorHasEnded =false;

	void completeMappings(QVector<KeyMapping*> mappings);
	QVBoxLayout *getAddGroup(int row);
	void insertKeyIntoUI(int row, int col);

	void checkMappings();
signals:
	void keyMappingChanged();
	void addBtnPressed(int index);
	void newValidKeyMappings(QVector<KeyMapping*> mappings); //not used at the moment, because saving happens explicitly by clicking a button
};

#endif // KEYMAPPINGVIEW_H
