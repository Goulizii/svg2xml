#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


	typedef struct infoConv_struct {
		QString article;
		QString machine;
		QString programme;
		QString materiel;
	}infoConv;

public:
	explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;
	QList<QTableWidgetItem*> MainWindow::getCheckedFile();
	QList<QList<QString>> MainWindow::getPathsFromFile(QString fileName);
	void MainWindow::pathToPolyLine(const QList<QList<QString>> paths, QList<int>* polyLines);

	void MainWindow::delDuplicate(QList<QPoint>* polyLines);
	QList<QPoint> MainWindow::triPolyLines(QList<QPoint>* polyLines);

	void MainWindow::writeAndSaveXML(QList<int> pixelsList, const QString fileName, const QString saveDir);
	void MainWindow::recupSAP(QString cleSVG, infoConv *info);
	void MainWindow::setTableConv(infoConv info);

private slots:
	void selectSvgClicked();
	void selectRepClicked();
	void toutCocherCheck();
	void convertirClicked();
	void selectSapCLicked();
	void selectXMLCLicked();
};


