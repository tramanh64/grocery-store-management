#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ptrAddItem = new AddItem(this);
    ptrRemoveItem = new RemoveItem(this);
    ptrExportItem = new ExportItem(this);
    ptrImportItem = new ImportItem(this);
    ptrItemReport = new ItemsReport(this);
    ptrFindItem = new FindItem(this);

    this->setFixedSize(400,300);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete ptrAddItem;
    delete ptrRemoveItem;
    delete ptrExportItem;
    delete ptrImportItem;
    delete ptrItemReport;
    delete ptrFindItem;
    MyDB::ResetInstance();
}

void MainWindow::on_btnAddItem_clicked()
{
    ptrAddItem->Show();
}

void MainWindow::on_btnRemoveItem_clicked()
{
    ptrRemoveItem->Show();
}

void MainWindow::on_btnExportItem_clicked()
{
    ptrExportItem->Show();
}

void MainWindow::on_btnImportItem_clicked()
{
    ptrImportItem->Show();
}

void MainWindow::on_btnReport_clicked()
{
    ptrItemReport->Show();
}

void MainWindow::on_btnFindItem_clicked()
{
    ptrFindItem->show();
}

