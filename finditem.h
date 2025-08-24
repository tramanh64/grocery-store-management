#ifndef FINDITEM_H
#define FINDITEM_H

#include <QDialog>
#include "mydb.h"
#include <QSqlTableModel>

namespace Ui {
class FindItem;
}

class FindItem : public QDialog
{
    Q_OBJECT

public:
    explicit FindItem(QWidget *parent = nullptr);
    ~FindItem();
    void Show();

    // Phương thức để lấy ID kế tiếp có thể dùng để tự động tăng ID
    int getNextAvailableID();

private slots:
    void on_btnFind_clicked();

private:
    Ui::FindItem *ui;                   // UI của dialog
    QSqlTableModel *model_finditem;      // Model cho việc tìm kiếm
    void updateUI();

};

#endif // FINDITEM_H
