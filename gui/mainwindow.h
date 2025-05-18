#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <memory>
#include "concurrentfilesearcher.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_searchButton_clicked();

private:
    void appendLog(const QString& message);
    void updateResults(const std::vector<std::filesystem::path>& results);

    Ui::MainWindow *ui;
    QStringListModel *resultsModel;
    QStringListModel *logModel;

    std::shared_ptr<ConcurrentFileSearcher> searcher;
};

#endif
