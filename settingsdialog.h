#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

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
	explicit SettingsDialog(QDialog *parent = nullptr);
private:
	QVBoxLayout * lay = new QVBoxLayout(this);
	QTabWidget * twTabs = new QTabWidget(this);
	QPushButton * pbSave = new QPushButton(tr("Save"),this);
signals:

private slots:
	void load();
	void save();
};

#endif // SETTINGSDIALOG_H
