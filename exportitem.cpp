#include "exportitem.h"
#include "ui_exportitem.h"
#include <QDateTime>  // Để lấy thời gian hiện tại

ExportItem::ExportItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportItem)
{
    ui->setupUi(this);

}

ExportItem::~ExportItem()
{
    delete ui;
}
int ExportItem::getNextAvailableID() {
    QSqlQuery query(MyDB::getInstance()->getDBInstance());

    // Tìm ID trống nhỏ nhất, tức là ID không tồn tại trong bảng
    query.exec("SELECT LogID FROM log_export ORDER BY LogID ASC");

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

void ExportItem::updateUI()
{
    qDebug() << "in updateTable()";

    // Tạo model mới để hiển thị dữ liệu
    QSqlTableModel *model = new QSqlTableModel(this, MyDB::getInstance()->getDBInstance());
    model->setTable("log_export"); // Chỉ định bảng Inventory
    model->select();  // Tải lại dữ liệu từ cơ sở dữ liệu

    // Thiết lập các tiêu đề cột nếu cần
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(3, Qt::Horizontal, tr("Day of Purchase"));
    model->setHeaderData(4, Qt::Horizontal, tr("Selling Price"));
    model->setHeaderData(5, Qt::Horizontal, tr("Purchase Price"));

    // Liên kết model với QTableView
    ui->tableView->setModel(model);

    // Nếu cần, có thể chỉnh sửa chế độ bảng
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Không cho phép chỉnh sửa
}
void ExportItem::Show()
{
    updateUI();
    this->show();
}

void ExportItem::on_btnExport_clicked()
{
    ui->lblInfo->setText("");  // Reset thông báo

    // Lấy ID sản phẩm và số lượng xuất từ UI
    QString sID = ui->txtID->text();
    QString sExportCount = ui->txtExportCount->text();  // Số lượng muốn xuất khẩu

    // Kiểm tra nếu số lượng xuất hợp lệ
    bool okExportCount;
    int exportCountInt = sExportCount.toInt(&okExportCount);
    if (!okExportCount || exportCountInt <= 0) {
        ui->lblInfo->setText("Số lượng xuất không hợp lệ.");
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

        // Kiểm tra nếu số lượng xuất nhiều hơn số lượng hiện có
        if (exportCountInt > quantityInt) {
            ui->lblInfo->setText("Số lượng xuất nhiều hơn trong kho.");
            return;
        }

        // Tính toán số lượng còn lại trong kho sau khi xuất
        int newQuantity = quantityInt - exportCountInt;

        // Cập nhật số lượng mới vào bảng Inventory
        query.prepare("UPDATE Inventory SET Quantity = :newQuantity WHERE Id = :Id");
        query.bindValue(":newQuantity", newQuantity);
        query.bindValue(":Id", itemId);

        if (!query.exec()) {
            qDebug() << query.lastError().text() << query.lastQuery();
            ui->lblInfo->setText("Không thể xuất hàng.");
        } else {
            if (query.numRowsAffected() > 0) {
                ui->lblInfo->setText("Xuất thành công");

                // Lấy ngày xuất từ QDateEdit
                QDate exportDate = ui->dateExport->date();  // Lấy ngày từ QDateEdit

                // Kiểm tra nếu ngày xuất hợp lệ
                if (!exportDate.isValid()) {
                    ui->lblInfo->setText("Vui lòng chọn ngày xuất hợp lệ.");
                    return;
                }

                // Chuyển đổi ngày xuất sang định dạng chuỗi "yyyy-MM-dd"
                QString sExportDate = exportDate.toString("yyyy-MM-dd");
                int newID = getNextAvailableID();

                // Cập nhật thông tin vào bảng log_export với Item_ID là khóa ngoại
                query.prepare("INSERT INTO log_export (LogID, Date, Item_ID, Name, Quantity_Export, Quantity_Left) "
                              "VALUES (:LogID, :Date, :Item_ID, :Name, :Quantity_Export, :Quantity_Left)");
                query.bindValue(":LogID", newID);  // Gán ngày xuất

                query.bindValue(":Date", sExportDate);  // Gán ngày xuất
                query.bindValue(":Item_ID", itemId);
                query.bindValue(":Name", itemName);  // Ghi tên sản phẩm vào log_export
                query.bindValue(":Quantity_Export", exportCountInt);
                query.bindValue(":Quantity_Left", newQuantity);
                if (!query.exec()) {
                    qDebug() << query.lastError().text() << query.lastQuery();
                    ui->lblInfo->setText("Không thể ghi vào log_export.");
                } else {
                    qDebug() << "Log Exported Successfully!";
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
void ExportItem::resetElements()
{
    ui->txtID->clear();  // Xóa trường ID
    ui->txtExportCount->clear();
    ui->dateExport->clear();
}
