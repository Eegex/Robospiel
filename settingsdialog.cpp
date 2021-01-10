#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QVector<KeyMapping*> mapping, QDialog * parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	lay->addWidget(twTabs);
	QWidget * generalWidget = new QWidget(this);
	generalWidget->setLayout(flGeneral);
	flGeneral->addRow(tr("User name"),leUsername);
	flGeneral->addRow(tr("User color"),pbUserColor);
	flGeneral->addRow(tr("Background color"),pbBackgroundColor);
	flGeneral->addRow(tr("Wall color"),pbWallColor);
	flGeneral->addRow(tr("Grid color"),pbGridColor);
	pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString());
	pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString());
	pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString());
	pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString());
	twTabs->addTab(generalWidget,tr("General"));
	keyMappings = new KeyMappingView(mapping, this);
	twTabs->addTab(keyMappings,tr("Key mappings"));
	lay->addWidget(pbSave);
	connect(leUsername,&QLineEdit::editingFinished,this,[&](){ settings.insert(username,leUsername->text()); });
	connect(pbUserColor,&QPushButton::clicked,this,[&](){ settings.insert(usercolor,QColorDialog::getColor(getUsercolor()).name()); pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString()); });
	connect(pbBackgroundColor,&QPushButton::clicked,this,[&](){ settings.insert(background,QColorDialog::getColor(getBackground()).name()); pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString()); });
	connect(pbWallColor,&QPushButton::clicked,this,[&](){ settings.insert(wallcolor,QColorDialog::getColor(getWallcolor()).name()); pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString()); });
	connect(pbGridColor,&QPushButton::clicked,this,[&](){ settings.insert(gridcolor,QColorDialog::getColor(getGridcolor()).name()); pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString()); });
	connect(pbSave,&QPushButton::clicked,this,&SettingsDialog::save);
}

QString SettingsDialog::getUsername() const
{
	return settings.value(username).toString();
}

QColor SettingsDialog::getUsercolor() const
{
	return QColor(settings.value(usercolor).toString());
}

QColor SettingsDialog::getBackground() const
{
	return QColor(settings.value(background).toString());
}

QColor SettingsDialog::getWallcolor() const
{
	return QColor(settings.value(wallcolor).toString());
}

QColor SettingsDialog::getGridcolor() const
{
	return QColor(settings.value(gridcolor).toString());
}

QVector<KeyMapping *> SettingsDialog::getMapping() const
{
	return keyMappings->getMapping();
}

void SettingsDialog::load()
{
	QDir saveOrdner(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveOrdner.exists())
	{
		saveOrdner.mkpath(saveOrdner.path());
	}
	QFile savefile(saveOrdner.filePath("settings.json"));
	if(savefile.open(QIODevice::ReadOnly))
	{
		settings = QJsonDocument::fromJson(savefile.readAll()).object();
		savefile.close();
	}
	QVector<KeyMapping*> mapping;
	QJsonObject rawMapping = settings.value(keymappings).toObject();
	for(const QString & action:rawMapping.keys())
	{
		QJsonArray rawKeys = rawMapping.value(action).toArray();
		QVector<Qt::Key> keys;
		for(const QJsonValue & v:rawKeys)
		{
			keys.append(static_cast<Qt::Key>(v.toInt()));
		}
		mapping.append(new KeyMapping(static_cast<PlayerAction>(action.toInt()),keys));
	}
	if(mapping.isEmpty())
	{
		mapping.append(new KeyMapping(PlayerAction::movePlayerNorth,{Qt::Key::Key_W,Qt::Key::Key_Up}));
		mapping.append(new KeyMapping(PlayerAction::movePlayerEast,{Qt::Key::Key_D,Qt::Key::Key_Right}));
		mapping.append(new KeyMapping(PlayerAction::movePlayerSouth,{Qt::Key::Key_S,Qt::Key::Key_Down}));
		mapping.append(new KeyMapping(PlayerAction::movePlayerWest,{Qt::Key::Key_A,Qt::Key::Key_Left}));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerNorth,Qt::Key::Key_I));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerEast,Qt::Key::Key_L));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerSouth,Qt::Key::Key_K));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerWest,Qt::Key::Key_J));
		mapping.append(new KeyMapping(PlayerAction::revert,Qt::Key::Key_R));
		mapping.append(new KeyMapping(PlayerAction::revertToBeginning,Qt::Key::Key_B));
		mapping.append(new KeyMapping(PlayerAction::giveUp,Qt::Key::Key_Q));
		mapping.append(new KeyMapping(PlayerAction::enterBidding,Qt::Key::Key_Space));
		mapping.append(new KeyMapping(PlayerAction::clearBidding,Qt::Key::Key_Backspace));
		mapping.append(new KeyMapping(PlayerAction::sendBidding,Qt::Key::Key_Return));
	}
	if(!settings.contains(username))
	{
		settings.insert(username,tr("New User"));
	}
	if(!settings.contains(usercolor))
	{
		settings.insert(usercolor,"#000000");
	}
	if(!settings.contains(background))
	{
		settings.insert(background,"#FFFF00");
	}
	if(!settings.contains(gridcolor))
	{
		settings.insert(gridcolor,"#00FFFF");
	}
	if(!settings.contains(wallcolor))
	{
		settings.insert(wallcolor,"#FF00FF");
	}
	delete keyMappings;
	keyMappings = new KeyMappingView(mapping, this); //TODO: ...?
	twTabs->addTab(keyMappings,tr("Key mappings"));
	leUsername->setText(getUsername());
	pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString());
	pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString());
	pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString());
	pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString());
}

void SettingsDialog::save()
{
	QJsonObject mappings;
	for(KeyMapping * m:keyMappings->getMapping())
	{
		QJsonArray keys;
		for(Qt::Key k:m->getKeys())
		{
			keys << static_cast<int>(k);
		}
		mappings.insert(QString::number(static_cast<int>(m->getAction())),keys);
	}
	settings.insert(keymappings,mappings);
	QJsonDocument document(settings);
	QDir saveOrdner(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveOrdner.exists())
	{
		saveOrdner.mkpath(saveOrdner.path());
	}
	QFile savefile(saveOrdner.filePath("settings.json"));
	if(savefile.open(QIODevice::WriteOnly))
	{
		savefile.write(document.toJson());
		savefile.close();
	}
	emit colorsChanged(QColor(settings.value(background).toString()),QColor(settings.value(wallcolor).toString()),QColor(settings.value(gridcolor).toString()));
	emit newMapping(keyMappings->getMapping());
	if(isVisible())
	{
		close();
	}
}
