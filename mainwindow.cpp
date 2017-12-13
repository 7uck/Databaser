#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qcheckbox.h"
#include "qdebug.h"
#include "qfiledialog.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qsqlerror.h"
#include "qsqltablemodel.h"
#include "qtableview.h"
#include "qtextstream.h"
#include "qxmlstream.h"

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

void MainWindow::on_actionBatch_Editor_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Select files...", "C:\\Users\\tuckerma\\Desktop\\Brett's Latest\\Language Files");
    if(filenames.isEmpty()) return;

    for(const QString &filename : filenames)
    {
        QFile file(filename);
        if(!file.open(QIODevice::ReadWrite))
        {
            qDebug() << "Error opening first file: " << file.errorString();
            return;
        }

        QFile newfile("C:\\Users\\tuckerma\\Desktop\\Brett's Latest\\New Language Files\\" + filename.split("/").last());
        if(!newfile.open(QIODevice::WriteOnly))
        {
            qDebug() << "Error creating second file: " << newfile.errorString();
            return;
        }

        qDebug() << "Modifying " + filename;

        QXmlStreamReader reader(&file);
        QXmlStreamWriter writer(&newfile);

        QRegExp regexp("\\s?SPN:[0-9,\\s]{1,7}\\sFMI:[0-9]{1,3}");

        while(!reader.atEnd())
        {
            reader.readNext();
            if(reader.isStartDocument() || reader.isDTD() || reader.attributes().hasAttribute("numerus"))
            {
                writer.writeCurrentToken(reader);
                continue;
            }

            if(reader.name().toString() == "source" || reader.name().toString() == "translation")
            {
                bool status = reader.attributes().hasAttribute("type");

                QString text = reader.readElementText(QXmlStreamReader::SkipChildElements);
                //qDebug() << "Before: " << text;

                if(text.contains(regexp))
                {
                    text = text.replace(regexp, "");
                    //qDebug() << "After: " << text;
                }

                writer.writeStartElement(reader.name().toString());
                if(status) writer.writeAttribute("type", text.isEmpty() ? "unfinished" : "finished");
                writer.writeCharacters(text);
                writer.writeEndElement();
            }
            else
            {
                //qDebug() << "Writing current token: " << reader.tokenString();
                writer.writeCurrentToken(reader);
            }
        }

        file.close();
        newfile.close();
    }
    qDebug() << "Modifications complete!";

// Future dialog code
//    QDialog dialog(this);

//    QLineEdit *findLine = new QLineEdit;
//    // Set the regexp to find SPN/FMI strings
//    findLine->setText("\\s?SPN:[0-9,\\s]{1,7}\\sFMI:[0-9]{1,3}");
//    QLineEdit *replaceLine = new QLineEdit;
//    QCheckBox *spreadsheetCheck = new QCheckBox("Generate spreadsheet?");
//    QPushButton *run = new QPushButton("Run");
//    connect(run, &QPushButton::clicked, [=]()
//    {
//       on_batchEditor_Run(findLine->text(), replaceLine->text(), spreadsheetCheck->isChecked());
//    });

//    QVBoxLayout *vlayout = new QVBoxLayout(&dialog);
//    vlayout->addWidget(new QLabel("Find"));
//    vlayout->addWidget(findLine);
//    vlayout->addWidget(new QLabel("Replace"));
//    vlayout->addWidget(replaceLine);
//    vlayout->addWidget(spreadsheetCheck);
//    vlayout->addWidget(run);

//    dialog.exec();
}

void MainWindow::on_actionCSV_Generator_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Select files...", "C:\\Users\\tuckerma\\Desktop\\New Language Files");
    if(filenames.isEmpty()) return;

    for(const QString &filename : filenames)
    {
        QFile xml(filename);
        if(!xml.open(QIODevice::ReadWrite))
        {
            qDebug() << "Error opening first file: " << xml.errorString();
            return;
        }
        QXmlStreamReader reader(&xml);

        QFile csv("C:\\Users\\tuckerma\\Desktop\\New Language Files\\" + filename.split("/").last() + ".csv");
        if(!csv.open(QIODevice::WriteOnly))
        {
            qDebug() << "Error in CSV file: " << csv.errorString();
            return;
        }
        QTextStream out(&csv);
        out << "Source, Translation\n";

        qDebug() << "Generating comparison file for ";

        while(!reader.atEnd())
        {
            reader.readNext();
            if(reader.isStartDocument() || reader.isDTD())
                continue;

            if(reader.name().toString() == "source")
            {
                QString text = reader.readElementText();
                out << (text.replace("\n", " ") + ",");
            }
            else if(reader.name().toString() == "translation")
            {
                QString text = reader.readElementText();
                out << (text.replace("\n", " ") + "\n");
            }
        }

        xml.close();
        csv.close();
    }
    qDebug() << "Spreadsheet complete!";
}

void MainWindow::on_actionCapitalizer_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Select files...", "C:\\Users\\tuckerma\\Desktop\\Praveen's Latest\\Language Files");
    if(filenames.isEmpty()) return;

    for(const QString &filename : filenames)
    {
        QFile file(filename);
        if(!file.open(QIODevice::ReadWrite))
        {
            qDebug() << "Error opening first file: " << file.errorString();
            return;
        }

        QFile newfile("C:\\Users\\tuckerma\\Desktop\\Praveen's Latest\\Capitalized\\" + filename.split("/").last());
        if(!newfile.open(QIODevice::WriteOnly))
        {
            qDebug() << "Error creating second file: " << newfile.errorString();
            return;
        }

        qDebug() << "Modifying " + filename;

        QXmlStreamReader reader(&file);
        QXmlStreamWriter writer(&newfile);
        writer.setAutoFormatting(true);

        //QRegExp regexp("\\s?SPN:[0-9,\\s]{1,7}\\sFMI:[0-9]{1,3}");

        QString sourceText;

        while(!reader.atEnd())
        {
            reader.readNext();
            if(reader.isStartDocument() || reader.isDTD() || reader.attributes().hasAttribute("numerus"))
            {
                writer.writeCurrentToken(reader);
                continue;
            }

            if(reader.name().toString() == "source")
            {
                QString text = reader.readElementText(QXmlStreamReader::SkipChildElements);
                sourceText = text;
                //qDebug() << "Before: " << text;

                writer.writeStartElement(reader.name().toString());
                writer.writeCharacters(text);
                writer.writeEndElement();
            }
            else if(reader.name().toString() == "translation")
            {
                QString type;
                if(reader.attributes().hasAttribute("type"))
                    type = reader.attributes().value("type").toString();

                if(type.isEmpty())
                {
                    QString text = reader.readElementText(QXmlStreamReader::SkipChildElements);
                    writer.writeTextElement(reader.name().toString(), text);
                }
                else
                {
                    qDebug() << "Source text before: " << sourceText;
                    QStringList parts = sourceText.split(' ', QString::SkipEmptyParts);
                    for(int i = 0; i < parts.size(); ++i)
                    {
                        QString part = parts.at(i);
                        part[0] = part[0].toUpper();
                        parts.replace(i, part);
                    }
                    sourceText = parts.join(" ");
                    qDebug() << "Source text after: " << sourceText;

                    // Call this to keep the reader in sync
                    QString text = reader.readElementText(QXmlStreamReader::SkipChildElements);

                    writer.writeTextElement(reader.name().toString(), sourceText);
                }

                sourceText.clear();
            }
            else
            {
                //qDebug() << "Writing current token: " << reader.tokenString();
                writer.writeCurrentToken(reader);
            }
        }

        file.close();
        newfile.close();
    }
    qDebug() << "Modifications complete!";
}
