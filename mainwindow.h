#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionOpen_Recent_triggered();

    void on_actionBatch_Editor_triggered();

    //void on_batchEditor_Run(const QString &find, const QString &replace, bool generateCsv);

    void on_actionCSV_Generator_triggered();

    void on_actionCapitalizer_triggered();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
