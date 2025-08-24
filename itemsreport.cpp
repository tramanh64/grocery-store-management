#include "itemsreport.h"
#include "ui_itemsreport.h"
#include <utility> // For std::move

ItemsReport::ItemsReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemsReport)
{
    ui->setupUi(this);

    // Kết nối nút "Show Table" với hàm updateUI
    connect(ui->btnShowTable, &QPushButton::clicked, this, &ItemsReport::updateUI);

}

ItemsReport::~ItemsReport()
{
    delete ui;
}

void ItemsReport::Show()
{
    updateUI();
    this->show();
}

void ItemsReport::updateUI()
{
    qDebug() << "in updateUI()";

    // Kiểm tra nếu người dùng chưa chọn bảng nào
    QString selectedTable = ui->cmbTable->currentText();
    if (selectedTable.isEmpty()) {
        ui->lblInfo->setText("Please select a table.");
        return;
    }

    // Tạo một model mới để hiển thị dữ liệu
    QSqlQueryModel *model = new QSqlQueryModel(this);

    // Lấy kết nối cơ sở dữ liệu
    QSqlQuery query(MyDB::getInstance()->getDBInstance());

    // Xác định cột sắp xếp dựa trên bảng đã chọn
    QString queryString;
    if (selectedTable == "Inventory") {
        queryString = QString("SELECT * FROM %1 ORDER BY Id DESC").arg(selectedTable);
    } else if (selectedTable == "log_import" || selectedTable == "log_export") {
        queryString = QString("SELECT * FROM %1 ORDER BY LogID DESC").arg(selectedTable);
    } else {
        ui->lblInfo->setText("Unknown table selected.");
        return;
    }

    query.prepare(queryString);

    // Thực hiện câu lệnh SQL
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery();
        ui->lblInfo->setText("Error executing query.");
    } else {
        qDebug() << "Query executed successfully!";
        // Cập nhật dữ liệu từ query vào model
        model->setQuery(std::move(query));

        // Hiển thị dữ liệu lên UI
        ui->tableView->setModel(model);

        // Hiển thị số dòng trong bảng
        qDebug() << "Rows are: " << model->rowCount();
        ui->tableView->show();
    }
}
