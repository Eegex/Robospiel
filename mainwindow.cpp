#include "keymappingview.h"
#include "mainwindow.h"
#include "PlayerBiddingWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    centralWidget = new MainWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setMenuBar(this->menuBar());
    showMaximized();
}

MainWindow::~MainWindow()
{

}

