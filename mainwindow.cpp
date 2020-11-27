#include "networkview.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setCentralWidget(new NetworkView());
    show();
}

MainWindow::~MainWindow()
{

}

