#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filematcher.h"
#include "concurrentfilesearcher.h"
#include "logger.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      resultsModel(new QStringListModel(this)),
      logModel(new QStringListModel(this))
{
    ui->setupUi(this);
    ui->resultListView->setModel(resultsModel);
    ui->logListView->setModel(logModel);
    Logger::setModel(logModel);

    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_searchButton_clicked()
{
    QString pattern = ui->patternLineEdit->text().trimmed();
        QString rootDir = ui->directoryLineEdit->text().trimmed();

        if (pattern.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter a search pattern.");
            Logger::log("The user did not enter a search pattern.", Logger::Level::Warn);
            return;
        }

        if (rootDir.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter a directory path.");
            Logger::log("The user did not enter a search path.", Logger::Level::Warn);
            return;
        }

        bool caseInsensitive = ui->caseInsensitiveRadio->isChecked();

        ConcurrentFileSearcher searcher({pattern.toStdString()}, caseInsensitive);

        resultsModel->setStringList({});
        Logger::log("Start searching...", Logger::Level::Info);

        try {
            auto results = searcher.search(rootDir.toStdString());

            QStringList resultList;
            for (const auto& path : results) {
                resultList << QString::fromStdString(path.string());
            }

            resultsModel->setStringList(resultList);

            Logger::log(QString("Search completed. Number of files found: %1")
                            .arg(resultList.size())
                            .toStdString(),
                        Logger::Level::Info);
        } catch (const std::exception& ex) {
            Logger::log(std::string("Error: ") + ex.what(), Logger::Level::Error);
            QMessageBox::critical(this, "Error", ex.what());
        }
}
