#include "CbrFileTree.h"

#include <QDebug>

FileTree::FileTree(FolderTree* mTreeView):
    _mTreeView(mTreeView)
{

}

FileTree::FileTree(ZipReader* zipReader,FolderTree* mTreeView, QString file_path , int index_filer_state):
    _zipReader(zipReader),_file_path(file_path),_index_file_state(index_filer_state),_mTreeView(mTreeView)
{
    _mTreeView->setLayout(_mTreeView->_layout);
    namefilterset();
    FileOriginal();

    TreeViewSet();
    PositionSet();

}

/*FileTree::FileTree(FolderTree* mTreeView, QString file_path , int index_filer_state):
    _file_path(file_path),_index_file_state(index_filer_state),_mTreeView(mTreeView)
{
    _mTreeView->setLayout(_mTreeView->_layout);
    namefilterset();
    FileOriginal_rar(_folder_path,_index_folder_state);

    TreeViewSet_rar(_file_path,_index_file_state);

    PositionSet();

}*/

void FileTree::TreeViewSet()
{
    _TreeView->setAnimated(false);
    _TreeView->setSortingEnabled(true);

    //_TreeView->header()->setSortIndicator(0,Qt::AscendingOrder);
    _TreeView->setModel(_model);
    _TreeView->setColumnHidden(1, true);
    _TreeView->setColumnHidden(2, true);
    _TreeView->setColumnHidden(3, true);

}
/*void FileTree::FileOriginal_rar(QString folder_path,int index_open) //
{
    //si fishier nouveau est ouverte, modifier l'address
    if(_index_folder_state == 1){
        rar_model->setRootPath(folder_path);}

    //definier l'address original
    else{
        rar_model->setRootPath("d:/");}

    rar_model->setNameFilterDisables(false);
    rar_model->setNameFilters(_nameFilter);

}*/

void FileTree::FileOriginal() //
{
    while(_zipReader->isReadFinished != 1){qDebug()<<"loading";}
    int num = _zipReader->_picNum;
    _model = new QStandardItemModel();
    _model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("PAGE")<<QStringLiteral("Name"));
    //si fishier nouveau est ouverte, modifier l'address
    for(int i = 0 ; i < num;i++){
        QString page;
        if(i+1 < 10){
            page = QString("00%1").arg(i+1);}
        else if(i+1 < 100){
            page = QString("0%1").arg(i+1);}
        else {
            page = QString("%1").arg(i+1);}

        _model->appendRow( new QStandardItem(page));

//        if(!_zipReader->_currentPicList.indexOf(page)){_model->appendRow( new QStandardItem(page));}
//        else {_model->appendRow( new QStandardItem(QIcon(_zipReader->_pixmapList.at(i)), page));}
//       _model->setItem(i,1,new QStandardItem(_zipReader->_currentPicList.at(i)));
    }
    //treeviewshow();

}

void FileTree::PositionSet()
{

    _mTreeView->_layout->setRowStretch(0, 1);
    _mTreeView->_layout->setRowStretch(1, 3);
    _mTreeView->_layout->setRowStretch(2, 6);

    _mTreeView->_layout->addWidget(this->_TreeView,2,0);
    _mTreeView->_layout->addWidget(_mTreeView->_TreeView,1,0);
    _mTreeView->_layout->addWidget(hide_button,0,0);
    _TreeView->show();
    connect(hide_button,SIGNAL(clicked()),this,SLOT(showDetailInfo()));
   // connect(_mTreeView,SIGNAL(clicked()),this,SLOT(TreeViewClick_pic()));

}

//function de montre ou cacher le treeview
void FileTree::showDetailInfo()
{
    hide_button->setMinimumWidth(2);
        if(_mTreeView->_TreeView->isHidden()){
            _mTreeView->_layout->setRowStretch(0, 1);
            _mTreeView->_layout->setRowStretch(1, 3);
            _mTreeView->_layout->setRowStretch(2, 6);

            _mTreeView->_layout->addWidget(this->_TreeView,2,0,1,1);
            //_layout->addWidget(hide_button,0,0);
            _TreeView->show();
            _mTreeView->_TreeView->show();

            hide_button->setText("▲");
        }

        else{
            _mTreeView->_layout->addWidget(hide_button,0,0,1,1);
            _mTreeView->_layout->addWidget(this->_TreeView,1,0,2,1);
            this->_TreeView->show();
            _mTreeView->_TreeView->hide();
            hide_button->setText("▼");
        }
    //_TreeView->clicked();
}

void FileTree::namefilterset()
{
    _nameFilter.clear();
    _nameFilter <<"*.jpg"<<"*.png"<<"*.webp";
}

//void FileTree::treeviewshow()
//{

//    for(int i =0; i<6;i++)
//    {
//        QString page_str;
//        int page = _zipReader->_currentPicList.at(i) + 1;

//        if(page < 10)
//        {
//            page_str = QString("00%1").arg(page);
//        }
//        else if(page < 100)
//        {
//            page_str = QString("0%1").arg(page);
//        }
//        else
//        {
//            page_str = QString("%1").arg(page);
//        }

//        //ima = _zipReader->_pixmapList.at(i);
////        if(!_zipReader->_currentPicList.indexOf(page)){_model->appendRow( new QStandardItem(page));}
////        else {_model->appendRow( new QStandardItem(QIcon(_zipReader->_pixmapList.at(i)), page));}
//        QIcon *icon = new QIcon(_zipReader->_pixmapList.at(i)->scaled(20,20));
//        _model->setItem(page-1,0,new QStandardItem(*icon, page_str));

//    }

//}
