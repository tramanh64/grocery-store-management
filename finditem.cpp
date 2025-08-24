#include "finditem.h"
#include "ui_finditem.h"
#include <utility> // For std::move

FindItem::FindItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindItem)
{
    ui->setupUi(this);
    ui->rdoName->setChecked(true);
    model_finditem = new QSqlTableModel(this, MyDB::getInstance()->getDBInstance());
    model_finditem->setTable("Inventory");  // Chỉ định bảng Inventory
    model_finditem->select();  // Tải dữ liệu từ bảng Inventory
    ui->tableView_2->setModel(model_finditem);  // Liên kết model với tableView_2
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho phép chỉnh sửa trực tiếp
}
void FindItem::updateUI()
{    // Khởi tạo model và liên kết với QTableView
    model_finditem->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model_finditem->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model_finditem->setHeaderData(2, Qt::Horizontal, tr("Quantity"));
    model_finditem->setHeaderData(3, Qt::Horizontal, tr("Day of Purchase"));
    model_finditem->setHeaderData(4, Qt::Horizontal, tr("Selling Price"));
    model_finditem->setHeaderData(5, Qt::Horizontal, tr("Purchase Price"));

    // Cập nhật dữ liệu từ bảng Inventory
    model_finditem->select();
}
FindItem::~FindItem()
{
    delete ui;
}
void FindItem::Show()
{
    updateUI();
    this->show();
}

void FindItem::on_btnFind_clicked()
{
    QString sValue = ui->txtValue->text();
    QString sCriteriaColum = "";

    if(ui->rdoId->isChecked())
        sCriteriaColum="Id";
    else if (ui->rdoName->isChecked())
        sCriteriaColum="Name";
    else if (ui->rdoQuantity->isChecked())
        sCriteriaColum="Quantity";
    else if (ui->rdoSellingPrice->isChecked())
        sCriteriaColum = "SellingPrice";
    else
        sCriteriaColum = "Name";

    qDebug() << "in init()";
    QSqlQueryModel * model = new QSqlQueryModel(this);

    QSqlQuery query( MyDB::getInstance()->getDBInstance());
    query.prepare("select * from Inventory where " +sCriteriaColum+" like '%" + sValue + "%'");

    if(!query.exec())
       qDebug() << query.lastError().text() << query.lastQuery();
    else
    {
       if(query.numRowsAffected()>0)
       qDebug() << "== success query fetch() : " << query.lastQuery();
       else {
           qDebug() << " No record ! found ";
           qDebug() << " Query is : " << query.lastQuery();
       }
    }
    while(query.next())
    qDebug()<<query.value(0).toString();

    model->setQuery(std::move(query));
    ui->tableView->setModel(model);
    qDebug() << "rows are : " << model->rowCount();
    ui->tableView->show();
    // Cập nhật model để hiển thị dữ liệu mới trong QTableView
    updateUI();

}
