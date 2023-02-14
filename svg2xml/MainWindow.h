#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;
	QList<QTableWidgetItem*> MainWindow::getCheckedFile();
	QList<QList<QString>>  MainWindow::getPathFromFile(QString filePath);
	void MainWindow::detect_letter(QList<QList<QString> > List_paths);

private slots:
	void selectSvgClicked();
	void selectRepClicked();
	void toutCocherCheck();
	void convertirClicked();
};


