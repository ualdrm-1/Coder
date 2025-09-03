#include "filecoder.h"

FileCoder::FileCoder(const QString inputFileName, QByteArray code,
                    QString outputFileName, QObject* parent):
    QObject(parent), m_inputFileName(inputFileName), m_code(code)
{
    if (outputFileName.isEmpty()) {
        outputFileName = inputFileName;
    }else{
        m_outputFileName = outputFileName;
    }
}

bool FileCoder::process()
{
    QFile inputFile(m_inputFileName);
    QFile outputFile(m_outputFileName);

    if (!inputFile.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Cannot open input file: " + m_inputFileName);
        return false;
    }

    if (!outputFile.open(QIODevice::WriteOnly)) {
        emit errorOccurred("Cannot open output file: " + m_outputFileName);
        inputFile.close();
        return false;
    }

    const qint64 bufferSize = 64 * 1024; // 64KB буфер
    char buffer[bufferSize];
    qint64 totalSize = inputFile.size();
    qint64 bytesProcessed = 0;

    while (!inputFile.atEnd()) {
        qint64 bytesRead = inputFile.read(buffer, bufferSize);
        if (bytesRead == -1) {
            emit errorOccurred("Error reading file");
            break;
        }

        for (int i = 0; i < bytesRead; ++i) {
            buffer[i] = buffer[i] ^ m_code[i % m_code.size()];
        }

        qint64 bytesWritten = outputFile.write(buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            emit errorOccurred("Error writing file");
            break;
        }

        bytesProcessed += bytesRead;
        int progress = totalSize > 0 ? (bytesProcessed * 100) / totalSize : 0;
        emit progressChanged(progress);

        QCoreApplication::processEvents();
    }

    inputFile.close();
    outputFile.close();

    emit finished(true);
    return true;
}

FileCoder::~FileCoder() {}
