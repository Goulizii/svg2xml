#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include<QCheckBox>
#include <QDir>
#include<QHBoxLayout>
#include<QtWidgets>
#include <QPointF>
#include <QPainterPath>
#include <QTransform>
#include <string>
#include "MainWindow.h"
#include <QtXml/QDomDocument>
#include <QtSvg/QSvgRenderer>
#include <experimental/filesystem>


using namespace std;

QString repCourant;
QString outputDir;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
	////change le fond des layout
	//ui->selectVerticalLayout->setAutoFillBackground(true);
	//QPalette palette = ui->selectVerticalLayout->palette();
	//palette.setColor(QPalette::Background, Qt::red);
	//ui->selectVerticalLayout->setPalette(palette);
	//ui->convertVerticalLayout->setStyleSheet("background-color: red;");

	ui->setupUi(this);
	ui->svgFileTableWidget->setColumnWidth(1, 70);
	ui->svgFileTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	repCourant = QDir::currentPath();
	outputDir = repCourant + "Conversion SVG XML daher/00_CSV ZIN SAP";
	ui->RepSelectSortiLabel->setText(repCourant);
	ui->RepEntreeSapLabel->setText(outputDir);

	ui->toutCheckBox->setChecked(true);

	int col;
	while(col<5)
	{
		col = ui->convTableWidget->columnCount();
		ui->convTableWidget->insertColumn(col);

		switch (col)
		{
			case 0:
			{
				QTableWidgetItem* paramItem = new QTableWidgetItem("SVG");
				ui->convTableWidget->setItem(0, col, paramItem);
				ui->convTableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
				break;
			}
			case 1:
			{
				QTableWidgetItem* paramItem = new QTableWidgetItem("Article");
				ui->convTableWidget->setItem(0, col, paramItem);
				ui->convTableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
				break;
			}
			case 2:
			{
				QTableWidgetItem* paramItem = new QTableWidgetItem("Programme");
				ui->convTableWidget->setItem(0, col, paramItem);
				ui->convTableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
				break;
			}
			case 3:
			{
				QTableWidgetItem* paramItem = new QTableWidgetItem("Machine");
				ui->convTableWidget->setItem(0, col, paramItem);
				ui->convTableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
				break;
			}
			case 4:
			{
				QTableWidgetItem* paramItem = new QTableWidgetItem("Matiere");
				ui->convTableWidget->setItem(0, col, paramItem);
				ui->convTableWidget->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
				break;
			}
		}
	}
	
	QObject::connect(ui->selectSvgPushButton, SIGNAL(clicked()), this, SLOT(selectSvgClicked()));
	QObject::connect(ui->selectionRepPushButton, SIGNAL(clicked()), this, SLOT(selectRepClicked()));
	QObject::connect(ui->toutCheckBox, SIGNAL(stateChanged(int)), this, SLOT(toutCocherCheck()));
	QObject::connect(ui->ConvertPushButton, SIGNAL(clicked()), this, SLOT(convertirClicked()));
}

MainWindow::~MainWindow()
{}


void MainWindow::selectSvgClicked()
{
	QTableWidget *table = ui->svgFileTableWidget;
	//ouverture de la boite de dialogue pour selectionner les fichiers svg
	QStringList fileNames = QFileDialog::getOpenFileNames(this, "Selectionnez un ou plusieurs fichiers", QDir::currentPath(), "Fichiers SVG (*.svg)");

	//ajout des fichiers a la QlistWidget
	int row;
	foreach(const QString& fileName, fileNames) {
		if(table->findItems(fileName, Qt::MatchWrap | Qt::MatchWildcard).isEmpty()) {
			row = table->rowCount();
			table->insertRow(row);
			QTableWidgetItem* fileItem = new QTableWidgetItem(fileName);
			
			// Creation du widget personnalise pour centre la checkbox
			QWidget* checkBoxWidget = new QWidget();
			QCheckBox* checkBox = new QCheckBox();
			checkBox->setChecked(true);
			QHBoxLayout* layoutCheckBox = new QHBoxLayout(checkBoxWidget);
			layoutCheckBox->addWidget(checkBox);
			layoutCheckBox->setAlignment(Qt::AlignCenter);
			layoutCheckBox->setContentsMargins(0, 0, 0, 0);
			checkBoxWidget->setLayout(layoutCheckBox);

			table->setCellWidget(row, 1, checkBoxWidget);
			table->setItem(row, 0, fileItem);
		}
	}
}

void MainWindow::selectRepClicked() {
	QString directory = QFileDialog::getExistingDirectory(this, tr("Selectionner un repertoire"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->RepSelectSortiLabel->setText(directory);
}

void MainWindow::toutCocherCheck() {
	QCheckBox *tCheckBox = ui->toutCheckBox;
	QTableWidget *table = ui->svgFileTableWidget;

	if (tCheckBox->isChecked()) {
		tCheckBox->setText("tout decocher");
		for (int i = 0; i < table->rowCount(); ++i) {
			QWidget* widget = table->cellWidget(i, 1);
			QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget->layout()->itemAt(0)->widget());
			checkBox->setChecked(true);
			
		}
	}
	else {
		tCheckBox->setText("tout cocher");
		for (int i = 0; i < table->rowCount(); ++i) {
			QWidget* widget = table->cellWidget(i, 1);
			QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget->layout()->itemAt(0)->widget());
			checkBox->setChecked(false);
		}
	}
}

void MainWindow::convertirClicked() {
	QList<QTableWidgetItem*> svgFileList = getCheckedFile();
	//la liste qui va stocker les chemins
	QList<QList<QString>> pathsFromFile;

	for (int i = 0; i < svgFileList.count(); ++i) {
		/*TOTO*/
		//liste qui va contenir les polyLines du fichier filePath
		QList<int> pixelsList;
		QString filePath(svgFileList[i]->text());
		pathsFromFile = getPathsFromFile(filePath);
		pathToPolyLine(pathsFromFile, &pixelsList);
		writeAndSaveXML(pixelsList, filePath);
	}
}

QList<QTableWidgetItem*> MainWindow::getCheckedFile() {
	QTableWidget *table = ui->svgFileTableWidget;
	QList<QTableWidgetItem*> itemList;

	for (int i = 0; i < table->rowCount(); ++i) {
		QWidget* widget = table->cellWidget(i, 1);
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget->layout()->itemAt(0)->widget());
		if (checkBox->isChecked())
			itemList.append(table->item(i, 0));
	}
	return itemList;
}


QList<QList<QString>> MainWindow::getPathsFromFile(QString filePath)
{
	QList<QList<QString>> pathsList;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return pathsList;

	QXmlStreamReader xml(&file);
	while (!xml.atEnd() && !xml.hasError())
	{
		// recherche des éléments de type "chemin"
		if (xml.name() == "path")
		{
			//on recupere le chemin complet pour l'ajouter a pathsList
			QList<QString> chemin = xml.attributes().value("d").toString().split(" ");
			if (chemin[0] != "")
			{
				//qDebug() << chemin;
				pathsList.append(chemin);
			}
		}
		xml.readNext();
	}
	if (xml.hasError())
		return pathsList;

	file.close();
	return pathsList;
}

void MainWindow::pathToPolyLine(const QList<QList<QString>> paths, QList<int>* polyLines) {

	for (int i = 0; i < paths.count(); i++)
	{
		//qDebug() << paths[i];
		for (int j = 0; j < paths[i].count(); j++)
		{
			//Parcours de tous les paths pour créer la liste des points convertis en pixel 
			QString lettre = paths[i][j];

			if (lettre == "M" || lettre == "m")
			{

				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelX);
				polyLines->append(pixelY);
			}
			else if (lettre == "L" || lettre == "l")
			{
				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelX);
				polyLines->append(pixelY);
			}
			else if (lettre == "H" || lettre == "h")
			{
				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelX);
				polyLines->append(pixelY);
			}
			else if (lettre == "V" || lettre == "v")
			{
				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelX);
				polyLines->append(pixelY);
			}
			else if (lettre == "C" || lettre == "c")
			{
				//2 point de controle et 1 point pour la fin

				int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				int pixelC2X = round(paths[i][j + 3].toDouble() / 3.0235602);
				int pixelC2Y = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);
				int pixelEndX = round(paths[i][j + 5].toDouble() / 3.0235602);
				int pixelEndY = round((paths[i][j + 6].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelC1X);
				polyLines->append(pixelC1Y);
				polyLines->append(pixelC2X);
				polyLines->append(pixelC2Y);
				polyLines->append(pixelEndX);
				polyLines->append(pixelEndY);
			}
			else if (lettre == "S" || lettre == "s")
			{
				//1 point de controle et 1 point pour la fin

				int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				int pixelEndX = round(paths[i][j + 3].toDouble() / 3.0235602);
				int pixelEndY = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelC1X);
				polyLines->append(pixelC1Y);
				polyLines->append(pixelEndX);
				polyLines->append(pixelEndY);
			}
			else if (lettre == "Q" || lettre == "q")
			{
				//1 point de controle et 1 point pour la fin

				int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				int pixelEndX = round(paths[i][j + 3].toDouble() / 3.0235602);
				int pixelEndY = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelC1X);
				polyLines->append(pixelC1Y);
				polyLines->append(pixelEndX);
				polyLines->append(pixelEndY);
			}
			else if (lettre == "T" || lettre == "t")
			{
				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelX);
				polyLines->append(pixelY);
			}
			else if (lettre == "A" || lettre == "a")
			{
				int pixelX = round(paths[i][j + 6].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 7].toDouble() - 447) / 2.77083333);

				polyLines->append(pixelX);
				polyLines->append(pixelY);
			}
		}
	}

	
	//supression des doublons qui se suivent
	//TODO pas sur de l'utilité
	//delDuplicate(polyLines);
	//*polyLines = triPolyLines(polyLines);
	//qDebug() << (*polyLines);
}

//void MainWindow::pathToPolyLine(const QList<QList<QString>> paths, QList<QPoint>* polyLines) {
//
//	for (int i = 0; i < paths.count(); i++)
//	{
//		qDebug() << paths[i];
//		for (int j = 0; j < paths[i].count(); j++)
//		{
//			Parcours de tous les paths pour créer la liste des points convertis en pixel 
//			QString lettre = paths[i][j];
//
//			if (lettre == "M" || lettre == "m")
//			{
//
//				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				QPoint point(pixelX, pixelY);
//				polyLines->append(point);
//			}
//			else if (lettre == "L" || lettre == "l")
//			{
//				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				QPoint point(pixelX, pixelY);
//				polyLines->append(point);
//			}
//			else if (lettre == "H" || lettre == "h")
//			{
//				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				QPoint point(pixelX, pixelY);
//				polyLines->append(point);
//			}
//			else if (lettre == "V" || lettre == "v")
//			{
//				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				QPoint point(pixelX, pixelY);
//				polyLines->append(point);
//			}
//			else if (lettre == "C" || lettre == "c")
//			{
//				2 point de controle et 1 point pour la fin
//
//				int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				int pixelC2X = round(paths[i][j + 3].toDouble() / 3.0235602);
//				int pixelC2Y = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);
//				int pixelEndX = round(paths[i][j + 5].toDouble() / 3.0235602);
//				int pixelEndY = round((paths[i][j + 6].toDouble() - 447) / 2.77083333);
//
//				QPoint pointC1(pixelC1X, pixelC1Y);
//				QPoint pointC2(pixelC2X, pixelC2Y);
//				QPoint pointEnd(pixelEndX, pixelEndY);
//				polyLines->append(pointC1);
//				polyLines->append(pointC2);
//				polyLines->append(pointEnd);
//			}
//			else if (lettre == "S" || lettre == "s")
//			{
//				1 point de controle et 1 point pour la fin
//
//				int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				int pixelEndX = round(paths[i][j + 3].toDouble() / 3.0235602);
//				int pixelEndY = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);
//
//				QPoint pointC1(pixelC1X, pixelC1Y);
//				QPoint pointEnd(pixelEndX, pixelEndY);
//				polyLines->append(pointC1);
//				polyLines->append(pointEnd);
//			}
//			else if (lettre == "Q" || lettre == "q")
//			{
//				1 point de controle et 1 point pour la fin
//
//				int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				int pixelEndX = round(paths[i][j + 3].toDouble() / 3.0235602);
//				int pixelEndY = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);
//
//				QPoint pointC1(pixelC1X, pixelC1Y);
//				QPoint pointEnd(pixelEndX, pixelEndY);
//				polyLines->append(pointC1);
//				polyLines->append(pointEnd);
//			}
//			else if (lettre == "T" || lettre == "t")
//			{
//				int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
//				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
//				QPoint point(pixelX, pixelY);
//				polyLines->append(point);
//			}
//			else if (lettre == "A" || lettre == "a")
//			{
//				int pixelX = round(paths[i][j + 6].toDouble() / 3.0235602);
//				int pixelY = round((paths[i][j + 7].toDouble() - 447) / 2.77083333);
//				QPoint point(pixelX, pixelY);
//				polyLines->append(point);
//			}
//		}
//	}
//
//	supression des doublons qui se suivent
//TODO pas sur de l'utilité
//delDuplicate(polyLines);
//*polyLines = triPolyLines(polyLines);
//qDebug() << (*polyLines);
//}


void MainWindow::delDuplicate(QList<QPoint>* polyLines)
{
	for (int i = 0; i < polyLines->count(); i++)
	{
		if ((*polyLines)[i].x() == (*polyLines)[i].y())
		{
			polyLines->removeAt(i);
		}
	}
}

QList<QPoint> MainWindow::triPolyLines(QList<QPoint>* polyLines)
{
	QList<QPoint> polyLinesTrie;
	for (int i = 0; i < polyLines->count(); i++)
	{
		polyLinesTrie.append((*polyLines)[i]);
		for (int j = 0; j < polyLines->count(); j++)
			if ((*polyLines)[i].y() == (*polyLines)[j].x() && i != j)
			{
				qDebug() << (*polyLines)[i].x() << (*polyLines)[i].y();
				qDebug() << (*polyLines)[j].x() << (*polyLines)[j].y();
				polyLinesTrie.append((*polyLines)[j]);
				polyLines->removeAt(j);
				break;
			}
	}
	return polyLinesTrie;
}

void MainWindow::writeAndSaveXML(QList<int> pixelsList, QString filePath)
{
	infoConv info; 
	int pos = filePath.lastIndexOf(".");
	filePath.remove(pos, filePath.length());
	QString cleSVG = filePath.split("/").last();
	recupSAP(cleSVG, &info);
	//qDebug() << "article" << info.article << endl << "machine" << info.machine << endl << "materiel" << info.materiel << endl << "programme" << info.programme;

	// Load the XML file
	qDebug() << "C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/04_Layout_generique_matiere/" + info.materiel + ".xml";
	QFile file("C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/04_Layout_generique_matiere/" + info.materiel + ".xml");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		//qDebug() << "C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/04_Layout_generique_matiere/" + info.materiel + ".xml";
		qDebug() << "echec ouverture fichier";
		return;
	}

	QDomDocument doc;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}
	file.close();

	// Find the desired tag
	QDomElement root = doc.documentElement();
	QDomNode configDataNode = root.firstChildElement("Configurations")
		.firstChildElement("ConfigData")
		.firstChildElement("Measuring")
		.firstChildElement("Objects")
		.firstChildElement("MeasuringObject")
		.firstChildElement("MPoly");


	if (!configDataNode.isNull()) {
		QDomElement configDataElem = configDataNode.toElement();
		QDomElement Points = doc.createElement("Points");
		configDataElem.appendChild(Points);
		for (int i = 0; i < pixelsList.size(); i++)
		{
			if (i % 2 == 0)
			{
				QDomElement Point = doc.createElement("Point");
				QDomElement X = doc.createElement("X");
				QDomElement Y = doc.createElement("Y");
				//Points.setAttribute("Y",pixelsList[i+1] );
				//Points.setAttribute("X",pixelsList[i] );
				QDomText  x_value = doc.createTextNode(QString::number(pixelsList[i]));
				QDomText  y_value = doc.createTextNode(QString::number(pixelsList[i + 1]));
				X.appendChild(x_value);
				Y.appendChild(y_value);

				Point.appendChild(X);
				Point.appendChild(Y);
				Points.appendChild(Point);
			}
		}
	}
	else
	{
		qDebug() << "fail write in xml file : " << cleSVG + "_" + info.programme + ".xml";
	}


	// Save the modified XML to a new file

	// Check if output folder exists	
	QString outDir = outputDir + "/" + info.machine + "_xml";
	if (!std::experimental::filesystem::is_directory(outDir.toStdString()) || !std::experimental::filesystem::exists(outDir.toStdString()))
	{
		qDebug() << "Creating" << outDir << "directory...";
		std::experimental::filesystem::create_directory(outDir.toStdString()); // create output folder
	}
	else
	{
		qDebug() << "Directory" << outDir << "already exist !";
	}


	qDebug() << outDir + "/" + cleSVG + "_" + info.programme + ".xml";
	QFile nouvFich(outDir + "/" + cleSVG + "_" + info.programme + ".xml");
	if (!nouvFich.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream stream(&nouvFich);
	stream << doc.toString();
	nouvFich.close();
}


void MainWindow::recupSAP(QString cleSVG, infoConv *info)
{
	QFile fichParc1("C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/00_CSV ZIN SAP/ZIN55_V3.csv");
	if (!fichParc1.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream streamParc1(&fichParc1);
	QString ligne;
	QStringList champs;
	// 1er parcours du fichier pour extraire le lien document/SVG a partir de la cle svg
	while (!streamParc1.atEnd()) 
	{
		ligne = streamParc1.readLine();
		champs = ligne.split('|');

		if (champs.size() > 5 && champs[4].contains(cleSVG)) {
			info->article = champs[1];
			break;
		}
	}
	//fermeture du fichier et du flux pour parcourir a nouveau le fichier
	fichParc1.close();
	
	QFile fichParc2("C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/00_CSV ZIN SAP/ZIN55_V3.csv");
	if (!fichParc2.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream streamParc2(&fichParc2);

	// 2eme parcours du fichier pour extraire la machine et le programme a partir de l'article
	while (!streamParc2.atEnd()) 
	{
		ligne = streamParc2.readLine();
		champs = ligne.split('|');
		if (champs.size() > 1)
		//on parcours toutes les lignes de ZIN55_V3.csv où l'article corespond pour recuperer la cellule MAGELIS
		while(champs.size() > 5 && champs[1].contains(info->article) && !streamParc2.atEnd()) 
		{
			//qDebug() << champs[5];
			if (champs[4].contains("MAGELIS_"))
			{
				QStringList parseMAGELIS = champs[4].split("MAGELIS_");
				if(parseMAGELIS[0] == "62121")
					info->machine = "Presse55T";
				else if(parseMAGELIS[0] == "61998")
					info->machine = "Presse 75T";
				else
					info->machine = "Inconnue";
				//il y a de esapces a la fin donc oblige de splilt a " " 
				info->programme = parseMAGELIS[1].split(" ").first();
			}
			ligne = streamParc2.readLine();
			champs = ligne.split("|");
		}
	}

	QFile fich("C:/Users/t.khallouf/source/repos/svg2xml/svg2xml/Conversion SVG XML daher/00_CSV ZIN SAP/ZIN41BV2.csv");
	if (!fich.open(QIODevice::ReadOnly | QIODevice::Text))
		return;


	QTextStream stream(&fich);

	while (!stream.atEnd())
	{
		ligne = stream.readLine();
		champs = ligne.split('\t');
		if (champs.size() > 13 && champs[1].contains(info->article)) {
			QString nomenclature = champs[12];
			//pour l'instant on a que ces 4 types de materiaux
			if (nomenclature.contains("ABS5045"))
			{
				info->materiel = "Matiere CPPS";
				break;
			}
			else if (nomenclature.contains("ABS5222"))
			{
				info->materiel = "Matiere VPPS";
				break;
			}
			else if (nomenclature.contains("ABS5833"))
			{
				info->materiel = "Matiere CPEEK";
				break;
			}
			else if (nomenclature.contains("BAC1532"))
			{
				info->materiel = "Matiere CPEKK";
				break;
			}
			else
			{
				info->materiel = "Inconnue";
				break;
			}
		}
	}
	//fermeture du fichier et du flux pour parcourir a nouveau le fichier
	fich.close();
}

void MainWindow::setTableConv(infoConv info)
{

}