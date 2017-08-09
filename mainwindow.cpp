#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qdebug.h"
#include "qfiledialog.h"
#include "qsqlerror.h"
#include "qsqltablemodel.h"
#include "qtableview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->splitter->setSizes({ 1, 2 });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose a Database", "C:\Users", "*.sql, *.db");
    if(!filename.isEmpty())
    {
        db = QSqlDatabase::database();
        if(!db.isValid())
        {
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(filename);
            if(!db.open())
            {
                qDebug() << db.lastError().text();
                return;
            }
        }
        QTreeWidgetItem *topItem = new QTreeWidgetItem({ filename.split("/").last() });
        ui->treeWidget->addTopLevelItem(topItem);

        for(const QString &table : db.tables())
            topItem->addChild(new QTreeWidgetItem({ table }));

        topItem->setExpanded(true);
    }
}

void MainWindow::on_actionOpen_Recent_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose a Database", "C:/Users/mtucker/Documents/Projects/Test Data", "*.sql, *.db");
    if(!filename.isEmpty())
    {
        db = QSqlDatabase::database();
        if(!db.isValid())
        {
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(filename);
            if(!db.open())
            {
                qDebug() << db.lastError().text();
                return;
            }
        }
        QTreeWidgetItem *topItem = new QTreeWidgetItem({ filename.split("/").last() });
        ui->treeWidget->addTopLevelItem(topItem);

        for(const QString &table : db.tables())
            topItem->addChild(new QTreeWidgetItem({ table }));

        topItem->setExpanded(true);
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString tableName = item->text(column);

    const int count = ui->tabWidget->count();
    for(int i = 0; i < count; ++i)
    {
        if(ui->tabWidget->tabText(i) == tableName)
        {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }

    QTableView *tableView = new QTableView;
    QSqlTableModel *tableModel = new QSqlTableModel(this, db);
    tableModel->setTable(tableName);
    tableModel->select();
    tableView->setModel(tableModel);
    int i = ui->tabWidget->addTab(tableView, tableName);
    ui->tabWidget->setCurrentIndex(i);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QWidget *tab = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete tab;
}


