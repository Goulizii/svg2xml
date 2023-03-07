#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT



		typedef struct info_conversion_struct
		{
			QString article;
			QString machine;
			QString programme;
			QString materiel;
		}infoConversion;

public:
	explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;


	QString outputDirSvg = "C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher";
	QString inputDirSap = "C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/00_CSV ZIN SAP";
	QString inputDirXml = "C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/04_Layout_generique_matiere";


	
	const QList<QString> infosList = QList<QString>() << "SVG" << "Article"<< "Programme"<< "Machine"<< "Matiere";
	static const int nInfo = 5;

	//recupere les fichiers coches dans l'ui
	void MainWindow::getCheckedFile(QList<QTableWidgetItem*>* svgFileList, QList<int>* rowIds);

	//recupere toutles path svg d'un fichier donnee 
	QList<QList<QString>> MainWindow::getPathsFromFile(QString fileName);

	//TODO
	void MainWindow::pathsToPolyLines(const QList<QList<QString>> paths, QList<QPoint>* polyLines);

	//TODO verifier utilite
	QVector<QList<QPoint>> MainWindow::triPolyLines(QVector<QList<QPoint>> polyLines);

	void MainWindow::mergeTwoPaths(QList<QPoint>* path1, QList<QPoint> path2);

	//recupere l'XML de base et en sauvegarde une nouvelle version en fonction de la liste de points donnees en entree
	void MainWindow::writeAndSaveXml(const QList<QPoint> polyLines, const QString fileName, infoConversion* infos);

	//recupere les informations contenus dans les csv necessaires a la conversion
	void MainWindow::recupSAP(QString cleSvg, infoConversion* infos);

	//met a jour la table de l'ui afin d'afficher les information des fichiers convertis
	void MainWindow::setTableConversion(infoConversion* infos, const QList<int> rowIds);

private slots:
	void selectSvgClicked();
	void selectRepClicked();
	void toutCocherCheck();
	void convertirClicked();
	void selectSapCLicked();
	void selectXMLCLicked();
};


