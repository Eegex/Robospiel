#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwidget.h"
#include <QWidget>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	MainWidget * centralWidget = nullptr;

protected:
	void closeEvent(QCloseEvent* event);
};
#endif // MAINWINDOW_H
