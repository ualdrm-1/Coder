#ifndef FILECODER_H
#define FILECODER_H

#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QThread>
#include <QCoreApplication>

class FileCoder: public QObject
{
    Q_OBJECT
private:
    QByteArray m_code;
    QString m_inputFileName;
    QString m_outputFileName;

    bool ReadFileData(QFile& file);
    bool CodeFileData(QFile& file);
public:
    FileCoder(const QString inputfileName, QByteArray code,
              QString outputFileName = "", QObject* parent = nullptr);
    ~FileCoder();
    bool process();
signals:
    void progressChanged(int percent);
    void finished(bool success);
    void errorOccurred(const QString& error);
};

#endif // FILECODER_H
