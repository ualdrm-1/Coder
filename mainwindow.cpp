#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(600,300);
    display = new FileListDisplay(this);

    setCentralWidget(display);
}

MainWindow::~MainWindow() {}

/* параметры:счетчик файла или перезапись
 * статус бар
*/
