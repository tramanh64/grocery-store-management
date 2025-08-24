#ifndef ADDITEM_H
#define ADDITEM_H

#include <QDialog>
#include "mydb.h"
#include <QSqlTableModel>

namespace Ui {
class AddItem;
}

class AddItem : public QDialog
{
    Q_OBJECT

public:
    explicit AddItem(QWidget *parent = nullptr);
    ~AddItem();
    void Show();
    int getNextAvailableID();

private slots:
    void on_btnAdd_clicked();

private:
    Ui::AddItem *ui;
    void resetElements();
    QSqlTableModel *model;
    void updateUI();

};

#endif // ADDITEM_H
