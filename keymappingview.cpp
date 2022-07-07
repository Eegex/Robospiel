#include "keymappingview.h"

#include <QSizePolicy>

KeyMappingView::KeyMappingView(QVector<KeyMapping*> mappings, QWidget *parent) : QWidget(parent)
{
	scroll = new QScrollArea(this);
	QWidget * widget = new QWidget(this);
	scroll->setWidget(widget);
	scroll->setWidgetResizable(true);
	completeMappings(mappings);
	grid = new QGridLayout;
	widget->setLayout(grid);
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
				name->setText(tr("move left"));
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
			case PlayerAction::giveUp:
				name->setText(tr("give up"));
				break;
			case PlayerAction::revert:
				name->setText(tr("undo the last move"));
				break;
			case PlayerAction::revertToBeginning:
				name->setText(tr("revert to initial positions"));
				break;
			case PlayerAction::freePlay:
				name->setText(tr("switch to freeplay"));
				break;
			case PlayerAction::vote:
				name->setText(tr("vote for current action"));
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
		grid->addWidget(getAddGroup(i), i, 2);
	}
	setWindowTitle(tr("Key Mappings"));
	lay = new QVBoxLayout(this);
	lay->addWidget(scroll);
	constructorHasEnded=true;
	checkMappings();
}

/**
 * @brief KeyMappingView::completeMappings adds default keyMappings and transfers the mappings to the allMappings variable
 * @param mappings the (incomplete) mapping
 */
void KeyMappingView::completeMappings(QVector<KeyMapping*> mappings)
{
	//collect all keys in use
	QVector<Qt::Key> usedKeys;
	for(KeyMapping* mapping:mappings)
	{
		usedKeys.append(mapping->getKeys());
	}
	for(PlayerAction action: mappableActions)
	{
		bool exists =false;

		//transfer the mapping to allMappings, if it exists
		for(int i=0; i<mappings.length(); i++)
		{
			if(mappings.at(i)->getAction()==action && mappings.at(i)->getKeys().size()>0)
			{
				exists=true;
				allMappings.append(new KeyMapping(*mappings.takeAt(i)));
				break;
			}
		}

		//add default mappings
		if(!exists)
		{
			QVector<Qt::Key> possibleDefaults;
			switch(action)
			{
				case PlayerAction::movePlayerNorth:
					possibleDefaults = {Qt::Key::Key_W,Qt::Key::Key_Up,Qt::Key::Key_F31};
					break;
				case PlayerAction::movePlayerEast:
					possibleDefaults = {Qt::Key::Key_D,Qt::Key::Key_Right,Qt::Key::Key_F32};
					break;
				case PlayerAction::movePlayerSouth:
					possibleDefaults = {Qt::Key::Key_S,Qt::Key::Key_Down,Qt::Key::Key_F33};
					break;
				case PlayerAction::movePlayerWest:
					possibleDefaults = {Qt::Key::Key_A,Qt::Key::Key_Left,Qt::Key::Key_F34};
					break;
				case PlayerAction::switchPlayerNorth:
					possibleDefaults = {Qt::Key::Key_I};
					break;
				case PlayerAction::switchPlayerEast:
					possibleDefaults = {Qt::Key::Key_L};
					break;
				case PlayerAction::switchPlayerSouth:
					possibleDefaults = {Qt::Key::Key_K};
					break;
				case PlayerAction::switchPlayerWest:
					possibleDefaults = {Qt::Key::Key_J};
					break;
				case PlayerAction::revert:
					possibleDefaults = {Qt::Key::Key_R};
					break;
				case PlayerAction::revertToBeginning:
					possibleDefaults = {Qt::Key::Key_B};
					break;
				case PlayerAction::giveUp:
					possibleDefaults = {Qt::Key::Key_Q};
					break;
				case PlayerAction::vote:
					possibleDefaults = {Qt::Key::Key_V};
					break;
			}

			//only use keys that are not already included in the mapping
			for(Qt::Key key : possibleDefaults)
			{
				if(!usedKeys.contains(key))
				{
					allMappings.append(new KeyMapping(action, key));
					usedKeys.append(key);
					exists = true;
					break;
				}
			}

			//when no key ist available, the action still has to be inserted to create an "Add" button later.
			if(!exists)
			{
				allMappings.append(new KeyMapping(action));
			}
		}
	}
	checkMappings();
}

/**
 * @brief KeyMappingView::checkMappings highlights label in the view in red to indicate duplicates
 */
void KeyMappingView::checkMappings()
{
	if(constructorHasEnded)
	{
		//iterate through all actions and all mapped keys for each action
		bool valid = true;
		QSet<Qt::Key> usedKeys;
		for(int i=0; i<allMappings.length(); i++)
		{
			bool rowValid = true;
			for(int j=0; j<allMappings.at(i)->getKeys().length(); j++)
			{
				//check for duplicate mapping
				if(usedKeys.contains(allMappings.at(i)->getKeys().at(j)))
				{
					valid = false;
					rowValid = false;
					//find all duplicates, (they occured before)
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

KeyInputBlock* KeyMappingView::getAddGroup(int row)
{
	KeyInputBlock* input = new KeyInputBlock();
	inputBlocks.insert(row, input);
	connect(inputBlocks.at(row), &KeyInputBlock::addMapping, this, [=](){
		if(inputBlocks.at(row)->hasKey() && allMappings.at(row)->addKey(inputBlocks.at(row)->getKey()))
		{
			insertKeyIntoUI(row, allMappings.at(row)->getKeys().length()-1);
			checkMappings();
		}
		inputBlocks.at(row)->reset();
	});

	return input;
}

void KeyMappingView::insertKeyIntoUI(int row, int col)
{
	QVBoxLayout* layout = new QVBoxLayout();
	QLabel* label = new QLabel(KeyInputRecorder::keyToString(allMappings.at(row)->getKeys().at(col)), this);
	keyLabels.at(row)->append(label);
	layout->addWidget(label);

	QPushButton* deleteBtn = new QPushButton(tr("Delete"), this);
	QFontMetrics* fontInfo = new QFontMetrics(deleteBtn->font());
	deleteBtn->setMaximumWidth(fontInfo->boundingRect(deleteBtn->text()+"...").width());
	connect(deleteBtn, &QAbstractButton::pressed, this, [=]()->void{
		keyLabels.at(row)->removeAll(label);

		layout->deleteLater();
		label->deleteLater();
		deleteBtn->deleteLater();

		allMappings.at(row)->removeKey(KeyInputRecorder::stringToKey(label->text()));
		checkMappings();
	});
	layout->addWidget(deleteBtn);
	layout->setAlignment(label, Qt::AlignHCenter);
	layout->setAlignment(deleteBtn, Qt::AlignHCenter);


	hBoxRows.at(row)->addLayout(layout);
	hBoxRows.at(row)->setAlignment(Qt::AlignLeft);
}

QVector<KeyMapping*> KeyMappingView::getMapping()
{
	QVector<KeyMapping*> copy;
	for(int i=0; i< allMappings.length(); i++)
	{
		copy.append(new KeyMapping(*allMappings.at(i)));
	}
	return copy;
}
