#include "additem.h"
#include "ui_additem.h"
#include "QDate"
#include "mydb.h"

AddItem::AddItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItem)
{
    ui->setupUi(this);

}
void AddItem::updateUI()
{
    qDebug() << "in updateTable()";

    // Tạo model mới để hiển thị dữ liệu
    QSqlTableModel *model = new QSqlTableModel(this, MyDB::getInstance()->getDBInstance());
    model->setTable("Inventory"); // Chỉ định bảng Inventory
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

AddItem::~AddItem()
{
    delete ui;
}

void AddItem::Show()
{
    updateUI();
    this->show();
}

int AddItem::getNextAvailableID() {
    QSqlQuery query(MyDB::getInstance()->getDBInstance());

    // Tìm ID trống nhỏ nhất, tức là ID không tồn tại trong bảng
    query.exec("SELECT ID FROM Inventory ORDER BY ID ASC");

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

void AddItem::on_btnAdd_clicked() {
    ui->lblInfo->setText("");
    QString sName = ui->txtName->text();
    QString sQuantity = ui->txtQuantity->text();
    QString sDOP = ui->txtDOP->text(); // Ngày nhập hàng
    QString sSellingPrice = ui->txtSellingPrice->text();
    QString sPurchasePrice = ui->txtPurchasePrice->text();

    // Kiểm tra nếu thông tin đầu vào còn thiếu
    if (sName.isEmpty() || sQuantity.isEmpty() || sDOP.isEmpty()) {
        ui->lblInfo->setText("All fields are required.");
        return;
    }
    // kiểm tra định dạng ngày
    QDate date = QDate::fromString(sDOP, "dd/MM/yyyy");
    if (!date.isValid()) {
        ui->lblInfo->setText("Invalid date format. Please use dd/MM/yyyy.");
        return;
    }
    QString formattedDate = sDOP;
//-----------------------------------------------------------------------------------
    // Chuyển đổi sSellingPrice và sPurchasePrice sang float
    bool sellingPriceOk, purchasePriceOk;
    float fSellingPrice = sSellingPrice.toFloat(&sellingPriceOk);
    float fPurchasePrice = sPurchasePrice.toFloat(&purchasePriceOk);

    if (!sellingPriceOk || !purchasePriceOk) {
        ui->lblInfo->setText("Invalid price format. Please ensure the prices are numeric.");
        return;
    }
//-----------------------------------------------------------------------------------
    //kiếm tra quantity có phải int > 0
    bool QuantityOk;
    int intQuantity = sQuantity.toInt(&QuantityOk);
    if (!QuantityOk || intQuantity <= 0) {
        ui->lblInfo->setText("số nguyên dương.");
        return;
    }
//-----------------------------------------------------------------------------------
    QSqlQuery query(MyDB::getInstance()->getDBInstance());

    // Kiểm tra xem sản phẩm với sName có tồn tại không
    query.prepare("SELECT ID, Quantity, Day_of_purchase FROM Inventory WHERE Name = :name");
    query.bindValue(":name", sName);

    if (!query.exec()) {
        qDebug() << "SQL Error:" << query.lastError().text();
        ui->lblInfo->setText("Error checking item existence.");
        return;
    }

    if (query.next()) {
        // Nếu sản phẩm tồn tại, lấy thông tin cần thiết
        int existingID = query.value("ID").toInt();
        QString existingDOP = query.value("Day_of_purchase").toString();

        // So sánh ngày mua
        if (formattedDate >= existingDOP) {
            // Ngày mua mới hợp lệ, cập nhật số lượng
            query.prepare("UPDATE Inventory SET Quantity = Quantity + :quantity, Day_of_purchase = :DOP WHERE ID = :id");
            query.bindValue(":quantity", sQuantity.toInt());
            query.bindValue(":DOP", formattedDate); // Cập nhật lại ngày mua
            query.bindValue(":id", existingID);
        } else {
            // Ngày mua mới nhỏ hơn, thông báo lỗi
            ui->lblInfo->setText("Error: New purchase date is earlier than the current purchase date.");
            return;
        }
    } else {
        // Nếu sản phẩm không tồn tại, thêm sản phẩm mới
        int newID = getNextAvailableID();
        query.prepare("INSERT INTO Inventory (ID, Name, Quantity, Day_of_purchase, SellingPrice, PurchasePrice) "
                      "VALUES (:id, :name, :quantity, :dop, :sellingPrice, :purchasePrice)");
        query.bindValue(":id", newID);
        query.bindValue(":name", sName);
        query.bindValue(":quantity", intQuantity);
        query.bindValue(":dop", formattedDate);
        query.bindValue(":sellingPrice", fSellingPrice);
        query.bindValue(":purchasePrice", fPurchasePrice);
    }
//-----------------------------------------------------------------------------------
    // Thực hiện truy vấn
    if (!query.exec()) {
        qDebug() << "SQL Error:" << query.lastError().text();
        ui->lblInfo->setText("Unable to add or update item.");
    } else {
        ui->lblInfo->setText("Item processed successfully!");
        resetElements();
        // Cập nhật model để hiển thị dữ liệu mới trong QTableView
        updateUI();
    }
}


void AddItem::resetElements()
{
    ui->txtName->clear();
    ui->txtQuantity->clear();
    ui->txtDOP->clear();
    ui->txtSellingPrice->clear();
    ui->txtPurchasePrice->clear();
}
