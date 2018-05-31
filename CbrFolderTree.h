#ifndef CBRFOLDERTREE_H
#define CBRFOLDERTREE_H


#include <QWidget>
#include <QMainWindow>

#include <QFileSystemModel>
#include <QTreeView>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QPixmap>

//#include "quazipdir.h"

class FolderTree : public QWidget
{
    Q_OBJECT
public:
    QTreeView* _TreeView = new QTreeView();
    QPushButton * hide_button = new QPushButton("▲");

    QString _folder_path;
    int _index_folder_state;
    int _index_file_state;

    QGridLayout* _layout = new QGridLayout;

    explicit FolderTree(QWidget *parent = 0);
    FolderTree(QGridLayout* layout);
    FolderTree(QGridLayout* layout,QString folder_path, int _index_folder_state = 0);
    //~FileTree();

    void FileOriginal(QString folder_path,int index_folder_state);
    void TreeViewSet(QString folder_path,int index_folder_state);
    void PositionSet();
    QStringList _nameFilter;
    QFileSystemModel* _model = new QFileSystemModel();

    QPixmap img;

private:
    void namefilterset();

private slots:
   // void TreeViewDoubleClick();


};

#endif // CBRFILETREE_H

