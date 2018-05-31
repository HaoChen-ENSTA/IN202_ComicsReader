#include "CbrFolderTree.h"
#include "ui_CbrMainWindow.h"
#include <QDebug>

FolderTree::FolderTree(QWidget *parent):
    QWidget(parent)
{
    namefilterset();
    FileOriginal(_folder_path,_index_folder_state);
    TreeViewSet(_folder_path,_index_folder_state);
    PositionSet();
}

FolderTree::FolderTree(QGridLayout* layout):
    _layout(layout)
{
    namefilterset();
    FileOriginal(_folder_path,_index_folder_state);
    TreeViewSet(_folder_path,_index_folder_state);
    PositionSet();
}

FolderTree::FolderTree(QGridLayout* layout, QString folder_path, int index_folder_state):
    _layout(layout),_folder_path(folder_path),_index_folder_state(index_folder_state)
{
    namefilterset();
    FileOriginal(_folder_path,_index_folder_state);
    TreeViewSet(_folder_path,_index_folder_state);
    PositionSet();
}

void FolderTree::FileOriginal(QString folder_path,int index_open) //
{

    //si fishier nouveau est ouverte, modifier l'address
    if(_index_folder_state == 1){
        _model->setRootPath(folder_path);}
        //_model->set
    //definier l'address original
    else{
        _model->setRootPath(QCoreApplication::applicationDirPath());}

    _model->setNameFilterDisables(false);
    _model->setNameFilters(_nameFilter);

}

void FolderTree::TreeViewSet(QString folder_path,int index_open)
{

    _TreeView->setAnimated(false);
    _TreeView->setSortingEnabled(true);
    _TreeView->setModel(_model);
    //_TreeView->sortByColumn(0, Qt::DescendingOrder);
    _TreeView->header()->setSortIndicator(0,Qt::AscendingOrder);

    //Hide les autres listes sans la premier
    _TreeView->setColumnHidden(1, true);
    _TreeView->setColumnHidden(2, true);
    _TreeView->setColumnHidden(3, true);

    _index_folder_state = index_open;
    //si fishier nouveau est ouverte, modifier l'address
    if(_index_folder_state == 1){
        _TreeView->setRootIndex(_model->index(folder_path));}

    //definier l'address original
    else{
        _TreeView->setRootIndex(_model->index(QCoreApplication::applicationDirPath()));
        //_TreeView->setRootIndex(_model->index(QDir::currentPath()));
    }

}

void FolderTree::PositionSet()
{
    //connect(_TreeView,SIGNAL(doubleClicked ( const QModelIndex)),this,SLOT(TreeViewDoubleClick()));


    _layout->setRowStretch(0, 1);
    _layout->setRowStretch(1, 0);
    _layout->setRowStretch(2, 0);

    _layout->addWidget(_TreeView,0,0,3,1);
    //_layout->addWidget(hide_button,0,0);

    _TreeView->show();
    //connect(hide_button,SIGNAL(clicked()),this,SLOT(showDetailInfo()));
}

void FolderTree::namefilterset()
{
    _nameFilter << "*.rar"<<"*.zip"<<"*.cbz"<<"*.cbr";
}

