#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filelistdisplay.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    FileListDisplay* display;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
