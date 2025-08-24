#ifndef EXPORTITEM_H
#define EXPORTITEM_H

#include <QDialog>
#include "mydb.h"
#include <QSqlTableModel>

namespace Ui {
class ExportItem;
}

class ExportItem : public QDialog
{
    Q_OBJECT

public:
    explicit ExportItem(QWidget *parent = nullptr);
    ~ExportItem();
    void Show();
    int getNextAvailableID();

private slots:
    void on_btnExport_clicked();

private:
    Ui::ExportItem *ui;
    void resetElements();
    QSqlTableModel *model;
    void updateUI();


};
#endif // EXPORTITEM_H
