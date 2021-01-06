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
	QScrollArea* scroll = new QScrollArea(this);
	keyMappings = new KeyMappingView(mapping, this);
	scroll->setWidget(keyMappings);
	scroll->setWidgetResizable(true);
	twTabs->addTab(scroll,tr("Key mappings"));
	lay->addWidget(pbSave);
	connect(leUsername,&QLineEdit::editingFinished,this,[&](){ settings.insert(username,leUsername->text()); });
	connect(pbUserColor,&QPushButton::clicked,this,[&](){ settings.insert(usercolor,QColorDialog::getColor().name()); pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString()); });
	connect(pbBackgroundColor,&QPushButton::clicked,this,[&](){ settings.insert(background,QColorDialog::getColor().name()); pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString()); });
	connect(pbWallColor,&QPushButton::clicked,this,[&](){ settings.insert(wallcolor,QColorDialog::getColor().name()); pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString()); });
	connect(pbGridColor,&QPushButton::clicked,this,[&](){ settings.insert(gridcolor,QColorDialog::getColor().name()); pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString()); });
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
	leUsername->setText(getUsername());
	pbUserColor->setStyleSheet("background-color:" + settings.value(usercolor).toString());
	pbBackgroundColor->setStyleSheet("background-color:" + settings.value(background).toString());
	pbWallColor->setStyleSheet("background-color:" + settings.value(wallcolor).toString());
	pbGridColor->setStyleSheet("background-color:" + settings.value(gridcolor).toString());
}

void SettingsDialog::save()
{

	QDir saveOrdner(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveOrdner.exists())
	{
		saveOrdner.mkpath(saveOrdner.path());
	}
	QFile savefile(saveOrdner.filePath("settings.json"));
	if(savefile.open(QIODevice::WriteOnly))
	{
		QJsonDocument document(settings);
		savefile.write(document.toJson());
		savefile.close();
	}
	emit colorsChanged(QColor(settings.value(background).toString()),QColor(settings.value(wallcolor).toString()),QColor(settings.value(gridcolor).toString()));
	emit newMapping(keyMappings->getMapping());
}
