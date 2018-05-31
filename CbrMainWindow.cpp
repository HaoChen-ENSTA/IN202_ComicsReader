#include "CbrMainWindow.h"
#include "ui_CbrMainWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>
//#include <opencv2/opencv.hpp>
//#include <asmOpenCV.h>
//#include <opencv2/core/cvstd.hpp>
#include <iostream>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//using namespace cv;
#include <QProcess>


using namespace std;

CbrMainWindow::CbrMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CbrMainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/qrc/logo.jpg"));
    isOpenedBefore = 0;
    picFlag = 0;
    _cbrscene = new QGraphicsScene;
    _cbrscene->setBackgroundBrush(QColor::fromRgb(224,224,224));
    ui->graphicsView->setScene(_cbrscene);
    _icon = new QSystemTrayIcon;
    _icon->setIcon(QIcon(":/qrc/logo.jpg"));
    _icon->setToolTip("ComicsBookReader");
    _icon->show();
    _icon->setContextMenu(ui->menu_File);

    connect(_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));

    this->installEventFilter(this);
    QTimer *time = new QTimer(this);
    connect(time,SIGNAL(timeout()),this,SLOT(setSliderVal()));
    time->start(200);
    ui->horizontalSlider->setPageStep(1);
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(jump2Pic()));

    //***************** CHEN Hao_beg *****************//

    Folder_openAction = new QAction(QIcon(":/images/doc-open"), tr("&Open folder"), this);
    Folder_openAction->setShortcuts(QKeySequence::Open);
    Folder_openAction->setStatusTip(tr("Open an existing file"));
    connect(Folder_openAction, &QAction::triggered, this, CbrMainWindow::folder_open);

//    File_openAction = new QAction(QIcon(":/images/doc-open"), tr("&Open file"), this);
//    File_openAction->setShortcuts(QKeySequence::Open);
//    File_openAction->setStatusTip(tr("Open an existing file"));
//    connect(File_openAction, &QAction::triggered, this, CbrMainWindow::file_open);

    ui->menu_File->addAction(Folder_openAction);
    //ui->menu_File->addAction(File_openAction);

    mTreeView = new FolderTree(ui->gridLayout_7);
    mTreeView->setLayout(mTreeView->_layout);
    connect(mTreeView->_TreeView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(TreeViewDoubleClick()));


//***************** CHEN Hao_end *****************//
}

CbrMainWindow::~CbrMainWindow()
{
    delete ui;
}

//***************** CHEN Hao_beg *****************//
void CbrMainWindow::closeEvent(QCloseEvent *event) //此函数在QWidget关闭时执行
{
    QString projectPath = QCoreApplication::applicationDirPath();   // 获取项目路径
    QString unCompressTo = projectPath+"/temp/";   // 解压目的地路径 这里因为我想解压到项目的根目录上所以截取了路径。
    folder_delete(unCompressTo);
}


void CbrMainWindow::folder_delete(QString deletePath)
{
    QString _deletePath = deletePath;   // 获取项目路径
    QFileInfoList fileList;
    QDir dir(_deletePath);
    //qDebug()<<unCompressTo;
    QFileInfo curFile;
    if(!dir.exists())  {qDebug()<<"!EXIST";return ;}//文件不存，则返回false
        fileList=dir.entryInfoList(QDir::Dirs|QDir::Files
                                   |QDir::Readable|QDir::Writable
                                   |QDir::Hidden|QDir::NoDotAndDotDot
                                   ,QDir::Name);
        while(fileList.size()>0)
        {
            int infoNum=fileList.size();
            for(int i=infoNum-1;i>=0;i--)
            {
                curFile=fileList[i];
                if(curFile.isFile())//如果是文件，删除文件
                {
                    QFile fileTemp(curFile.filePath());
                    fileTemp.remove();
                    fileList.removeAt(i);
                }
                if(curFile.isDir())//如果是文件夹
                {
                    QDir dirTemp(curFile.filePath());
                    QFileInfoList fileList1=dirTemp.entryInfoList(QDir::Dirs|QDir::Files
                                                                  |QDir::Readable|QDir::Writable
                                                                  |QDir::Hidden|QDir::NoDotAndDotDot
                                                                  ,QDir::Name);
                    if(fileList1.size()==0)//下层没有文件或文件夹
                    {
                        dirTemp.rmdir(".");
                        fileList.removeAt(i);
                    }
                    else//下层有文件夹或文件
                    {
                        for(int j=0;j<fileList1.size();j++)
                        {
                            if(!(fileList.contains(fileList1[j])))
                                fileList.append(fileList1[j]);
                        }
                    }
                }
            }
        }
}

void CbrMainWindow::folder_open()
{

    if(index_file_state == 1 ){delete ima_TreeView->_TreeView;
    delete ima_TreeView->hide_button;}
    delete mTreeView->_TreeView;
    QString folder_name = QFileDialog::getExistingDirectory(NULL,"Open",".");
    index_folder_state = 1;
    mTreeView = new FolderTree(ui->gridLayout_7,folder_name,index_folder_state);
    //mTreeView->setLayout(mTreeView->_layout);
    //mTreeView->_TreeView->show();
    connect(mTreeView->_TreeView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(TreeViewDoubleClick()));
    index_file_state = 0;
}


void CbrMainWindow::TreeViewDoubleClick()
{

    QModelIndex index = mTreeView->_TreeView->currentIndex();

            //qDebug()<<mTreeView->_model->fileInfo(index).filePath();

    _file_suffix = mTreeView->_model->fileInfo(index).suffix();
    qDebug()<<_file_suffix;
    if(_file_suffix.compare("zip")!=0&&_file_suffix.compare("cbz")!=0&&_file_suffix.compare("cbr")!=0&&
                    _file_suffix.compare("rar")!=0){qDebug()<<"break"; return;}

    if(index_file_state == 1 ){delete ima_TreeView->_TreeView;
    delete ima_TreeView->hide_button;}

    QString path = mTreeView->_model->fileInfo(index).filePath();
    if (    _file_suffix.compare("zip")==0||_file_suffix.compare("cbz")==0||
            _file_suffix.compare("jpg")==0||_file_suffix.compare("jpeg")==0||
            _file_suffix.compare("bmp")==0||
            _file_suffix.compare("png")==0)
    {
        // not opened any file yet
        if(!isOpenedBefore){
            isOpenedBefore = 1;
            // update curentPic imediately, setSlideVal() needs it
            currentPic.clear();
            currentPic.append(0);
            modeFlag = 0; // reset
            _zipReader = new ZipReader(path,_file_suffix,_fileName);
            _cbrscene->addWidget(_zipReader);
            _cbrtread = new CbrThread(this);
            connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
            connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
            // use start() instead of run()!!! run() uses main thread resources
            _cbrtread->start();
        }
        // already open file before
        else if(isOpenedBefore==1){
            // destroy the thread first
            _cbrtread->requestInterruption();
            _cbrtread->quit();
            _cbrtread->wait();
            delete _cbrtread;// delete the thread completely
            // otherwise the thread still read the _zipReader
            // delete all the resource of _zipReader first
            if(_zipReader->_Item!=NULL){delete _zipReader->_Item;}
            if(_zipReader->_Item1!=NULL){delete _zipReader->_Item1;}
            if(!_zipReader->_pixmapList.isEmpty()){
                _zipReader->_sizeList.clear();
//                _zipReader->QPixmapCache::clear();
                qDeleteAll(_zipReader->_pixmapList);
                _zipReader->_pixmapList.clear();
            }
//            if(_zipReader->zipR!=NULL){delete _zipReader->zipR;}
            delete _zipReader;
            _zipReader = NULL;
            modeFlag = 0; // reset
            picFlag=0;
            currentPic.clear();
            currentPic.append(picFlag);
            _zipReader = new ZipReader(path,_file_suffix,_fileName);
            _cbrscene->addWidget(_zipReader);
            _cbrtread = new CbrThread(this);
    //        _zipReader->moveToThread(backgroundThread);
    //        _zipReader->moveToThread(_cbrtread);
            connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
            connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
            // use start() instead of run()!!! run() uses main thread resources
            _cbrtread->start();
        }
        index_file_state = 1;
                    //mTreeView->_TreeView->hide();
                    //mTreeView->hide_button->setText("▼");
        while(_zipReader->isReadFinished != 1){}
        ima_TreeView = new FileTree(_zipReader, mTreeView,path,index_file_state);

        connect(ima_TreeView->_TreeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(TreeViewClick_pic()));
        connect(ima_TreeView->_TreeView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(qDebug()<<"click";));

        //QMessageBox::information(NULL,"Success","Success to open file");
    }
    // if the file selected is not regnisable, do something
    else if (_file_suffix.compare("cbr")==0||_file_suffix.compare("rar")==0){
        qDebug()<<"runing rar uncompress";
        QString projectPath = QCoreApplication::applicationDirPath();   // 获取项目路径
        //QString exePath = "/exe/WinRAR.exe";
        QString exePath = projectPath+"WinRAR.exe";
        QString unCompress = mTreeView->_model->fileInfo(index).filePath();;   // 需要解压的文件路径
        QString unCompressTo = projectPath+"/temp/";   // 解压目的地路径 这里因为我想解压到项目的根目录上所以截取了路径。
        qDebug()<<unCompressTo;
        QString cmdExe = "WinRAR.exe";
        QString exePath1 = exePath.replace(QRegExp("/"),"//");
        QString unCompress1 = unCompress.replace(QRegExp("/"),"//");
        QString unCompressTo1 = unCompressTo.replace(QRegExp("/"),"//");
        folder_delete(unCompressTo);

        QStringList cmdList ;
        cmdList.append("x");
        cmdList.append("/Y");
        cmdList.append("-ibck");
        cmdList.append(unCompress1);
        cmdList.append(unCompressTo1);
        QProcess pro ;
        pro.setWorkingDirectory(exePath1);

        if(pro.execute(cmdExe,cmdList) == 0){
            // not opened any file yet
            if(!isOpenedBefore){
                isOpenedBefore = 1;
                // update curentPic imediately, setSlideVal() needs it
                currentPic.clear();
                currentPic.append(0);
                modeFlag = 0; // reset
                _zipReader = new ZipReader(unCompressTo,_file_suffix,_fileName);
                _cbrscene->addWidget(_zipReader);
                _cbrtread = new CbrThread(this);
                connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
                connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
                // use start() instead of run()!!! run() uses main thread resources
                _cbrtread->start();
            }
            // already open file before
            else if(isOpenedBefore==1){
                // destroy the thread first
                _cbrtread->requestInterruption();
                _cbrtread->quit();
                _cbrtread->wait();
                delete _cbrtread;// delete the thread completely
                // otherwise the thread still read the _zipReader
                // delete all the resource of _zipReader first
                if(_zipReader->_Item!=NULL){delete _zipReader->_Item;}
                if(_zipReader->_Item1!=NULL){delete _zipReader->_Item1;}
                if(!_zipReader->_pixmapList.isEmpty()){
                    _zipReader->_sizeList.clear();
    //                _zipReader->QPixmapCache::clear();
                    qDeleteAll(_zipReader->_pixmapList);
                    _zipReader->_pixmapList.clear();
                }
    //            if(_zipReader->zipR!=NULL){delete _zipReader->zipR;}
                delete _zipReader;
                _zipReader = NULL;
                modeFlag = 0; // reset
                picFlag=0;
                currentPic.clear();
                currentPic.append(picFlag);
                _zipReader = new ZipReader(unCompressTo,_file_suffix,_fileName);
                _cbrscene->addWidget(_zipReader);
                _cbrtread = new CbrThread(this);
        //        _zipReader->moveToThread(backgroundThread);
        //        _zipReader->moveToThread(_cbrtread);
                connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
                connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
                // use start() instead of run()!!! run() uses main thread resources
                _cbrtread->start();
            }
            index_file_state = 1;
                        //mTreeView->_TreeView->hide();
                        //mTreeView->hide_button->setText("▼");
            while(_zipReader->isReadFinished != 1){}
            ima_TreeView = new FileTree(_zipReader, mTreeView,path,index_file_state);

            connect(ima_TreeView->_TreeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(TreeViewClick_pic()));
            connect(ima_TreeView->_TreeView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(qDebug()<<"click";));

            //QMessageBox::information(NULL,"Success","Success to open file");

//            index_file_state = 1;
//            //mTreeView->_TreeView->hide();
//            //mTreeView->hide_button->setText("▼");

//            ima_TreeView = new FileTree(_zipReader, mTreeView,path,index_file_state);

//            connect(ima_TreeView->_TreeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(TreeViewClick_pic()));

           // QMessageBox::about(this,"Success","Success to open file");
        }

    }

    else{
        QMessageBox::information(NULL, tr("Warning"), tr("The file is not regnisable!"));
        return;
    }



}


void CbrMainWindow::TreeViewClick_pic()//bug: two page model:click error
{
        QModelIndex index = ima_TreeView->_TreeView->currentIndex();
        qDebug()<<index.row();
        int val = index.row();
        qDebug()<<isOpenedBefore;
        if (isOpenedBefore)
        {
            // avoid "index out of range" error!!!
            if(!_cbrtread->isRunning()){
                if (modeFlag==0)//one pic only
                {
                    if(!_zipReader->_pixmapList.isEmpty()){// first initialize avoid conflict with thread(thread read not finished)
                        this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                        this->_zipReader->_Item->hide();
                        picFlag = val;
                        this->_zipReader->picFlag = this->picFlag;
                        qDebug()<<this->_zipReader->picFlag;
                        //reload
                        this->_zipReader->traceList();

                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        adjust();
                    }
                }
                else if(modeFlag==1)// two pic mode
                {
                    if (picNum==1){}//show one pic only(last one
                    else if (picNum>1 && picNum%2==1 && currentPic.length()==1)//show one pic only(last one
                    {
                        this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                        this->_zipReader->_Item->hide();
                        if (val%2==0) picFlag = val;
                        else picFlag = val-1;
                        this->_zipReader->picFlag = this->picFlag;
                        this->_zipReader->traceList();
                        this->_zipReader->initItem();
                        qDebug()<<this->_zipReader->picFlag;
                        //reload
                        if (val < picNum-1){
                            this->_zipReader->_layout->addWidget(_zipReader->_Item);
                            this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                            currentPic.clear();
                            currentPic.append(picFlag);
                            currentPic.append(picFlag+1);
                            adjust();
                        }
                        else if (val == picNum-1){
                            this->_zipReader->_layout->addWidget(_zipReader->_Item);
                            currentPic.clear();
                            currentPic.append(picFlag);
                            adjust();
                        }
                    }
                    else if (picNum>1 && picNum%2==1 && currentPic.length()==2)//show two pics
                    {
                        this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                        this->_zipReader->_Item->hide();
                        this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                        this->_zipReader->_Item1->hide();
                        if(val%2==0&&val!=picNum-1){
                            picFlag = val;
                            this->_zipReader->picFlag = this->picFlag;
                            this->_zipReader->traceList();
                            this->_zipReader->_layout->addWidget(_zipReader->_Item);
                            this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                            currentPic.clear();
                            currentPic.append(picFlag);
                            currentPic.append(picFlag+1);
                            adjust();
                        }
                        else if(val%2==0&&val==picNum-1){
                            picFlag = val;
                            this->_zipReader->traceList();
                            this->_zipReader->_layout->addWidget(_zipReader->_Item);
                            currentPic.clear();
                            currentPic.append(picFlag);
                            adjust();
                        }
                        else if (val%2==1){
                            picFlag = val -1;
                            this->_zipReader->traceList();
                            this->_zipReader->picFlag = this->picFlag;
                            this->_zipReader->_layout->addWidget(_zipReader->_Item);
                            this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                            currentPic.clear();
                            currentPic.append(picFlag);
                            currentPic.append(picFlag+1);
                            adjust();

                        }
                    }
                    else if (picNum>1 && picNum%2==0){
                        this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                        this->_zipReader->_Item->hide();
                        this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                        this->_zipReader->_Item1->hide();
                        if (val%2==0){
                            picFlag = val;
                        }
                        else picFlag = val -1;
                        this->_zipReader->picFlag = this->picFlag;
                        this->_zipReader->traceList();

                        //reload
                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        currentPic.append(picFlag+1);
                        adjust();
                    }
                }
            }
        }
//    QModelIndex index = ima_TreeView->_TreeView->currentIndex();
//    qDebug()<<index.row();

//    //******** Combination version 1 *****************//
//    if (!this->_zipReader->_ItemList.isEmpty())
//    {
//        if (!_zipReader->_ItemList.isEmpty())
//        {
//            if (modeFlag==0)//one pic only
//            {
//                _zipReader->_layout->removeWidget(_zipReader->_ItemList.at(picFlag));
//                _zipReader->_ItemList.at(picFlag)->hide();
//                picFlag = index.row();

//                //reload
//                _zipReader->_layout->addWidget(_zipReader->_ItemList.at(picFlag));
//                currentPic.clear();
//                currentPic.append(picFlag);
//                adjust();
//            }
//            else // two pic mode
//            {
//                if (picNum>1 && currentPic.length()==1)//show one pic only(last one
//                {
//                    _zipReader->_layout->removeWidget(_zipReader->_ItemList.at(picFlag));
//                    this->_zipReader->_ItemList.at(picFlag)->hide();
//                    picFlag = index.row();

//                    //reload
//                    this->_zipReader->_layout->addWidget(_zipReader->_ItemList.at(picFlag));
//                    this->_zipReader->_layout->addWidget(_zipReader->_ItemList.at(picFlag+1));
//                    currentPic.clear();
//                    currentPic.append(picFlag);
//                    currentPic.append(picFlag+1);
//                    adjust();
//                }
//                else if (picNum==1 && currentPic.length()==1)//show one pic only(last one
//                {
//                    this->_zipReader->_layout->removeWidget(_zipReader->_ItemList.at(picFlag));
//                    this->_zipReader->_ItemList.at(picFlag)->hide();
//                    picFlag = index.row();

//                    //reload
//                    this->_zipReader->_layout->addWidget(_zipReader->_ItemList.at(picFlag));
//                    currentPic.clear();
//                    currentPic.append(picFlag);
//                    adjust();
//                }
//                else if (currentPic.length()==2)
//                {
//                    this->_zipReader->_layout->removeWidget(_zipReader->_ItemList.at(picFlag));
//                    this->_zipReader->_ItemList.at(picFlag)->hide();
//                    this->_zipReader->_layout->removeWidget(_zipReader->_ItemList.at(picFlag+1));
//                    this->_zipReader->_ItemList.at(picFlag+1)->hide();
//                    picFlag = index.row();
//                    //reload
//                    this->_zipReader->_layout->addWidget(_zipReader->_ItemList.at(picFlag));
//                    this->_zipReader->_layout->addWidget(_zipReader->_ItemList.at(picFlag+1));
//                    currentPic.clear();
//                    currentPic.append(picFlag);
//                    currentPic.append(picFlag+1);
//                    adjust();
//                }
//            }
//        }
//    //******** Combination version 1 *****************//
//    }

}

//***************** CHEN Hao_end *****************//

void CbrMainWindow::on_pushButton_4_clicked()//first picture function
{
    if (isOpenedBefore)
    {
        if(!_cbrtread->isRunning()){
            if (modeFlag==0)//one pic only
            {
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                picFlag = 0;
                this->_zipReader->picFlag = this->picFlag;

                // reload in the function adjust()
                currentPic.clear();
                currentPic.append(picFlag);
                adjust();
            }
            else // two pics mode
            {
                if (picNum>1 && currentPic.length()==1)//show one pic only(last one
                {
                    _zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    picFlag = 0;
                    this->_zipReader->picFlag = this->picFlag;

                    // reload in the function adjust()
                    currentPic.clear();
                    currentPic.append(picFlag);
                    currentPic.append(picFlag+1);
                    adjust();
                }
                else if (picNum==1 && currentPic.length()==1)// one pic only
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    picFlag = 0;
                    this->_zipReader->picFlag = this->picFlag;

                    // reload in the function adjust()
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
                else if (currentPic.length()==2)
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    picFlag = 0;
                    this->_zipReader->picFlag = this->picFlag;
                    // reload in the function adjust()
                    currentPic.clear();
                    currentPic.append(picFlag);
                    currentPic.append(picFlag+1);
                    adjust();
                }
            }
        }
    }
}

void CbrMainWindow::on_pushButton_3_clicked()// previous picture function
{
    if (isOpenedBefore)// already open before
    {
        if(!_cbrtread->isRunning()){
            if (modeFlag==0) // one pic view mode
            {
                if(picFlag>0)
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    picFlag -= 1;
                    this->_zipReader->picFlag = this->picFlag;
                    //reload
//                    this->_zipReader->_layout->addWidget(_zipReader->_Item);
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
            }
            else//two pics view mode
            {
                if (picFlag>=2 && picFlag<picNum-1)// two pics showed
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    //reload
                    currentPic.clear();
                    currentPic.append(picFlag-2);
                    currentPic.append(picFlag-1);
                    picFlag -= 2;
                    this->_zipReader->picFlag = this->picFlag;
                    adjust();
                }
                else if (picFlag>=2 && picFlag==picNum-1) // one pic showed
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    //reload
                    currentPic.clear();
                    currentPic.append(picFlag-2);
                    currentPic.append(picFlag-1);
                    picFlag -= 2;
                    this->_zipReader->picFlag = this->picFlag;
                    adjust();
                }
            }
        }
    }
}

void CbrMainWindow::on_pushButton_2_clicked()// next picture function
{
    if (isOpenedBefore) // opened file before
    {
        if(!_cbrtread->isRunning()){
            if (modeFlag==0) // one pic view
            {
                if (picFlag>=0 && picFlag<picNum-1)
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    picFlag += 1;
                    this->_zipReader->picFlag = this->picFlag;

                    // reload in the function adjust()
//                    this->_zipReader->_layout->addWidget(_zipReader->_Item);
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
            }
            else//two pics mode
            {
                if (picFlag<picNum-3)
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    // reload in the function adjust()
                    currentPic.clear();
                    currentPic.append(picFlag+2);
                    currentPic.append(picFlag+3);
                    picFlag += 2;
                    this->_zipReader->picFlag = this->picFlag;
                    adjust();
                }
                else if (picFlag==picNum-3)
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    //reload
                    picFlag += 2;
                    this->_zipReader->picFlag = this->picFlag;
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
            }
        }
        else qDebug()<<"reading not finished";
    }
}
void CbrMainWindow::on_pushButton_clicked()// last picture function
{
    if (isOpenedBefore)// opened file before
    {
        if(!_cbrtread->isRunning()){
            if (modeFlag==0)// one pic only
            {
                this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                this->_zipReader->_Item->hide();
                picFlag = picNum-1;
                this->_zipReader->picFlag = this->picFlag;

                // reload
//                this->_zipReader->_layout->addWidget(_zipReader->_Item);
                currentPic.clear();
                currentPic.append(picFlag);
                adjust();
            }
            else // two pic mode
            {
                if (picNum==1){}// show one pic only, do nothing(last one
                else if (picNum>1 && picNum%2==1 && currentPic.length()==1)//show one pic only(last one
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    picFlag = picNum-1;
                    this->_zipReader->picFlag = this->picFlag;

                    //reload
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
                else if (picNum>1 && picNum%2==1 && currentPic.length()==2)//show one pic only(last one
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    picFlag = picNum-1;
                    this->_zipReader->picFlag = this->picFlag;
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
                else if (picNum>1 && picNum%2==0)
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    picFlag = picNum-2;
                    this->_zipReader->picFlag = this->picFlag;
                    currentPic.clear();
                    currentPic.append(picFlag);
                    currentPic.append(picFlag+1);
                    adjust();
                }
            }
        }
    }
}

void CbrMainWindow::adjust()//fit to window
{
    max_height=this->height();
    max_width = this->width();
//    _cbrscene->addWidget(_zipReader);
    max_size = qMin(max_height,max_height);
    if (_zipReader->_Item!=NULL) {
        delete _zipReader->_Item;
        _zipReader->_Item = NULL;
    }
    if (_zipReader->_Item1!=NULL) {
        delete _zipReader->_Item;
        _zipReader->_Item1 = NULL;
    }
    if (currentPic.length()==1)// one picture only
    {
        _zipReader->_Item = new QLabel(_zipReader);
        _cbrscene->setSceneRect(0,0,max_size,max_size);// must add this one in order to fix window
        _zipReader->_layout->addWidget(_zipReader->_Item);
        _zipReader->_Item->setFixedSize(max_size,max_size);
        if(filterFlag==0){
            _zipReader->_Item->setAlignment(Qt::AlignCenter);
            _zipReader->_Item->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag))->scaled(max_size,max_size,Qt::KeepAspectRatio));
            _zipReader->_Item->show();
        }
        else if (filterFlag==1){
            QPixmap img;
            //img = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag)));
            _zipReader->_Item->setPixmap(img.scaled(max_size,max_size,Qt::KeepAspectRatio));
//            _zipReader->_layout->addWidget(_zipReader->_Item);
            _zipReader->_Item->setAlignment(Qt::AlignCenter);
            _zipReader->_Item->show();
        }
    }
    else// two pictures
    {
        _zipReader->_Item = new QLabel(_zipReader);
        _zipReader->_Item1 = new QLabel(_zipReader);
        this->_zipReader->_layout->addWidget(_zipReader->_Item);
        this->_zipReader->_layout->addWidget(_zipReader->_Item1);
        if(this->height()==max_size)// height is smaller
        {
            if(this->width()>=2*max_size)
            {
                _cbrscene->setSceneRect(0,0,2*max_size,max_size);
                _zipReader->_Item->setFixedSize(max_size,max_size);
                if (filterFlag==0){
                    _zipReader->_Item->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag))->scaled(max_size,max_size,Qt::KeepAspectRatio));
                    _zipReader->_Item1->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1))->scaled(max_size,max_size,Qt::KeepAspectRatio));
                }
                else if (filterFlag==1){
                    QPixmap img1;
                    //img1 = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag)));
                    _zipReader->_Item->setPixmap(img1.scaled(max_size,max_size,Qt::KeepAspectRatio));
//                    _zipReader->_layout->addWidget(_zipReader->_Item);
                    QPixmap img2;
                    //img2 = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1)));
                    _zipReader->_Item1->setPixmap(img2.scaled(max_size,max_size,Qt::KeepAspectRatio));
//                    _zipReader->_layout->addWidget(_zipReader->_Item1);
                }
                _zipReader->_Item->show();
                _zipReader->_Item1->setFixedSize(max_size,max_size);
                _zipReader->_Item1->show();
            }
            else
            {
                _cbrscene->setSceneRect(0,0,this->width(),this->width()/2);
                _zipReader->_Item->setFixedSize(this->width()/2,this->width()/2);
                if(filterFlag==0){
                    _zipReader->_Item->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag))->scaled(max_size,max_size,Qt::KeepAspectRatio));
                    _zipReader->_Item1->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1))->scaled(max_size,max_size,Qt::KeepAspectRatio));
                }
                else if (filterFlag==1){
                    QPixmap img1;
                    //img1 = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag)));
                    _zipReader->_Item->setPixmap(img1.scaled(max_size,max_size,Qt::KeepAspectRatio));
//                    _zipReader->_layout->addWidget(_zipReader->_Item);
                    QPixmap img2;
                    //img2 = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1)));
                    _zipReader->_Item1->setPixmap(img2.scaled(max_size,max_size,Qt::KeepAspectRatio));
//                    _zipReader->_layout->addWidget(_zipReader->_Item1);
                }
                _zipReader->_Item->show();
                _zipReader->_Item1->setFixedSize(this->width()/2,this->width()/2);
                _zipReader->_Item1->show();
            }
        }
        else// width is smaller
        {
            _cbrscene->setSceneRect(0,0,this->width(),this->width()/2);
            _zipReader->_Item->setFixedSize(this->width()/2,this->width()/2);
            if (filterFlag==0){
                _zipReader->_Item->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag))->scaled(max_size,max_size,Qt::KeepAspectRatio));
                _zipReader->_Item1->setPixmap(_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1))->scaled(max_size,max_size,Qt::KeepAspectRatio));
            }
            else if(filterFlag==1){
                QPixmap img1;
                //img1 = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag)));
                _zipReader->_Item->setPixmap(img1.scaled(max_size,max_size,Qt::KeepAspectRatio));
//                _zipReader->_layout->addWidget(_zipReader->_Item);
                QPixmap img2;
                //img2 = filter(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1)));
                _zipReader->_Item1->setPixmap(img2.scaled(max_size,max_size,Qt::KeepAspectRatio));
//                _zipReader->_layout->addWidget(_zipReader->_Item1);
            }
            _zipReader->_Item->show();
            _zipReader->_Item1->setFixedSize(this->width()/2,this->width()/2);
            _zipReader->_Item1->show();
        }
        _zipReader->_Item->setAlignment(Qt::AlignRight);
        _zipReader->_Item1->setAlignment(Qt::AlignLeft);
//        _zipReader->_layout->addWidget(_zipReader->_Item);
//        _zipReader->_layout->addWidget(_zipReader->_Item1);
        _zipReader->_layout->setSpacing(10);
    }
    _zipReader->adjustSize();// need to resize
    // emit signal
    emit updatePicSignal();
}

void CbrMainWindow::on_action_Open_triggered()// open action
{

    QString path = getPath();
    if (path =="select none") return;

    if(_file_suffix.compare("zip")!=0&&_file_suffix.compare("cbz")!=0&&_file_suffix.compare("cbr")!=0&&
                    _file_suffix.compare("rar")!=0){qDebug()<<"break"; return;}


    if(index_file_state == 1 ){delete ima_TreeView->_TreeView;
    delete ima_TreeView->hide_button;}

    if (_file_suffix.compare("zip")==0||_file_suffix.compare("cbz")==0||
            _file_suffix.compare("jpg")==0||
            _file_suffix.compare("jpeg")==0||_file_suffix.compare("bmp")==0||
            _file_suffix.compare("png")==0)
    {
        // not opened any file yet
        if(!isOpenedBefore){
            isOpenedBefore = 1;
            // update curentPic imediately, setSlideVal() needs it
            currentPic.clear();
            currentPic.append(0);
            modeFlag = 0; // reset
            _zipReader = new ZipReader(path,_file_suffix,_fileName);
            _cbrscene->addWidget(_zipReader);
            _cbrtread = new CbrThread(this);
    //        _zipReader->moveToThread(backgroundThread);
    //        _zipReader->moveToThread(_cbrtread);
            connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
            connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
            // use start() instead of run()!!! run() uses main thread resources
            _cbrtread->start();
    //        while(_zipReader->isReadFinished)
    //            QCoreApplication::processEvents();
        }
        // already open file before
        else if(isOpenedBefore==1){
            // destroy the thread first
            _cbrtread->requestInterruption();
            _cbrtread->quit();
            _cbrtread->wait();
            delete _cbrtread;// delete the thread completely
            // otherwise the thread still read the _zipReader
            // delete all the resource of _zipReader first
            if(_zipReader->_Item!=NULL){delete _zipReader->_Item;}
            if(_zipReader->_Item1!=NULL){delete _zipReader->_Item1;}
            if(!_zipReader->_pixmapList.isEmpty()){
                _zipReader->_sizeList.clear();
//                _zipReader->QPixmapCache::clear();
                qDeleteAll(_zipReader->_pixmapList);
                _zipReader->_pixmapList.clear();
            }
//            if(_zipReader->zipR!=NULL){delete _zipReader->zipR;}
            delete _zipReader;
            _zipReader = NULL;
            modeFlag = 0; // reset
            picFlag=0;
            currentPic.clear();
            currentPic.append(picFlag);
            _zipReader = new ZipReader(path,_file_suffix,_fileName);
            _cbrscene->addWidget(_zipReader);
            _cbrtread = new CbrThread(this);
    //        _zipReader->moveToThread(backgroundThread);
    //        _zipReader->moveToThread(_cbrtread);
            connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
            connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
            // use start() instead of run()!!! run() uses main thread resources
            _cbrtread->start();
        }
        index_file_state = 1;
                    //mTreeView->_TreeView->hide();
                    //mTreeView->hide_button->setText("▼");
        while(_zipReader->isReadFinished != 1){}
        ima_TreeView = new FileTree(_zipReader, mTreeView,path,index_file_state);

        connect(ima_TreeView->_TreeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(TreeViewClick_pic()));
    }
    else if (_file_suffix.compare("cbr")==0||_file_suffix.compare("rar")==0){
        qDebug()<<"runing rar uncompress";
        QString projectPath = QCoreApplication::applicationDirPath();   // 获取项目路径
        //QString exePath = "/exe/WinRAR.exe";
        QString exePath = projectPath+"WinRAR.exe";
        //QString unCompress = mTreeView->_model->fileInfo(index).filePath();;   // 需要解压的文件路径
        QString unCompressTo = projectPath+"/temp/";   // 解压目的地路径 这里因为我想解压到项目的根目录上所以截取了路径。
        qDebug()<<unCompressTo;
        QString cmdExe = "WinRAR.exe";
        QString exePath1 = exePath.replace(QRegExp("/"),"//");
        QString unCompress1 = path.replace(QRegExp("/"),"//");
        QString unCompressTo1 = unCompressTo.replace(QRegExp("/"),"//");
        folder_delete(unCompressTo);

        QStringList cmdList ;
        cmdList.append("x");
        cmdList.append("/Y");
        cmdList.append("-ibck");
        cmdList.append(unCompress1);
        cmdList.append(unCompressTo1);
        QProcess pro ;
        pro.setWorkingDirectory(exePath1);

        if(pro.execute(cmdExe,cmdList) == 0){
            // not opened any file yet
            if(!isOpenedBefore){
                isOpenedBefore = 1;
                // update curentPic imediately, setSlideVal() needs it
                currentPic.clear();
                currentPic.append(0);
                modeFlag = 0; // reset
                _zipReader = new ZipReader(unCompressTo,_file_suffix,_fileName);
                _cbrscene->addWidget(_zipReader);
                _cbrtread = new CbrThread(this);
                connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
                connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
                // use start() instead of run()!!! run() uses main thread resources
                _cbrtread->start();
            }
            // already open file before
            else if(isOpenedBefore==1){
                // destroy the thread first
                _cbrtread->requestInterruption();
                _cbrtread->quit();
                _cbrtread->wait();
                delete _cbrtread;// delete the thread completely
                // otherwise the thread still read the _zipReader
                // delete all the resource of _zipReader first
                if(_zipReader->_Item!=NULL){delete _zipReader->_Item;}
                if(_zipReader->_Item1!=NULL){delete _zipReader->_Item1;}
                if(!_zipReader->_pixmapList.isEmpty()){
                    _zipReader->_sizeList.clear();
    //                _zipReader->QPixmapCache::clear();
                    qDeleteAll(_zipReader->_pixmapList);
                    _zipReader->_pixmapList.clear();
                }
    //            if(_zipReader->zipR!=NULL){delete _zipReader->zipR;}
                delete _zipReader;
                _zipReader = NULL;
                modeFlag = 0; // reset
                picFlag=0;
                currentPic.clear();
                currentPic.append(picFlag);
                _zipReader = new ZipReader(unCompressTo,_file_suffix,_fileName);
                _cbrscene->addWidget(_zipReader);
                _cbrtread = new CbrThread(this);
        //        _zipReader->moveToThread(backgroundThread);
        //        _zipReader->moveToThread(_cbrtread);
                connect(_zipReader,SIGNAL(displaySignal()),this,SLOT(displayPic()));
                connect(this,SIGNAL(updatePicSignal()),_cbrtread,SLOT(updatePicSlot()));
                // use start() instead of run()!!! run() uses main thread resources
                _cbrtread->start();
            }
            index_file_state = 1;
                        //mTreeView->_TreeView->hide();
                        //mTreeView->hide_button->setText("▼");
            while(_zipReader->isReadFinished != 1){}

            ima_TreeView = new FileTree(_zipReader, mTreeView,path,index_file_state);

            connect(ima_TreeView->_TreeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(TreeViewClick_pic()));
            connect(ima_TreeView->_TreeView,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(qDebug()<<"click";));

            //QMessageBox::information(NULL,"Success","Success to open file");

//            index_file_state = 1;
//            //mTreeView->_TreeView->hide();
//            //mTreeView->hide_button->setText("▼");

//            ima_TreeView = new FileTree(_zipReader, mTreeView,path,index_file_state);

//            connect(ima_TreeView->_TreeView,SIGNAL(clicked(const QModelIndex)),this,SLOT(TreeViewClick_pic()));

           // QMessageBox::about(this,"Success","Success to open file");
        }

    }
    // if the file selected is not regnisable, do something
    else{
        QMessageBox::information(NULL, tr("Warning"), tr("The file is not regnisable!"));
        return;
    }

}
void CbrMainWindow::keyPressEvent(QKeyEvent *ev)
{
    //ev->modifiers();
    if(ev->key()==Qt::Key_A)
    {
        on_pushButton_3_clicked();
    }
    if(ev->key()==Qt::Key_D)
    {
        on_pushButton_2_clicked();
    }
    if(ev->modifiers()==Qt::ControlModifier)// whether wiht control, shift...
           {
               if(ev->key()==Qt::Key_Equal)// ctrl +: zoom in
               {
                     on_pushButton_6_clicked();
               }
               if(ev->key()==Qt::Key_Minus)// ctrl -: zoom out
               {
                     on_pushButton_5_clicked();
               }
               if(ev->key()==Qt::Key_S) // ctrl S: save action
               {
                     on_action_Save_triggered();
               }
               if(ev->key()==Qt::Key_O) // open action
               {
                     on_action_Open_triggered();
               }

           }
}

void CbrMainWindow::wheelEvent(QWheelEvent *ev)
{
    if(ev->modifiers()==Qt::ControlModifier)
    {
        if(ev->delta()>0)// ctrl wheel up: zoom out
        {
            on_pushButton_6_clicked();
        }
        else{on_pushButton_5_clicked();}// ctrl wheel down: zoom in
    }
}
void CbrMainWindow::mousePressEvent(QMouseEvent*ev)
{
    if(ev->buttons()==Qt::RightButton)// if is right click
    {ui->menu_File->exec(QCursor::pos());}// execute file menu
}
void CbrMainWindow::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
    {
        if(this->isHidden())
        {
            this->show();
            this->resize(width_tmp,height_tmp);
            this->setWindowState(Qt::WindowActive);
            this->activateWindow();
        }
        else
        {
            width_tmp = this->width();
            height_tmp = this->height();
            this->hide();
        }
    }
}

void CbrMainWindow::on_pushButton_6_clicked()// zoom in function
{
    if(isOpenedBefore){
        ui->graphicsView->scale(1.2,1.2);
        zoomFactor = zoomFactor*1.2;
        ui->graphicsView->setFactor(1);
    }
}

void CbrMainWindow::on_pushButton_5_clicked()//zoom out function
{
    if (isOpenedBefore){
        ui->graphicsView->scale(1/1.2,1/1.2);
        zoomFactor = zoomFactor/1.2;
        ui->graphicsView->setFactor(-1);
    }
}


void CbrMainWindow::on_pushButton_7_clicked()// change view mode function
{
    if (isOpenedBefore){
        if (modeFlag ==0)// one to two pics
        {
            modeFlag = 1;
            if (picFlag%2==1)//
            {
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                picFlag -= 1;
                this->_zipReader->picFlag = this->picFlag;
                currentPic.clear();
                currentPic.append(picFlag);
                currentPic.append(picFlag+1);
                adjust();
            }
            else if(picFlag%2==0 && picFlag<picNum-1)
            {
                //delete
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                //reload
                _zipReader->_layout->addWidget(_zipReader->_Item);
                _zipReader->_layout->addWidget(_zipReader->_Item1);
                currentPic.clear();
                currentPic.append(picFlag);
                currentPic.append(picFlag+1);
                adjust();
            }
            else if(picFlag%2==0 && picFlag==picNum-1){}
        }
        else// two pics to one pic
        {
            modeFlag = 0;
            if (picFlag<picNum-1)// two photo showed
            {
                // we delete all photos first
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                _zipReader->_layout->removeWidget(_zipReader->_Item1);
                _zipReader->_Item1->hide();
                // reload again
                _zipReader->_layout->addWidget(_zipReader->_Item);
                _zipReader->_Item->setAlignment(Qt::AlignCenter);
                currentPic.clear();
                currentPic.append(picFlag);
                adjust();
            }
            else if (picFlag == picNum-1)
            {
                // delete first
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                // reload again
                _zipReader->_layout->addWidget(_zipReader->_Item);
                _zipReader->_Item->setAlignment(Qt::AlignCenter);
                currentPic.clear();
                currentPic.append(picFlag);
                adjust();
            }
        }
    }
}

void CbrMainWindow::on_action_Exit_triggered()// close action
{
    this->close();
}

QString CbrMainWindow::getPath()
{
    QFileDialog* fileOpenDlg = new QFileDialog;
    QString strPath = fileOpenDlg->getOpenFileName(NULL,
                                 "Select file to open",
                                 _strDirOpen,
                                 "zip/cbz file (*.zip;*.cbz);;cbr file(*.cbr);;rar (*.rar)"
                                 "jpg/jpeg (*.jpg);;png (*.png);;bmp (*.bmp);;All (*)");
    if(strPath.isEmpty())
    {
        return "select none";
    }
    delete fileOpenDlg;
    fileOpenDlg=NULL;
    QFileInfo fileInfo(strPath); // obtain file info
    _file_suffix = fileInfo.suffix();
    _strDirOpen = fileInfo.filePath();// record diretory info
    _fileName = fileInfo.fileName();
    return strPath;
}

void CbrMainWindow::on_pushButton_8_clicked()// auto fix
{
    if (isOpenedBefore){
        qreal val = pow(1.2, -ui->graphicsView->getFactor());
        ui->graphicsView->scale(val,val);
        ui->graphicsView->setFactor(0);
    }
}

void CbrMainWindow::setSliderVal(){
    if(isOpenedBefore){
//        ui->horizontalSlider->setMaximum(picNum-1);
        ui->horizontalSlider->setMinimum(0);
        ui->horizontalSlider->setValue(this->currentPic.at(this->currentPic.length()-1));
    }
}

void CbrMainWindow::jump2Pic(){
    int val = ui->horizontalSlider->value();
    if (isOpenedBefore)
    {
        // avoid "index out of range" error!!!
        if(!_cbrtread->isRunning()&&_zipReader->_currentPicList.indexOf(val)!=-1){
            if (modeFlag==0)//one pic only
            {
                if(!_zipReader->_pixmapList.isEmpty()){// first initialize avoid conflict with thread(thread read not finished)
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    picFlag = val;
                    this->_zipReader->picFlag = this->picFlag;
                    //reload
                    this->_zipReader->_layout->addWidget(_zipReader->_Item);
                    currentPic.clear();
                    currentPic.append(picFlag);
                    adjust();
                }
            }
            else if(modeFlag==1)// two pic mode
            {
                if (picNum==1){}//show one pic only(last one
                else if (picNum>1 && picNum%2==1 && currentPic.length()==1)//show one pic only(last one
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    if (val%2==0) picFlag = val;
                    else picFlag = val-1;
                    this->_zipReader->picFlag = this->picFlag;

                    //reload
                    if (val < picNum-1){
                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        currentPic.append(picFlag+1);
                        adjust();
                    }
                    else if (val == picNum-1){
                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        adjust();
                    }
                }
                else if (picNum>1 && picNum%2==1 && currentPic.length()==2)//show two pics
                {
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    if(val%2==0&&val!=picNum-1){
                        picFlag = val;
                        this->_zipReader->picFlag = this->picFlag;
                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        currentPic.append(picFlag+1);
                        adjust();
                    }
                    else if(val%2==0&&val==picNum-1){
                        picFlag = val;
                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        adjust();
                    }
                    else if (val%2==1){
                        picFlag = val -1;
                        this->_zipReader->picFlag = this->picFlag;
                        this->_zipReader->_layout->addWidget(_zipReader->_Item);
                        this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                        currentPic.clear();
                        currentPic.append(picFlag);
                        currentPic.append(picFlag+1);
                        adjust();

                    }
                }
                else if (picNum>1 && picNum%2==0){
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item);
                    this->_zipReader->_Item->hide();
                    this->_zipReader->_layout->removeWidget(_zipReader->_Item1);
                    this->_zipReader->_Item1->hide();
                    if (val%2==0){
                        picFlag = val;
                    }
                    else picFlag = val -1;
                    this->_zipReader->picFlag = this->picFlag;

                    //reload
                    this->_zipReader->_layout->addWidget(_zipReader->_Item);
                    this->_zipReader->_layout->addWidget(_zipReader->_Item1);
                    currentPic.clear();
                    currentPic.append(picFlag);
                    currentPic.append(picFlag+1);
                    adjust();
                }
            }
        }
    }
}
CbrThread::CbrThread(CbrMainWindow *pWindow) : QThread() {
    _MainWindow = pWindow;
}
void CbrThread::run(){
    if(picFlag!=_MainWindow->_zipReader->picFlag){
        _MainWindow->_zipReader->initItem();
        picFlag = _MainWindow->_zipReader->picFlag;
    }
}
void CbrThread::updatePicSlot(){
    this->start();
}
CbrThread::~CbrThread(){
    this->quit();
    this->wait();
}

void CbrMainWindow::on_pushButton_9_clicked()// filter action
{
    if(isOpenedBefore){
        if (filterFlag ==0)// change to filter
        {
            filterFlag = 1;
            if (currentPic.length()==1)//
            {
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                //reload
                adjust();
            }
            else if (currentPic.length()==2){
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                _zipReader->_layout->removeWidget(_zipReader->_Item1);
                _zipReader->_Item1->hide();
//                _zipReader->_Item->clear();
//                _zipReader->_Item1->clear();
                adjust();
            }
        }
        else if(filterFlag==1)// filter to normal
        {
            filterFlag = 0;
            if (currentPic.length()==1)//
            {
                // we delete all photos first
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                // reload again
//                _zipReader->_Item->setAlignment(Qt::AlignCenter);
                adjust();
            }
            else if (currentPic.length()==2)
            {
                // we delete all photos first
                _zipReader->_layout->removeWidget(_zipReader->_Item);
                _zipReader->_Item->hide();
                _zipReader->_layout->removeWidget(_zipReader->_Item1);
                _zipReader->_Item1->hide();
                // reload again
//                _zipReader->_Item->setPixmap(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag)));
//                _zipReader->_layout->addWidget(_zipReader->_Item);
//                _zipReader->_Item->setAlignment(Qt::AlignLeft);
//                _zipReader->_Item1->setPixmap(*_zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1)));
//                _zipReader->_layout->addWidget(_zipReader->_Item1);
//                _zipReader->_Item1->setAlignment(Qt::AlignRight);
                adjust();
            }
        }
    }
}

//QPixmap CbrMainWindow::filter(QPixmap inPixmap)
//{
//    Mat cvMat = ASM::QPixmapToCvMat( inPixmap, false );
//    Mat grayImage;
//    cv::cvtColor(cvMat, grayImage, CV_BGR2GRAY);
//    cv::medianBlur(grayImage, grayImage, 7);
//    Mat edge;
//    cv::Laplacian(grayImage, edge, CV_8U, 5);
//    Mat dstImage;
//    cv::threshold(edge, dstImage, 127, 255, THRESH_BINARY_INV);// >127 ? 0:255,用黑色描绘轮廓
//    QPixmap res = ASM::cvMatToQPixmap( dstImage );
//    return res;
//}
void CbrMainWindow::displayPic(){// show fisrt picture imediately
    _zipReader->_Item->setPixmap(*_zipReader->_pixmapList.at(0));
    _zipReader->_layout->addWidget(_zipReader->_Item);
    _zipReader->_Item->setAlignment(Qt::AlignCenter);
    currentPic.clear();
    currentPic.append(0);
    if (!isOpenedBefore){}// already open file before
    this->picNum = _zipReader->_picNum;
    picFlag = 0;// important, otherwise clicked on_pushButton_2_clicked it still there
    this->_zipReader->picFlag = this->picFlag;
    adjust(); // adjust first pic
}

void CbrMainWindow::on_action_Save_triggered() // save action
{
    if (isOpenedBefore){
        if (currentPic.length()==1)// one picture only
        {
            QString filename1 = QFileDialog::getSaveFileName(this,tr("Save Image"),"",tr("Images (*.png *.bmp *.jpg)")); // choose path
            _zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag))->save(filename1);
        }
        else if(currentPic.length()==2)
        {
            QString filename1 = QFileDialog::getSaveFileName(this,tr("Save Image"),"",tr("Images (*.png *.bmp *.jpg)")); // choose path
            _zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag))->save(filename1);
            QString filename2 = QFileDialog::getSaveFileName(this,tr("Save Image"),"",tr("Images (*.png *.bmp *.jpg)")); // choose path
            _zipReader->_pixmapList.at(_zipReader->_currentPicList.indexOf(picFlag+1))->save(filename2);
        }
    }
}


void CbrMainWindow::on_pushButton_10_clicked()
{
    if(hide_index == false){
        ui->horizontalLayout_2->setStretch(0,1);
        ui->horizontalLayout_2->setStretch(1,60);
        ui->pushButton_10->setText(">");
        mTreeView->_TreeView->hide();
        if(index_file_state == 1){
            ima_TreeView->_TreeView->hide();
            ima_TreeView->hide_button->hide();
        }
        hide_index = true;
    }
    else{
        ui->horizontalLayout_2->setStretch(0,1);
        ui->horizontalLayout_2->setStretch(1,6);
        ui->pushButton_10->setText("<");
        mTreeView->_TreeView->show();
        if(index_file_state == 1){
            ima_TreeView->_TreeView->show();
            ima_TreeView->hide_button->show();

        }
        hide_index = false;
    }

}
