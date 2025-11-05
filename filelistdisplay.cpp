    #include "filelistdisplay.h"

    FileListDisplay::FileListDisplay(QWidget* parent): QWidget(parent),
                                                       m_currentCoder(nullptr),
                                                       m_coderThread(nullptr)
    {
        m_VLayout = new QVBoxLayout(this);
        m_HLayout = new QHBoxLayout;
        m_bottomLayout = nullptr;

        setMainPart();
        setBottomPart();
        setFileSystem();

        m_VLayout->addLayout(m_HLayout);
        m_VLayout->addLayout(m_bottomLayout);

        m_progressDialog=nullptr;

        connect(m_sourceDir, &QListView::doubleClicked, this,
                &FileListDisplay::on_sourceDir_DoubleClicked);
        connect(m_aimDir, &QListView::doubleClicked, this,
                &FileListDisplay::on_aimDir_DoubleClicked);
        connect(m_sourceDir, &QListView::clicked, this,
                &FileListDisplay::on_sourceDir_Clicked);
        connect(m_startProcess, &QPushButton::clicked, this,
                &FileListDisplay::on_enterCode_Clicked);
        connect(m_saveCode, &QPushButton::clicked, this,
                &FileListDisplay::on_saveButton_Clicked);
        connect(m_enterCode, &QLineEdit::textChanged, this, [this]() {
            m_saveCode->setEnabled(true);
        });

    }

    FileListDisplay::~FileListDisplay()
    {
        if (m_coderThread && m_coderThread->isRunning()) {
            m_coderThread->quit();
            m_coderThread->wait();
        }
        if(m_currentCoder){
            delete m_currentCoder;
            m_currentCoder=nullptr;
        }
        delete m_HLayout;
        delete m_bottomLayout;
    }

    void FileListDisplay::setMainPart(){
        m_sourceDir = new QListView(this);
        m_aimDir = new QListView(this);

        m_startProcess = new QPushButton("CODE",this);

        m_startProcess->setFixedSize(100, 50);

        m_sourceDir->setFixedSize(230,200);
        m_aimDir->setFixedSize(230,200);

        m_HLayout->addWidget(m_sourceDir,1, Qt::AlignLeft);
        m_HLayout->addWidget(m_startProcess,1, Qt::AlignCenter);
        m_HLayout->addWidget(m_aimDir, 1, Qt::AlignRight);
    }

    void FileListDisplay::setBottomPart(){
        QHBoxLayout* codeLayout = new QHBoxLayout();
        m_bottomLayout = new QVBoxLayout;

        m_enterCode = new QLineEdit(this);

        m_saveCode = new QPushButton("Apply",this);
        m_saveCode->setFixedSize(60,50);

        m_hint = new QLabel("Enter 8 Bit Code",this);

        codeLayout->addWidget(m_enterCode);
        codeLayout->addWidget(m_saveCode, 0, Qt::AlignLeft);

        m_bottomLayout->addWidget(m_hint);
        m_bottomLayout->addLayout(codeLayout);

        m_enterCode->setInputMask("00 00 00 00 00 00 00 00");
        m_enterCode->setFixedSize(200,50);
    }

    void FileListDisplay::setFileSystem(){
        m_model = new QFileSystemModel(this);
        m_model->setFilter(QDir::AllEntries);
        m_model->setRootPath("");

        m_sourceDir->setModel(m_model);
        m_sourceDir->setRootIndex(m_model->index(QDir::currentPath()));

        m_aimDir->setModel(m_model);
        m_aimDir->setRootIndex(m_model->index(QDir::currentPath()));
    }
    /*сигналы*/
    void FileListDisplay::on_sourceDir_DoubleClicked(const QModelIndex &index){
        QFileInfo fileInfo = m_model->fileInfo(index);

        if(fileInfo.fileName()== ".."){
            QDir dir = fileInfo.dir();
            dir.cdUp();
            m_sourceDir->setRootIndex(m_model->index(dir.absolutePath()));
        }else if(fileInfo.fileName()=="."){
            m_sourceDir->setRootIndex(m_model->index(""));
        }else if(fileInfo.isDir()){
            m_sourceDir->setRootIndex(index);
        }
    }

    void FileListDisplay::on_aimDir_DoubleClicked(const QModelIndex &index){
        QFileInfo fileInfo = m_model->fileInfo(index);

        if(fileInfo.fileName() == ".."){
            QDir dir = fileInfo.dir();
            dir.cdUp();
            m_aimDir->setRootIndex(m_model->index(dir.absolutePath()));
        }else if(fileInfo.fileName() == "."){
            m_aimDir->setRootIndex(m_model->index(""));
        }else if(fileInfo.isDir()){
            m_aimDir->setRootIndex(index);
            m_destinationPath = fileInfo.absoluteFilePath();
            qDebug() << "Destination folder:" << m_destinationPath;
        }
    }

    void FileListDisplay::on_sourceDir_Clicked(const QModelIndex &index){
        QFileInfo fileInfo = m_model->fileInfo(index);
        if(fileInfo.isFile()){
            m_fileName = fileInfo.absoluteFilePath();
        }
    }

    void FileListDisplay::on_enterCode_Clicked(){
        if (m_fileName.isEmpty()) {
            qWarning() << "No file selected!";
            return;
        }

        if (m_code.isEmpty()) {
            qWarning() << "Apply first.";
            return;
        }

        if (m_destinationPath.isEmpty()) {
            qWarning() << "No folder selected!";
            return;
        }

        QFileInfo sourceFileInfo(m_fileName);
        QString baseName = sourceFileInfo.completeBaseName();
        QString suffix = sourceFileInfo.suffix();
        QString outputFileName = m_destinationPath + "/" + sourceFileInfo.fileName();

        QFileInfo outputFileInfo(outputFileName);
        int counter = 1;

        while (outputFileInfo.exists()) {
            outputFileName = m_destinationPath + "/" + baseName +
                             "_" + QString::number(counter) +
                             (suffix.isEmpty() ? "" : "." + suffix);
            outputFileInfo.setFile(outputFileName);
            counter++;
        }

        if (m_coderThread && m_coderThread->isRunning()) {
            m_coderThread->quit();
            m_coderThread->wait();
            delete m_coderThread;
            m_coderThread=nullptr;
        }

        if(m_currentCoder){
            delete m_coderThread;
            m_coderThread=nullptr;
        }

        if (!m_progressDialog) {
            m_progressDialog = new QProgressDialog("Processing file...", "Cancel", 0, 100, this);
            m_progressDialog->setWindowModality(Qt::WindowModal);
            m_progressDialog->setAutoClose(true);
            m_progressDialog->setAutoReset(true);
        }

        m_coderThread = new QThread(this);
        m_currentCoder = new FileCoder(m_fileName, m_code, outputFileName);
        m_currentCoder->moveToThread(m_coderThread);

        connect(m_coderThread, &QThread::started, m_currentCoder, &FileCoder::process);
        connect(m_currentCoder, &FileCoder::progressChanged, this, &FileListDisplay::on_CoderProgress);
        connect(m_currentCoder, &FileCoder::finished, this, &FileListDisplay::on_CoderFinished);
        connect(m_currentCoder, &FileCoder::errorOccurred, this, &FileListDisplay::on_CoderError);
        connect(m_progressDialog, &QProgressDialog::canceled, this, [this]() {
            if (m_coderThread && m_coderThread->isRunning()) {
                m_coderThread->quit();
                m_coderThread->wait();
            }
            if(m_currentCoder){
                m_currentCoder->deleteLater();
                m_currentCoder=nullptr;
            }
        });

        connect(m_currentCoder, &FileCoder::finished, m_coderThread, &QThread::quit);
        connect(m_currentCoder, &FileCoder::finished, m_currentCoder, &FileCoder::deleteLater);
        connect(m_coderThread, &QThread::finished, m_coderThread, &QThread::deleteLater);
        connect(m_coderThread, &QThread::finished, this, [this]() {
            m_coderThread = nullptr;
        });


        m_progressDialog->setValue(0);
        m_progressDialog->show();
        m_coderThread->start();
    }

    void FileListDisplay::on_saveButton_Clicked(){
        QString codeText = m_enterCode->text().remove(" ");

        if (codeText.isEmpty()) {
            qWarning() << "Code cant be empty";
            return;
        }

        if (codeText.length() != 16) {
            qWarning() << "Code must be 8 bytes";
            return;
        }

        QRegularExpression hexRegex("^[0-9A-Fa-f]{16}$");
        if (!hexRegex.match(codeText).hasMatch()) {
            qWarning() << "Code must contain only hex characters (0-9, A-F)";
            return;
        }

        m_code = QByteArray::fromHex(codeText.toLatin1());
        qDebug() << "Code saved:" << m_code.toHex();

        m_saveCode->setEnabled(false);
    }

    void FileListDisplay::on_CoderProgress(int percent)
    {
        if(m_progressDialog && !m_progressDialog->wasCanceled())
            m_progressDialog->setValue(percent);
    }

    void FileListDisplay::on_CoderFinished(bool success)
    {
        m_progressDialog->setValue(100);
        if (success) {
            QMessageBox::information(this, "Success", "File processed successfully!");
        }
        m_currentCoder = nullptr;
        m_fileName.clear();
    }

    void FileListDisplay::on_CoderError(const QString& error)
    {
        m_progressDialog->cancel();
        m_progressDialog->hide();
        QMessageBox::critical(this, "Error", error);
        m_currentCoder = nullptr;
    }
