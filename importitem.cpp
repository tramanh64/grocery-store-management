#include "importitem.h"
#include "ui_importitem.h"
#include <QDateTime>  // Để lấy thời gian hiện tại

ImportItem::ImportItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportItem)
{
    ui->setupUi(this);   
}

ImportItem::~ImportItem()
{
    delete ui;
}

int ImportItem::getNextAvailableID() {
    QSqlQuery query(MyDB::getInstance()->getDBInstance());

    // Tìm ID trống nhỏ nhất, tức là ID không tồn tại trong bảng
    query.exec("SELECT LogID FROM log_import ORDER BY LogID ASC");

    int nextID = 1;  // Nếu bảng trống, bắt đầu từ ID 1
    while (query.next()) {
        int id = query.value(0).toInt();
        if (id == nextID) {
            // ID này đã có, kiểm tra tiếp
            nextID++;
        } else {
            // Nếu tìm thấy ID trống (nextID không có trong bảng), trả về nextID
            break;
        }
    }

    return nextID;
}

void ImportItem::updateUI()
{
    // Khởi tạo model và liên kết với QTableView
    model = new QSqlTableModel(this, MyDB::getInstance()->getDBInstance());
    model->setTable("log_import");  // Chỉ định bảng Inventory
    model->select();  // Tải dữ liệu từ bảng Inventory

    // Thiết lập tiêu đề cột (tùy chỉnh nếu cần)
    model->setHeaderData(0, Qt::Horizontal, tr("LogID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Date_import"));
    model->setHeaderData(2, Qt::Horizontal, tr("Item_ID"));
    model->setHeaderData(3, Qt::Horizontal, tr("Name"));
    model->setHeaderData(4, Qt::Horizontal, tr("Quantity_Export"));
    model->setHeaderData(5, Qt::Horizontal, tr("Quantity_New"));

    // Liên kết model với QTableView
    ui->tableView->setModel(model);

    // Tuỳ chọn: thiết lập chế độ chỉnh sửa nếu cần
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho phép chỉnh sửa trực tiếp
}
void ImportItem::Show()
{
    updateUI();
    this->show();
}

void ImportItem::on_btnImport_clicked()
{
    ui->lblInfo->setText("");  // Reset thông báo

    // Lấy ID sản phẩm và số lượng nhập từ UI
    QString sID = ui->txtID->text();
    QString sImportCount = ui->txtImportCount->text();  // Số lượng muốn nhập

    // Kiểm tra nếu số lượng nhập hợp lệ
    bool okImportCount;
    int importCountInt = sImportCount.toInt(&okImportCount);
    if (!okImportCount || importCountInt <= 0) {
        ui->lblInfo->setText("Số lượng nhập không hợp lệ.");
        return;
    }

    // Truy vấn thông tin sản phẩm từ bảng Inventory theo ID
    QSqlQuery query(MyDB::getInstance()->getDBInstance());
    query.prepare("SELECT Id, Name, Quantity FROM Inventory WHERE Id = :Id");
    query.bindValue(":Id", sID.toInt());  // Sử dụng ID làm khóa

    int itemId = -1;  // ID của sản phẩm trong Inventory
    int quantityInt = 0;  // Số lượng hiện tại trong kho
    QString itemName;  // Tên sản phẩm

    if (query.exec() && query.next()) {
        itemId = query.value("Id").toInt();       // Lấy Id
        itemName = query.value("Name").toString();  // Lấy Name
        quantityInt = query.value("Quantity").toInt();  // Lấy Quantity hiện có

        // Tính toán số lượng còn lại trong kho sau khi nhập
        int newQuantity = quantityInt + importCountInt;

        // Cập nhật số lượng mới vào bảng Inventory
        query.prepare("UPDATE Inventory SET Quantity = :newQuantity WHERE Id = :Id");
        query.bindValue(":newQuantity", newQuantity);
        query.bindValue(":Id", itemId);

        if (!query.exec()) {
            qDebug() << query.lastError().text() << query.lastQuery();
            ui->lblInfo->setText("Không thể nhập hàng.");
        } else {
            if (query.numRowsAffected() > 0) {
                ui->lblInfo->setText("Nhập thành công");

                // Lấy ngày nhập từ QDateEdit
                QDate importDate = ui->dateImport->date();  // Lấy ngày từ QDateEdit

                // Kiểm tra nếu ngày nhập hợp lệ
                if (!importDate.isValid()) {
                    ui->lblInfo->setText("Vui lòng chọn ngày nhập hợp lệ.");
                    return;
                }

                // Chuyển đổi ngày nhập sang định dạng chuỗi "yyyy-MM-dd"
                QString sImportDate = importDate.toString("yyyy-MM-dd");
                int newID = getNextAvailableID();

                // Cập nhật thông tin vào bảng log_import với Item_ID là khóa ngoại
                query.prepare("INSERT INTO log_import (LogID, Date, Item_ID, Name, Quantity_Import, Quantity_New) "
                              "VALUES (:LogID, :Date, :Item_ID, :Name, :Quantity_Import, :Quantity_New)");
                query.bindValue(":LogID", newID);

                query.bindValue(":Date", sImportDate);  // Gán ngày nhập
                query.bindValue(":Item_ID", itemId);
                query.bindValue(":Name", itemName);  // Ghi tên sản phẩm vào log_import
                query.bindValue(":Quantity_Import", importCountInt);
                query.bindValue(":Quantity_New", newQuantity);
                if (!query.exec()) {
                    qDebug() << query.lastError().text() << query.lastQuery();
                    ui->lblInfo->setText("Không thể ghi vào log_import.");
                } else {
                    qDebug() << "Log Imported Successfully!";
                    resetElements();  // Reset các trường nhập liệu
                    // Cập nhật model để hiển thị dữ liệu mới trong QTableView
                    updateUI();
                }
            } else {
                ui->lblInfo->setText("Không thể cập nhật số lượng.");
            }
        }
    } else {
        ui->lblInfo->setText("Sản phẩm không tồn tại trong kho.");
    }
}

// Hàm reset các trường nhập liệu
void ImportItem::resetElements()
{
    ui->txtID->clear();  // Xóa trường ID
    ui->txtImportCount->clear();
    ui->dateImport->clear();
}
