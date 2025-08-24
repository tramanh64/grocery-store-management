#ifndef IMPORTITEM_H
#define IMPORTITEM_H

#include <QDialog>
#include "mydb.h"
#include <QSqlTableModel>

namespace Ui {
class ImportItem;
}

class ImportItem : public QDialog
{
    Q_OBJECT

public:
    explicit ImportItem(QWidget *parent = nullptr);
    ~ImportItem();
    void Show();
    int getNextAvailableID();

private slots:
    void on_btnImport_clicked();

private:
    Ui::ImportItem *ui;
    void resetElements();
    QSqlTableModel *model;
    void updateUI();
};

#endif // IMPORTITEM_H
