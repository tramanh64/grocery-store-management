/*
 * Project Developed by CppBuzz.com for learning QT-C++ concepts
 * Users are allowed to use/modify this code according to their requirement.
 * These projects are free to download.
 * Date: 25th April 2020
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "additem.h"
#include "removeitem.h"
#include "exportitem.h"
#include "importitem.h"
#include "itemsreport.h"
#include "finditem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAddItem_clicked();

    void on_btnRemoveItem_clicked();

    void on_btnExportItem_clicked();

    void on_btnImportItem_clicked();

    void on_btnReport_clicked();

    void on_btnFindItem_clicked();

private:
    Ui::MainWindow *ui;
    AddItem * ptrAddItem;
    RemoveItem * ptrRemoveItem;
    ExportItem * ptrExportItem;
    ImportItem * ptrImportItem;
    ItemsReport *ptrItemReport;
    FindItem * ptrFindItem;

};

#endif // MAINWINDOW_H
