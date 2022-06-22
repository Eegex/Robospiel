#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QVector<KeyMapping*> mapping, QDialog * parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	lay->addWidget(twTabs);
	QWidget * generalWidget = new QWidget(this);
	generalWidget->setLayout(flGeneral);
	flGeneral->addRow(tr("User name"),leUsername);
	flGeneral->addRow(tr("Timer time (when you're server)"),leTimerTime);
	flGeneral->addRow(tr("User color"),pbUserColor);
	flGeneral->addRow(tr("Background color"),pbBackgroundColor);
	flGeneral->addRow(tr("Checkered Board"), pbCheckerboardColor);
	flGeneral->addRow(tr("Wall color"),pbWallColor);
	flGeneral->addRow(tr("Grid color"),pbGridColor);
	flGeneral->addRow(tr("Player color start"),pbPlayerColorLow);
	flGeneral->addRow(tr("Player color end"),pbPlayerColorHigh);
	flGeneral->addRow(tr("Show top bidder on timer"),cbTopBidding);
	flGeneral->addRow(tr("Fair Mode on"), cbFairMode);
	pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString());
	pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString());
	pbCheckerboardColor->setStyleSheet("background-color:" + settings.value(checkered).toString());
	pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString());
	pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString());
	pbPlayerColorLow->setStyleSheet("background-color:" + settings.value(playercolorlow).toString());
	pbPlayerColorHigh->setStyleSheet("background-color:" + settings.value(playercolorhigh).toString());
	cbTopBidding->setChecked(getShowTopBidding());
	cbFairMode->setChecked(getFairModeOn());
	twTabs->addTab(generalWidget,tr("General"));
	keyMappings = new KeyMappingView(mapping, this);
	twTabs->addTab(keyMappings,tr("Key mappings"));
	lay->addWidget(pbSave);
	connect(leUsername,&QLineEdit::editingFinished,this,[&]()
	{
		settings.insert(username,leUsername->text());
	});
	connect(leTimerTime,&QSpinBox::editingFinished,this,[&]()
	{
		settings.insert(timertime,leTimerTime->value());
	});
	connect(pbUserColor,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getUsercolor(),nullptr,tr("User Color"));
		if(c.isValid())
		{
			settings.insert(usercolor,c.name());
			pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString());
		}
	});
	connect(pbBackgroundColor,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getBackground(),nullptr,tr("Background Color"));
		if(c.isValid())
		{
			settings.insert(background,c.name());
			pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString());
		}
	});
	connect(pbCheckerboardColor,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getCheckerboardColor(),nullptr,tr("Checkerboard Color"));
		if(c.isValid())
		{
			settings.insert(checkered,c.name());
			pbCheckerboardColor->setStyleSheet("background-color:" + settings.value(checkered).toString());
		}
		else
		{
			settings.insert(checkered,"");
			pbCheckerboardColor->setStyleSheet("");
		}
	});
	connect(pbWallColor,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getUsercolor(),nullptr,tr("Wall Color"));
		if(c.isValid())
		{
			settings.insert(wallcolor,c.name());
			pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString());
		}
	});
	connect(pbGridColor,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getUsercolor(),nullptr,tr("Grid Color"));
		if(c.isValid())
		{
			settings.insert(gridcolor,c.name());
			pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString());
		}
	});
	connect(pbPlayerColorLow,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getUsercolor(),nullptr,tr("Player color-range begin"));
		if(c.isValid())
		{
			settings.insert(playercolorlow,c.name());
			pbPlayerColorLow->setStyleSheet("background-color:" + settings.value(playercolorlow).toString());
		}
	});
	connect(pbPlayerColorHigh,&QPushButton::clicked,this,[&]()
	{
		QColor c = QColorDialog::getColor(getUsercolor(),nullptr,tr("Player color-range end"));
		if(c.isValid())
		{
			settings.insert(playercolorhigh,QColorDialog::getColor(getPlayerColorHigh()).name());
			pbPlayerColorHigh->setStyleSheet("background-color:" + settings.value(playercolorhigh).toString());
		}
	});
	connect(cbTopBidding,&QCheckBox::toggled,this,[&]()
	{
		settings.insert(topBidding,cbTopBidding->isChecked());
	});
	connect(cbFairMode,&QCheckBox::toggled,this,[&]()
	{
		settings.insert(fairMode,cbFairMode->isChecked());
	});
	connect(pbSave,&QPushButton::clicked,this,&SettingsDialog::save);
}

QString SettingsDialog::getUsername() const
{
	return settings.value(username).toString();
}

int SettingsDialog::getTimerTime() const
{
	return settings.value(timertime).toInt();
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

QColor SettingsDialog::getPlayerColorLow() const
{
	return QColor(settings.value(playercolorlow).toString());
}

QColor SettingsDialog::getPlayerColorHigh() const
{
	return QColor(settings.value(playercolorhigh).toString());
}

QVector<KeyMapping *> SettingsDialog::getMapping() const
{
	return keyMappings->getMapping();
}

bool SettingsDialog::getShowTopBidding() const
{
	return settings.value(topBidding).toBool();
}

bool SettingsDialog::getFairModeOn() const
{
	return settings.value(fairMode).toBool();
}

QColor SettingsDialog::getCheckerboardColor() const
{
	return settings.value(checkered).toString();
}

/*!
 * \brief SettingsDialog::load loads a settings-File from at QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).
 * If none exists, standard values are set.
 */
void SettingsDialog::load()
{
	QDir saveDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveDir.exists())
	{
		saveDir.mkpath(saveDir.path());
	}
	QFile savefile(saveDir.filePath("settings.json"));
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
	if(mapping.size() < 12)
	{
		while(mapping.isEmpty())
		{
			delete mapping.takeFirst();
		}
		qDebug() << "neues mapping";
		mapping.append(new KeyMapping(PlayerAction::movePlayerNorth,{Qt::Key::Key_W,Qt::Key::Key_Up,Qt::Key::Key_F31}));
		mapping.append(new KeyMapping(PlayerAction::movePlayerEast,{Qt::Key::Key_D,Qt::Key::Key_Right,Qt::Key::Key_F32}));
		mapping.append(new KeyMapping(PlayerAction::movePlayerSouth,{Qt::Key::Key_S,Qt::Key::Key_Down,Qt::Key::Key_F33}));
		mapping.append(new KeyMapping(PlayerAction::movePlayerWest,{Qt::Key::Key_A,Qt::Key::Key_Left,Qt::Key::Key_F34}));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerNorth,Qt::Key::Key_I));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerEast,Qt::Key::Key_L));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerSouth,Qt::Key::Key_K));
		mapping.append(new KeyMapping(PlayerAction::switchPlayerWest,Qt::Key::Key_J));
		mapping.append(new KeyMapping(PlayerAction::revert,Qt::Key::Key_R));
		mapping.append(new KeyMapping(PlayerAction::revertToBeginning,Qt::Key::Key_B));
		mapping.append(new KeyMapping(PlayerAction::giveUp,Qt::Key::Key_Q));
		mapping.append(new KeyMapping(PlayerAction::vote,Qt::Key::Key_V));
	}
	if(!settings.contains(username))
	{
		settings.insert(username,tr("New User"));
	}
	if(!settings.contains(timertime))
	{
		settings.insert(timertime,30);
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
	if(!settings.contains(playercolorhigh))
	{
		settings.insert(playercolorhigh,"#FF0000");
	}
	if(!settings.contains(playercolorlow))
	{
		settings.insert(playercolorlow,"#FF0001");
	}
	if(!settings.contains(topBidding))
	{
		settings.insert(topBidding,false);
	}
	if(!settings.contains(fairMode))
	{
		settings.insert(fairMode,true);
	}
	if(!settings.contains(checkered))
	{
		settings.insert(checkered,"#88FF0000");
	}
	delete keyMappings;
	keyMappings = new KeyMappingView(mapping, this);
	twTabs->addTab(keyMappings,tr("Key mappings"));
	leUsername->setText(getUsername());
	leTimerTime->setMinimum(1);
	leTimerTime->setMaximum(99);
	leTimerTime->setSingleStep(5);
	leTimerTime->setValue(getTimerTime());
	pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString());
	pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString());
	pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString());
	pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString());
	pbPlayerColorLow->setStyleSheet("background-color:" + settings.value(playercolorlow).toString());
	pbPlayerColorHigh->setStyleSheet("background-color:" + settings.value(playercolorhigh).toString());
	if(QColor(settings.value(checkered).toString()).isValid())
	{
		pbCheckerboardColor->setStyleSheet("background-color:" + settings.value(checkered).toString());
	}
	else
	{
		pbCheckerboardColor->setStyleSheet("");
	}
	cbTopBidding->setChecked(getShowTopBidding());
	cbFairMode->setChecked(getFairModeOn());
}

/*!
 * \brief SettingsDialog::save Applies the settings and saves them to a file (at QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).
 */
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
	emit colorsChanged();
	emit newMapping(keyMappings->getMapping());
	emit usernameChanged(settings.value(username).toString());
	emit timertimeChanged(settings.value(timertime).toInt());
	emit usercolorChanged(QColor(settings.value(usercolor).toString()));
	if(isVisible())
	{
		close();
	}


}

void SettingsDialog::enableTimerChange(bool boolean)
{
	leTimerTime->setDisabled(!boolean);
}

void SettingsDialog::giveServerTimerTimeToEveryone()
{
	//emit timertimeChanged(settings.value(timertime).toString().toInt());
}
