#ifndef ZIPREADER_H
#define ZIPREADER_H

#include <QWidget>
#include <QLabel>
#include <QList>
#include <QHBoxLayout>
#include <QDir>
#include <QStringList>
#include <QString>
#include "quazip/JlCompress.h"
//#include <src/qtrar_global.h>
//#include <src/qtrar.h>
//#include <src/qtrarfile.h>
//#include <src/qtrarfileinfo.h>

class ZipReader : public QWidget
{
    Q_OBJECT
public:
    explicit ZipReader(QString pthStr,QString file_suffix, QString fileName, QWidget *parent = nullptr);
    QString _pathStr;
    QDir* _dir;
    QString _file_suffix;
    QStringList _picList;
    QLabel *_Item;
    QLabel *_Item1;
    QPixmap *img;
    QList <QPixmap *> _pixmapList;
    int _picNum;
    QList<QString > _fileNameList;
    QHBoxLayout *_layout;
    QString _strDirOpen;
    QList<int> _sizeList;
    int isReadFinished;
    int picFlag=0;
    int isFirst;
    QString getPath();
    int m_nBuffSize; /**<  有效数据长度 */
    char* m_pBuff; /**< 数据 */
    void initItem();
    int getFileNum();
    QStringList getFileList();
    void traceList();
    void init();
    QuaZip* zipR;
    //QtRAR* file_com;
    QList<int> _currentPicList;
    int isThread=1;
    QString _fileName;
    QFileInfoList _fileList;
private:

signals:
    void displaySignal();

public slots:
};

#endif // ZIPREADER_H
