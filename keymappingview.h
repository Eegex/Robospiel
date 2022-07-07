#ifndef KEYMAPPINGVIEW_H
#define KEYMAPPINGVIEW_H

#include "keyinputblock.h"
#include "keyinputrecorder.h"
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
	PlayerAction mappableActions[12] = { PlayerAction::movePlayerNorth,
										PlayerAction::movePlayerWest,
										PlayerAction::movePlayerSouth,
										PlayerAction::movePlayerEast,

										PlayerAction::switchPlayerNorth,
										PlayerAction::switchPlayerWest,
										PlayerAction::switchPlayerSouth,
										PlayerAction::switchPlayerEast,

										PlayerAction::giveUp,
										PlayerAction::revert,
										PlayerAction::revertToBeginning,
										PlayerAction::vote
									   };
	QVector<KeyMapping*> allMappings;
	QGridLayout * grid = nullptr;
	QVBoxLayout * lay = nullptr;
	QVector<QLabel*> mappingLabels;
	QVector<QVector<QLabel*>*> keyLabels;
	QVector<QHBoxLayout*> hBoxRows;
	QVector<KeyInputBlock*> inputBlocks;
	QScrollArea * scroll = nullptr;
	bool constructorHasEnded =false;

	void completeMappings(QVector<KeyMapping*> mappings);
	KeyInputBlock* getAddGroup(int row);
	void insertKeyIntoUI(int row, int col);

	void checkMappings();
signals:
	void keyMappingChanged();
	void addBtnPressed(int index);
	void newValidKeyMappings(QVector<KeyMapping*> mappings); //not used at the moment, because saving happens explicitly by clicking a button
};

#endif // KEYMAPPINGVIEW_H
