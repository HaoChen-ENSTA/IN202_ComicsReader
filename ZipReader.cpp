#include "ZipReader.h"
#include <QMessageBox>
#include <QLabel>
#include <qDebug>
#include <QDir>
#include <QString>
#include <QFileDialog>
#include "quazip/JlCompress.h"
#include <QThread>
#include <QPixmapCache>
//#include <QUnrar>
#include <sstream>

ZipReader::ZipReader(QString pthStr,QString file_suffix, QString fileName, QWidget *parent) : QWidget(parent),
  _pathStr(pthStr), _file_suffix(file_suffix),_fileName(fileName),m_nBuffSize(0), m_pBuff(NULL)
{
    // initialization
    _layout = new QHBoxLayout(this);
    _Item = new QLabel(this);
    _Item1 = new QLabel(this);
    _file_suffix = file_suffix;
    _pathStr = pthStr;
    _fileName = fileName;
    isFirst =1;
    picFlag = 0;
    isThread=1;
    this->adjustSize();
//    setStyleSheet(tr("background-image: url(../ComicsBookReader/qrc/background.png)"));
    this->setStyleSheet("background-color:#E0E0E0");//the same with _cbrscene color
}

// obtain some information necessory
void ZipReader::init(){
    if(_file_suffix.compare("cbz")==0||_file_suffix.compare("zip")==0)
    {
        zipR = new QuaZip(_pathStr);
        //QtRAR file_com(_pathStr);
        //if(!file_com.open(QtRAR::OpenModeExtract)){
        if(!zipR->open(QuaZip::mdUnzip)){
            return;  //Erreur, on se casse
        }
        QList<QuaZipFileInfo> info;
        //QList<QtRARFileInfo> info;
        info = zipR->getFileInfoList ();
        //info = file_com.fileInfoList();
        //_fileNameList = file_com.fileNameList();
        _fileNameList = zipR->getFileNameList();
        _picNum = info.count();
    }
    if(_file_suffix.compare("cbr")==0||_file_suffix.compare("rar")==0)
    {
        //************* CHEN Hao: modify beg ******************//
        QStringList filters;
        filters << "*.jpg"<<"*.png";//设置过滤类型
        //QFileInfoList fileList;
        _dir = new QDir(_pathStr);

 //       _dir->setNameFilters(filters);
        qDebug()<<_dir;
        //QFileInfo curFile;
        if(!_dir->exists())  {qDebug()<<"!EXIST";return ;}//文件不存，则返回false
        //QList<QFileInfo> info;
        _fileList=_dir->entryInfoList(QDir::Dirs|QDir::Files
                                          |QDir::Readable|QDir::Writable
                                          |QDir::Hidden|QDir::NoDotAndDotDot
                                          ,QDir::Name);
            //info = _dir.entryInfoList();
        if(_fileList[0].isDir()){
            //_dir->cleanPath();
            qDebug()<<_fileList.at(0).filePath();
            _dir = new QDir(_fileList.at(0).filePath());
            qDebug()<<_dir->path();
            _fileList.clear();
            _fileList=_dir->entryInfoList(QDir::Dirs|QDir::Files|QDir::Readable|QDir::Writable
                                          |QDir::Hidden|QDir::NoDotAndDotDot,QDir::Name);

            qDebug()<<_fileList;
        }
        _picNum = _fileList.size();

       //************* CHEN Hao: modify end ******************//

    }
    else if(_file_suffix.compare("jpg")==0||_file_suffix.compare("jpeg")==0||
            _file_suffix.compare("bmp")==0||_file_suffix.compare("png")==0){
        qDebug()<<_fileName;
        _fileNameList.append(_fileName);
        _picNum = 1;
    }
    else qDebug()<<"file is not regnisable";
    qDebug()<<"picnum"<<_picNum;
}

void ZipReader::initItem()
{
    isReadFinished=0;
    if(isThread){
        init();
        isThread=0;
    }
    if(!_pixmapList.isEmpty()){
        _sizeList.clear();
        QPixmapCache::clear();
        qDeleteAll(_pixmapList);
        _pixmapList.clear();
    }
    qDebug()<<"picflag is"<<picFlag;
    traceList();
    if(_file_suffix.compare("cbz")==0||_file_suffix.compare("zip")==0){
       QList<int>::iterator i;
       QuaZipFile file(zipR);

        //QtRARFile file(file_com);
        for(i=_currentPicList.begin();i!=_currentPicList.end();i++)
        {
            zipR->setCurrentFile(_fileNameList.at(*i));
            if (!file.open(QIODevice::ReadOnly)){
                qDebug()<<"open pics fail";
                return;
            }
            img=new QPixmap;
            QDataStream mp(&file);
            if (m_pBuff != NULL)
            {
                delete m_pBuff;
                m_pBuff = NULL;
            }
            int nSize = file.size();
            do
            {
                m_pBuff = new char[nSize + 1];
            }while (m_pBuff == NULL);
            m_nBuffSize = mp.readRawData(m_pBuff, nSize);
            // if reading fail, return
            if (m_nBuffSize == -1)
            {
                return;
            }
            img->loadFromData(reinterpret_cast<unsigned char*>(m_pBuff), m_nBuffSize);
            _pixmapList.append(img);
            file.close();
    //        delete img;
            if (isFirst){
                isFirst=0;
                emit displaySignal();// display first pic
            }
        }
    }
    else if(_file_suffix.compare("cbr")==0||_file_suffix.compare("rar")==0){
        QList<int>::iterator i;
        qDebug()<<_dir->path();

        qDebug()<<_fileList;

        for(i=_currentPicList.begin();i!=_currentPicList.end();i++)
        {
            QFile file(_fileList[*i].filePath());
            _dir->setCurrent(_fileList[*i].filePath());
            qDebug()<<_fileList[*i].filePath();
            qDebug()<<file;
            if (!file.open(QIODevice::ReadOnly)){
                qDebug()<<"open pics fail";
                return;
            }
            img=new QPixmap;
            QDataStream mp(&file);
            if (m_pBuff != NULL)
            {
                delete m_pBuff;
                m_pBuff = NULL;
            }
            int nSize = file.size();
            do
            {
                m_pBuff = new char[nSize + 1];
            }while (m_pBuff == NULL);
            m_nBuffSize = mp.readRawData(m_pBuff, nSize);
            // if reading fail, return
            if (m_nBuffSize == -1)
            {
                return;
            }
            img->loadFromData(reinterpret_cast<unsigned char*>(m_pBuff), m_nBuffSize);
            _pixmapList.append(img);
            file.close();
    //        delete img;
            if (isFirst){
                isFirst=0;
                emit displaySignal();// display first pic
            }
        }
    }
    else if (_file_suffix.compare("jpg")==0||_file_suffix.compare("jpeg")==0||
             _file_suffix.compare("bmp")==0||_file_suffix.compare("png")==0){
        img=new QPixmap;
        img->load(_pathStr);
        _pixmapList.append(img);
        _currentPicList.clear();
        _currentPicList.append(0);
        if (isFirst){
            isFirst=0;
            emit displaySignal();// display first pic
        }
    }

    qDebug()<<"read over";
    isReadFinished = 1;
}

void ZipReader::traceList(){
    _currentPicList.clear();
    if(_picNum<=10){
        for(int i=0;i<=_picNum-1;i++){
            _currentPicList.append(i);
        }
    }
    else{
        if(4<=picFlag && picFlag<=_picNum-6){
            _currentPicList = {0,1,picFlag-2,picFlag-1,picFlag,picFlag+1,picFlag+2,picFlag+3,_picNum-2,_picNum-1};
        }
        else if(picFlag<4){
            for(int i=0;i<=picFlag+3;i++){
                _currentPicList.append(i);
            }
            _currentPicList.append(_picNum-2);
            _currentPicList.append(_picNum-1);
        }
        else if(picFlag>_picNum-6){
            _currentPicList.append(0);
            _currentPicList.append(1);
            for(int i=picFlag-2;i<=_picNum-1;i++){
                _currentPicList.append(i);
            }
        }
    }
    qDebug()<<"current Pic list is "<<_currentPicList;
}
