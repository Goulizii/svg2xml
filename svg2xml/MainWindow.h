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
	QList<QList<QString>> MainWindow::getPathsFromFile(QString filePath);
	void MainWindow::pathToPolyLine(const QList<QList<QString>> paths, QList<QPoint>* polyLines);
	void MainWindow::delDuplicate(QList<QPoint>* polyLines);
	QList<QPoint> MainWindow::triPolyLines(QList<QPoint>* polyLines);

private slots:
	void selectSvgClicked();
	void selectRepClicked();
	void toutCocherCheck();
	void convertirClicked();
};


