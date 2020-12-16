#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "keymapping.h"
#include "keymappingview.h"

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
    explicit SettingsDialog(QVector<KeyMapping *> mapping, QDialog *parent = nullptr);
private:
	QVBoxLayout * lay = new QVBoxLayout(this);
	QTabWidget * twTabs = new QTabWidget(this);
	QPushButton * pbSave = new QPushButton(tr("Save"),this);
    KeyMappingView* keyMappings;
signals:
    void newMapping(QVector<KeyMapping*> mapping);

private slots:
	void load();
	void save();
};

#endif // SETTINGSDIALOG_H
