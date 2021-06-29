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
	setWindowTitle(tr("Key Mappings"));
	lay = new QVBoxLayout(this);
	lay->addWidget(scroll);
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
	for(PlayerAction action: mappableActions)
	{
		bool exists =false;
		for(int i=0; i<mappings.length(); i++)
		{
			if(mappings.at(i)->getAction()==action)
			{
				exists=true;
				allMappings.append(new KeyMapping(*mappings.takeAt(i)));
				break;
			}
		}
		if(!exists)
		{
			allMappings.append(new KeyMapping(action));
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

QVBoxLayout* KeyMappingView::getAddGroup(int row)
{
	QVBoxLayout* layout = new QVBoxLayout();
	KeyInput* input = new KeyInput();


	QPushButton* btnAddKey = new QPushButton(tr("Add"), this);
	btnAddKey->setFixedWidth(KeyInput::inputWidth);
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
	QLabel* label = new QLabel(KeyInput::keyToString(allMappings.at(row)->getKeys().at(col)), this);
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

		allMappings.at(row)->removeKey(KeyInput::stringToKey(label->text()));
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
