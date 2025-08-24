#include "removeitem.h"
#include "ui_removeitem.h"

RemoveItem::RemoveItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveItem)
{
    ui->setupUi(this);

}
void RemoveItem::updateUI()
{
    qDebug() << "in updateUI()";

    // Khởi tạo model và liên kết với QTableView
    QSqlTableModel *model = new QSqlTableModel(this, MyDB::getInstance()->getDBInstance());
    model->setTable("Inventory");  // Chỉ định bảng Inventory
    model->select();  // Tải dữ liệu từ bảng Inventory

    // Thiết lập tiêu đề cột (tùy chỉnh nếu cần)
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(3, Qt::Horizontal, tr("Day of Purchase"));
    model->setHeaderData(4, Qt::Horizontal, tr("Selling Price"));
    model->setHeaderData(5, Qt::Horizontal, tr("Purchase Price"));

    // Liên kết model với QTableView
    ui->tableView->setModel(model);

    // Tuỳ chọn: thiết lập chế độ chỉnh sửa nếu cần
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho phép chỉnh sửa trực tiếp
}

RemoveItem::~RemoveItem()
{
    delete ui;
}

void RemoveItem::Show()
{
    updateUI();
    this->show();
}

void RemoveItem::on_btnRemove_clicked() {
    ui->lblInfo->setText("");

    // Lấy tiêu chí từ QComboBox và giá trị từ QLineEdit
    QString criteria = ui->cmbCriteria->currentText(); // "Name" hoặc "ID"
    QString value = ui->txtCriteria->text();

    // Kiểm tra xem giá trị có được nhập hay không
    if (value.isEmpty()) {
        ui->lblInfo->setText("Please enter the item criteria for removal.");
        return;
    }

    QSqlQuery query(MyDB::getInstance()->getDBInstance());
    query.clear();

    // Chuẩn bị câu lệnh SQL để tìm ID của item
    int itemID = -1;
    if (criteria == "Name") {
        query.prepare("SELECT ID FROM Inventory WHERE Name = :value");
    } else if (criteria == "Id") {
        query.prepare("SELECT ID FROM Inventory WHERE Id = :value");
    }
    query.bindValue(":value", value);

    // Thực hiện câu lệnh SELECT để lấy ID
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery();
        ui->lblInfo->setText("Unable to fetch item ID.");
        return;
    }

    if (query.next()) {
        itemID = query.value(0).toInt();
    } else {
        ui->lblInfo->setText("Item not found.");
        return;
    }

    // Tiến hành xóa item từ bảng Inventory
    query.clear();
    if (criteria == "Name") {
        query.prepare("DELETE FROM Inventory WHERE Name = :value");
    } else if (criteria == "Id") {
        query.prepare("DELETE FROM Inventory WHERE Id = :value");
    }
    query.bindValue(":value", value);

    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery();
        ui->lblInfo->setText("Unable to Remove Item");
        return;
    }

    // Nếu xóa thành công, cập nhật lại ID của các mục còn lại
    query.prepare("UPDATE Inventory SET ID = ID - 1 WHERE ID > :itemID");
    query.bindValue(":itemID", itemID);
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery();
        ui->lblInfo->setText("Unable to update remaining Item IDs.");
        return;
    }

    // Cập nhật thành công
    qDebug() << "Item removed and IDs updated successfully.";
    ui->lblInfo->setText("Item Removed and IDs Updated Successfully!");
    resetElements();
    // Cập nhật model để hiển thị dữ liệu mới trong QTableView
    updateUI();
}

void RemoveItem::resetElements()
{
    ui->txtCriteria->clear();
}
