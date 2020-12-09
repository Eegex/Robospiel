#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QDialog * parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	lay->addWidget(twTabs);
	twTabs->addTab(new QWidget(this),tr("General"));
	twTabs->addTab(new QWidget(this),tr("Key mappings"));
	lay->addWidget(pbSave);
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
}
