#ifndef FILELISTDISPLAY_H
#define FILELISTDISPLAY_H

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QFileSystemModel>
#include <QDir>
#include <QModelIndex>
#include<QProgressDialog>
#include <QMessageBox>

#include "filecoder.h"

class FileListDisplay: public QWidget
{
    Q_OBJECT
private:
    QPushButton* m_startProcess;
    QPushButton* m_saveCode;

    QString m_fileName;
    QString m_destinationPath;

    QProgressDialog* m_progressDialog;

    QListView* m_sourceDir;
    QListView* m_aimDir;

    QVBoxLayout* m_bottomLayout;
    QVBoxLayout* m_VLayout;
    QHBoxLayout* m_HLayout;

    QLineEdit *m_enterCode;

    QThread* m_coderThread;

    QLabel* m_hint;

    FileCoder* m_currentCoder;

    QFileSystemModel *m_model;

    QByteArray m_code;

    void setMainPart();
    void setBottomPart();
    void setFileSystem();
private slots:
    void on_sourceDir_DoubleClicked(const QModelIndex& index);
    void on_aimDir_DoubleClicked(const QModelIndex& index);
    void on_sourceDir_Clicked(const QModelIndex &index);
    void on_enterCode_Clicked();
    void on_saveButton_Clicked();
    void on_CoderProgress(int percent);
    void on_CoderFinished(bool success);
    void on_CoderError(const QString& error);
public:
    FileListDisplay(QWidget* parent = nullptr);
    ~FileListDisplay();
};

#endif // FILELISTDISPLAY_H
