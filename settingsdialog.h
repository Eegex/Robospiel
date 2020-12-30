#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QFormLayout>
#include <QLineEdit>
#include <QColorDialog>
#include <QScrollArea>
#include "keymapping.h"
#include "keymappingview.h"

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SettingsDialog(QVector<KeyMapping *> mapping, QDialog *parent = nullptr);
private:
	QVBoxLayout * lay = new QVBoxLayout(this);
	QLineEdit * leUsername = new QLineEdit(this);
	QPushButton * pbUserColor = new QPushButton(this);
	QPushButton * pbBackgroundColor = new QPushButton(this);
	QPushButton * pbWallColor = new QPushButton(this);
	QPushButton * pbGridColor = new QPushButton(this);
	QTabWidget * twTabs = new QTabWidget(this);
	QPushButton * pbSave = new QPushButton(tr("Save"),this);
	QFormLayout * flGeneral = new QFormLayout;
	KeyMappingView * keyMappings;
	QString username;
	QColor user = Qt::black;
	QColor back = QColor(229,255,0);
	QColor wall = QColor(255, 0, 140);
	QColor grid = QColor(0, 255, 217);
signals:
	void newMapping(QVector<KeyMapping*> mapping);
	void colorsChanged(QColor b, QColor w, QColor g);

private slots:
	void load();
	void save();
};

#endif // SETTINGSDIALOG_H
