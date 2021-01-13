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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "keymapping.h"
#include "keymappingview.h"

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SettingsDialog(QVector<KeyMapping *> mapping, QDialog *parent = nullptr);
	QString getUsername() const;
	QColor getUsercolor() const;
	QColor getBackground() const;
	QColor getWallcolor() const;
	QColor getGridcolor() const;
	QVector<KeyMapping *> getMapping() const;

public slots:
	void load();
	void save();

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
	QJsonObject settings;
	const QString username = "un";
	const QString usercolor = "sc";
	const QString background = "bc";
	const QString wallcolor = "wc";
	const QString gridcolor = "gc";
	const QString keymappings = "km";
signals:
	void newMapping(QVector<KeyMapping*> mapping);
	void colorsChanged();
    void usernameChanged(QString name);
    void usercolorChanged(QColor color);

private slots:
};

#endif // SETTINGSDIALOG_H
