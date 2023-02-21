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
	QList<QString> MainWindow::getPathsFromFile(QString filePath);
	void MainWindow::pathToPolyLine(QList<QString> paths, QList<QString>* polyLines);

private slots:
	void selectSvgClicked();
	void selectRepClicked();
	void toutCocherCheck();
	void convertirClicked();
};


