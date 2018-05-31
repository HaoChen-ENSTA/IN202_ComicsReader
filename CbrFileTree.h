#ifndef CBRFILETREE_H
#define CBRFILETREE_H

#include <QGraphicsView>

#include "CbrFolderTree.h"
#include <QIcon>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QStringList>
#include <QLabel>
#include <ZipReader.h>
#include <QStandardItem>
#include <QStandardItemModel>

class FileTree : public FolderTree
{
    Q_OBJECT
public:
    FolderTree* _mTreeView;

    QString _file_path;
    int _index_file_state;

    explicit FileTree( FolderTree* mTreeView);
    FileTree(ZipReader* zipReader,FolderTree* mTreeView, QString file_path , int _index_filer_state = 0);
    FileTree(FolderTree* mTreeView, QString file_path , int _index_filer_state = 0);
    //~FileTree();

    //void TreeViewSet(QString file_path,int index_file_state);
    void PositionSet();
    void FileOriginal();
    void TreeViewSet();
    void treeviewshow();
    void FileOriginal_rar(QString file_path,int index_file_state);
    void TreeViewSet_rar(QString folder_path,int index_folder_state);
    //void FileOriginal(QString folder_path,int index_open);

    //QImage img =new QImage;
    ZipReader* _zipReader;
    QStandardItemModel* _model;
    QFileSystemModel* rar_model;
private:
    QStringList m_imgList;
    void namefilterset();


private slots:
    void showDetailInfo();
    //void TreeViewClick_pic();

};



#endif // CBRFILETREE_H
