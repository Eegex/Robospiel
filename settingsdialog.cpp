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
	pbUserColor->setStyleSheet("background-color:" + user.name());
	pbBackgroundColor->setStyleSheet("background-color:" + back.name());
	pbWallColor->setStyleSheet("background-color:" + wall.name());
	pbGridColor->setStyleSheet("background-color:" + grid.name());
	twTabs->addTab(generalWidget,tr("General"));
	QScrollArea* scroll = new QScrollArea(this);
	keyMappings = new KeyMappingView(mapping, this);
	scroll->setWidget(keyMappings);
	scroll->setWidgetResizable(true);
	twTabs->addTab(scroll,tr("Key mappings"));
	lay->addWidget(pbSave);
	connect(leUsername,&QLineEdit::editingFinished,this,[&](){ username = leUsername->text(); });
	connect(pbUserColor,&QPushButton::clicked,this,[&](){ user = QColorDialog::getColor(); pbUserColor->setStyleSheet("background-color:" + user.name()); });
	connect(pbBackgroundColor,&QPushButton::clicked,this,[&](){ back = QColorDialog::getColor(); pbBackgroundColor->setStyleSheet("background-color:" + back.name()); });
	connect(pbWallColor,&QPushButton::clicked,this,[&](){ wall = QColorDialog::getColor(); pbWallColor->setStyleSheet("background-color:" + wall.name()); });
	connect(pbGridColor,&QPushButton::clicked,this,[&](){ grid = QColorDialog::getColor(); pbGridColor->setStyleSheet("background-color:" + grid.name()); });
	connect(pbSave,&QPushButton::clicked,this,&SettingsDialog::save);
}

void SettingsDialog::load()
{
	QDir saveOrdner(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveOrdner.exists())
	{
		saveOrdner.mkpath(saveOrdner.path());
	}
	QFile savefile(saveOrdner.filePath("user.config"));
	if(savefile.open(QIODevice::ReadOnly))
	{
		savefile.close();
	}
}

void SettingsDialog::save()
{
	QDir saveOrdner(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	if(!saveOrdner.exists())
	{
		saveOrdner.mkpath(saveOrdner.path());
	}
	QFile savefile(saveOrdner.filePath("user.config"));
	if(savefile.open(QIODevice::WriteOnly))
	{
		savefile.close();
	}
	emit colorsChanged(back,wall,grid);
	emit newMapping(keyMappings->getMapping());
}
