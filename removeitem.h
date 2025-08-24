#ifndef REMOVEITEM_H
#define REMOVEITEM_H

#include <QDialog>
#include "mydb.h"
#include <QSqlTableModel>

namespace Ui {
class RemoveItem;
}

class RemoveItem : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveItem(QWidget *parent = nullptr);
    ~RemoveItem();
    void Show();
    int getNextAvailableID();

private slots:
    void on_btnRemove_clicked();

private:
    Ui::RemoveItem *ui;
    void resetElements();
    QSqlTableModel *model;
    void updateUI();


};

#endif // REMOVEITEM_H
