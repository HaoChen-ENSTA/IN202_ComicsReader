#ifndef CBRMAINWINDOW_H
#define CBRMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSystemTrayIcon>//托盘，程序的缩小状态
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QScrollBar>
#include <graphicsview.hpp>
#include <ZipReader.h>
#include <QSlider>
#include <QThread>

#include "CbrFolderTree.h"
#include "CbrFileTree.h"

class CbrThread;
namespace Ui {
class CbrMainWindow;
}

class CbrMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CbrMainWindow(QWidget *parent = 0);
    ~CbrMainWindow();
    QString _strDirOpen;
    QGraphicsScene* _cbrscene;
    ZipReader* _zipReader;
    int picFlag;
    QList<int> currentPic;
    int picNum=0;
    int modeFlag=0;
//    QList<QWidget*> WidgetList;
    int isOpenedBefore;
    qreal scale=1;
    qreal zoomFactor=1;
    void adjust();
    QString getPath();
    void keyPressEvent(QKeyEvent *);
    void wheelEvent(QWheelEvent*);
    void mousePressEvent(QMouseEvent*);
    void twoPicViewInit();
    QSystemTrayIcon* _icon;//logo
    qreal max_width;
    qreal max_height;
    qreal max_size;
    qreal width_tmp;
    qreal height_tmp;
    QSlider* _slider;
    QString _file_suffix;
    CbrThread* _cbrtread;
    int filterFlag=0;
    QPixmap filter(QPixmap img);
    QString _fileName;

    //***************** CHEN Hao_beg *****************//
    FolderTree* mTreeView;
    FileTree* ima_TreeView;
    QString folder_name;
    QString file_name;
    int index_folder_state = 0;// 1 represent ouvert
    int index_file_state = 0;// 1 represent ouvert
    bool hide_index = false;
    void closeEvent(QCloseEvent *event);
//***************** CHEN Hao_end *****************//

public slots:
    void slotActivated(QSystemTrayIcon::ActivationReason);
    void setSliderVal();
    void jump2Pic();
    void displayPic();
private slots:

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_action_Open_triggered();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_action_Exit_triggered();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_action_Save_triggered();

    void TreeViewDoubleClick();

    void TreeViewClick_pic();

    void on_pushButton_10_clicked();

signals:
    void updatePicSignal();
private:
    Ui::CbrMainWindow *ui;

    void folder_open();
    void file_open();
    void folder_delete(QString deletePath);
    QAction *Folder_openAction;
    QAction *File_openAction;
};

class CbrThread:public QThread
{
    Q_OBJECT
public :
    CbrMainWindow* _MainWindow;
    CbrThread(CbrMainWindow *pWindow);
    virtual void run();
    int picFlag=6;
    ~CbrThread();
public slots:
    void updatePicSlot();
};

#endif // CBRMAINWINDOW_H
