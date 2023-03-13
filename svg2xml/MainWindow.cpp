#include <iostream>
#include <string>
#include "MainWindow.h"

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

#include <QtXml/QDomDocument>
#include <QtSvg/QSvgRenderer>
#include <experimental/filesystem>
#include <QVectorIterator>
#include<iterator>


using namespace std;
typedef QList<QPoint> PointList;


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowClass)
{

	ui->setupUi(this);
	ui->svgFileTableWidget->setColumnWidth(1, 70);
	ui->svgFileTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	ui->tabConvParamWidget->setCurrentIndex(0);
	ui->toutCheckBox->setChecked(true);
	
	QObject::connect(ui->selectSvgPushButton, SIGNAL(clicked()), this, SLOT(selectSvgClicked()));
	QObject::connect(ui->selectionRepPushButton, SIGNAL(clicked()), this, SLOT(selectRepClicked()));
	QObject::connect(ui->toutCheckBox, SIGNAL(stateChanged(int)), this, SLOT(toutCocherCheck()));
	QObject::connect(ui->ConvertPushButton, SIGNAL(clicked()), this, SLOT(convertirClicked()));
	QObject::connect(ui->selectionSapPushButton, SIGNAL(clicked()), this, SLOT(selectSapCLicked()));
	QObject::connect(ui->selectionXMLPushButton, SIGNAL(clicked()), this, SLOT(selectXMLCLicked()));
	
}

MainWindow::~MainWindow()
{}


void MainWindow::selectSvgClicked()
{
	QTableWidget *tableSvg = ui->svgFileTableWidget;
	//ouverture de la boite de dialogue pour selectionner les fichiers svg
	QStringList fileNames = QFileDialog::getOpenFileNames(this, "Selectionnez un ou plusieurs fichiers", QDir::currentPath(), "Fichiers SVG (*.svg)");

	//ajout des fichiers a la QlistWidget
	int row;
	foreach(const QString& fileName, fileNames) 
	{
		if(tableSvg->findItems(fileName, Qt::MatchWrap | Qt::MatchWildcard).isEmpty()) 
		{
			row = tableSvg->rowCount();
			tableSvg->insertRow(row);
			QTableWidgetItem* fileItem = new QTableWidgetItem(fileName);
			
			// Creation du widget personnalise pour centre la checkbox
			QWidget* checkBoxWidget = new QWidget();
			QCheckBox* checkBox = new QCheckBox();
			checkBox->setChecked(true);
			QHBoxLayout* checkBoxlayout = new QHBoxLayout(checkBoxWidget);
			checkBoxlayout->addWidget(checkBox);
			checkBoxlayout->setAlignment(Qt::AlignCenter);
			checkBoxlayout->setContentsMargins(0, 0, 0, 0);
			checkBoxWidget->setLayout(checkBoxlayout);

			tableSvg->setCellWidget(row, 1, checkBoxWidget);
			tableSvg->setItem(row, 0, fileItem);
		}
	}
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

void MainWindow::selectRepClicked() {
	QString directory = QFileDialog::getExistingDirectory(this, tr("Selectionner un repertoire"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->repSelectSortiLabel->setText(directory);
}

void MainWindow::selectSapCLicked()
{
	inputDirSap = QFileDialog::getExistingDirectory(this, tr("Selectionner le repertoire des SAP"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->repEntreeSapLabel->setText(inputDirSap);
}

void MainWindow::selectXMLCLicked()
{
	inputDirXml = QFileDialog::getExistingDirectory(this, tr("Selectionner le repertoire des xml generiques"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->repEntreeXmlLabel->setText(inputDirXml);
}

void MainWindow::convertirClicked() 
{
	if (ui->svgFileTableWidget->rowCount() == 0)
	{
		QMessageBox::critical(nullptr, "Error", "Select at least 1 svg file !");
		return;
	}

	if (outputDirSvg.isEmpty())
		outputDirSvg = QFileDialog::getExistingDirectory(this, tr("Selectionner un repertoire de sortie"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


	//recupere les fichiers a convertir ainsi que leurs numeros de ligne dans la table
	QList<QTableWidgetItem*> svgFileList;
	QList<int> rowIds;
	getCheckedFile(&svgFileList, &rowIds);

	//la liste qui va stocker les chemins
	QList<QList<QString>> pathsFromFile;
	
	//liste qui va contenir le chemin trie
	PointList polyLines;
	for (int iFich = 0; iFich < svgFileList.count(); ++iFich)
	{
		QString filePath(svgFileList[iFich]->text());
		infoConversion infosConversion;

		QFile file(filePath);

		//recupere le nom du fichier pour le passer en parametre dans writeAndSaveXML
		QString fileName =  QFileInfo(file).fileName();;
		pathsFromFile = getPathsFromFile(filePath);
		pathsToPolyLines(pathsFromFile);
		writeAndSaveXml(polyLines, fileName, &infosConversion);
		setTableConversion(&infosConversion, rowIds);
	}
}

void MainWindow::getCheckedFile(QList<QTableWidgetItem*>* svgFileList, QList<int>* rowIds) {
	QTableWidget *table = ui->svgFileTableWidget;

	for (int i = 0; i < table->rowCount(); ++i) {
		QWidget* widget = table->cellWidget(i, 1);
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget->layout()->itemAt(0)->widget());
		if (checkBox->isChecked())
		{
			svgFileList->append(table->item(i, 0));
			rowIds->append(i);

		}
	}
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
	{
		QMessageBox::critical(nullptr, "Error", "Failed read in svg !" + filePath);
		return pathsList;
	}

	file.close();
	return pathsList;
}



PointList MainWindow::pathsToPolyLines(const QList<QList<QString>> paths)
{
	QList<PointList> polyLines;
	for (int i = 0; i < paths.count(); i++)
	{
		PointList pathToAdd;
		for (int j = 0; j < paths[i].count(); j++)
		{
			//Parcours de tous les paths pour créer la liste des points convertis en pixel 
			QString lettre = paths[i][j];
			if (lettre == "M" || lettre == "m")
			{
				//int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//QPoint point(pixelX, pixelY);

				QPoint point(round(paths[i][j + 1].toDouble()) , round((paths[i][j + 2].toDouble())));

				pathToAdd.append(point);
				j += 2;
			}
			else if (lettre == "L" || lettre == "l")
			{
				//int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//QPoint point(pixelX, pixelY);

				QPoint point(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));

				pathToAdd.append(point);
				j += 2;
			}
			else if (lettre == "H" || lettre == "h")
			{
				/*int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);*/

				QPoint point(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));

				pathToAdd.append(point);
				j += 2;
			}
			else if (lettre == "V" || lettre == "v")
			{
				//int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//QPoint point(pixelX, pixelY);

				QPoint point(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));

				pathToAdd.append(point);
				j += 2;
			}
			else if (lettre == "C" || lettre == "c")
			{
				//2 point de controle et 1 point pour la fin

				//int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//int pixelC2X = round(paths[i][j + 3].toDouble() / 3.0235602);
				//int pixelC2Y = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);
				//int pixelEndX = round(paths[i][j + 5].toDouble() / 3.0235602);
				//int pixelEndY = round((paths[i][j + 6].toDouble() - 447) / 2.77083333);

				//QPoint pointC1(pixelC1X, pixelC1Y);
				//QPoint pointC2(pixelC2X, pixelC2Y);
				//QPoint pointEnd(pixelEndX, pixelEndY);

				QPoint pointC1(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));
				QPoint pointC2(round(paths[i][j + 3].toDouble()), round((paths[i][j + 4].toDouble())));
				QPoint pointEnd(round(paths[i][j + 5].toDouble()), round((paths[i][j + 6].toDouble())));

				pathToAdd.append(pointC1);
				pathToAdd.append(pointC2);
				pathToAdd.append(pointEnd);
				j += 6;
			}
			else if (lettre == "S" || lettre == "s")
			{
				//1 point de controle et 1 point pour la fin

				//int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//int pixelEndX = round(paths[i][j + 3].toDouble() / 3.0235602);
				//int pixelEndY = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);

				//QPoint pointC1(pixelC1X, pixelC1Y);
				//QPoint pointEnd(pixelEndX, pixelEndY);

				QPoint pointC1(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));
				QPoint pointEnd(round(paths[i][j + 3].toDouble()), round((paths[i][j + 4].toDouble())));

				pathToAdd.append(pointC1);
				pathToAdd.append(pointEnd);
				j += 4;
			}
			else if (lettre == "Q" || lettre == "q")
			{
				//1 point de controle et 1 point pour la fin

				//int pixelC1X = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelC1Y = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//int pixelEndX = round(paths[i][j + 3].toDouble() / 3.0235602);
				//int pixelEndY = round((paths[i][j + 4].toDouble() - 447) / 2.77083333);

				//QPoint pointC1(pixelC1X, pixelC1Y);
				//QPoint pointEnd(pixelEndX, pixelEndY);

				QPoint pointC1(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));
				QPoint pointEnd(round(paths[i][j + 3].toDouble()), round((paths[i][j + 4].toDouble())));

				pathToAdd.append(pointC1);
				pathToAdd.append(pointEnd);
				j += 4;
			}
			else if (lettre == "T" || lettre == "t")
			{
				//int pixelX = round(paths[i][j + 1].toDouble() / 3.0235602);
				//int pixelY = round((paths[i][j + 2].toDouble() - 447) / 2.77083333);
				//QPoint point(pixelX, pixelY);

				QPoint point(round(paths[i][j + 1].toDouble()), round((paths[i][j + 2].toDouble())));

				pathToAdd.append(point);
				j += 2;
			}
			else if (lettre == "A" || lettre == "a")
			{
				//int pixelX = round(paths[i][j + 6].toDouble() / 3.0235602);
				//int pixelY = round((paths[i][j + 7].toDouble() - 447) / 2.77083333);
				//QPoint point(pixelX, pixelY);

				QPoint point(round(paths[i][j + 6].toDouble()), round((paths[i][j + 7].toDouble())));

				pathToAdd.append(point);
				j += 7;
			}
		}
		polyLines.append(pathToAdd);
	}
	return triPolyLines(&polyLines);
}


//fusionne 2 paths si ils se suivent
bool mergePath(PointList& l1, PointList& l2)
{
	if (l1.last() == l2.first())
	{
		l1 << l2;
	}
	else if (l1.last() == l2.last())
	{
		std::reverse(l2.begin(), l2.end());
		l1 << l2;
	}
	else if (l1.first() == l2.first())
	{
		std::reverse(l1.begin(), l1.end());
		l1 << l2;
	}
	else if (l1.first() == l2.last())
	{
		l1 = l2 + l1;
	}
	else
		return false;
	return true;
};

//suprime les doublons
PointList cleanPolyLine(PointList polyLines)
{
	for (int iPoint = 0; iPoint < polyLines.count() - 1;)
	{
		if (polyLines[iPoint] == polyLines[iPoint + 1])
			polyLines.removeAt(iPoint + 1);
		else
			iPoint++;

	}
	return polyLines;
}


PointList MainWindow::triPolyLines(QList<PointList>* polyLines)
{
	for (int iStart = 0; iStart < polyLines->count(); iStart++)
		for (int iOther = iStart + 1 ; iOther < polyLines->count(); iOther++)
			if (mergePath((*polyLines)[iStart], (*polyLines)[iOther]))
			{
				polyLines->removeAt(iOther);
				iOther = iStart;
			}

	return cleanPolyLine(polyLines->first());
}




void MainWindow::writeAndSaveXml(const PointList polyLines, const QString fileName, infoConversion* infos)
{
	QList<QString> convertFile;

	//recuperation du nom du fichier pour l'inclure dans le nom xml
	QString cleSvg = fileName.split(".").first();
	//recuperation de toutes les infos
	recupSAP(cleSvg, infos);

	// charge le fichier xml generique
	if (inputDirXml.isEmpty())
	{
		QMessageBox::critical(nullptr, "Error", "Select directory for generic XML !");
		return;
	}
	//TODO changer par la variable matiere de infos
	QFile file(inputDirXml + "/Matiere CPEEK.xml");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "failed open Material file";
		return;
	}

	QDomDocument xmlCoreDoc;
	if (!xmlCoreDoc.setContent(&file)) {
		file.close();
		return;
	}
	file.close();

	// Find the desired tag
	QDomElement root = xmlCoreDoc.documentElement();
	QDomNode configDataNode = root.firstChildElement("Configurations")
									.firstChildElement("ConfigData")
									.firstChildElement("Measuring")
									.firstChildElement("Objects")
									.firstChildElement("MeasuringObject")
									.firstChildElement("MPoly");


	if (!configDataNode.isNull()) {
		QDomElement configDataElem = configDataNode.toElement();
		QDomElement Points = xmlCoreDoc.createElement("Points");
		configDataElem.appendChild(Points);
		for (int i = 0; i < polyLines.size(); i++)
		{
			if (i % 2 == 0)
			{
				QDomElement Point = xmlCoreDoc.createElement("Point");
				QDomElement X = xmlCoreDoc.createElement("X");
				QDomElement Y = xmlCoreDoc.createElement("Y");
				//Points.setAttribute("Y",polyLines[i+1]);
				//Points.setAttribute("X",polyLines[i]);
				QDomText  x_value = xmlCoreDoc.createTextNode(QString::number(polyLines[i].x()));
				QDomText  y_value = xmlCoreDoc.createTextNode(QString::number(polyLines[i].y()));
				X.appendChild(x_value);
				Y.appendChild(y_value);

				Point.appendChild(X);
				Point.appendChild(Y);
				Points.appendChild(Point);
			}
		}
	}


	// Save the modified XML to a new file
	
	QString outDirPath = outputDirSvg + "/" + infos->machine + "_xml";

	// Check if output folder exists
	QDir outDir(outDirPath);
	if (!outDir.exists())
	{
		if (!outDir.mkpath("."))
		{
			QMessageBox::critical(nullptr, "Error", "Failed to create directory: " + outDirPath);
			return;
		}
	}

	QFile nouvFich(outDirPath + "/" + cleSvg + "_" + infos->programme + ".xml");
	if (!nouvFich.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(nullptr, "Error", "Failed open " + outDirPath);
		return;
	}
	QTextStream stream(&nouvFich);
	stream << xmlCoreDoc.toString();

	nouvFich.close();
}


void MainWindow::recupSAP(QString cleSvg, infoConversion* infos)
{
	if (inputDirSap.isEmpty())
		QMessageBox::critical(nullptr, "Error", "Select directory for SAP files !");

	QFile fichParc1(inputDirSap + "/ZIN55_V3.csv");
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
		
		if (champs.size() > 5 && champs[4].contains(cleSvg)) {
			infos->article = champs[1];
			break;
		}
	}
	if(infos->article.isEmpty())
	{
		infos->article = "Inconnue";
		QMessageBox::critical(nullptr, "Error", "SVG article not found in SAP file !");
		return;

	}
	//fermeture du fichier et du flux pour parcourir a nouveau le fichier
	fichParc1.close();
	
	QFile fichParc2(inputDirSap +"/ZIN55_V3.csv");
	if (!fichParc2.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(nullptr, "Error", "Sap file is empty !");
		return;
	}
		QTextStream streamParc2(&fichParc2);

	// 2eme parcours du fichier pour extraire la machine et le programme a partir de l'article
	while (!streamParc2.atEnd()) 
	{
		ligne = streamParc2.readLine();
		champs = ligne.split('|');
		if (champs.size() > 1)
		//on parcours toutes les lignes de ZIN55_V3.csv où l'article corespond pour recuperer la cellule MAGELIS
		while(champs.size() > 5 && champs[1].contains(infos->article) && !streamParc2.atEnd()) 
		{
			//qDebug() << champs[5];
			if (champs[4].contains("MAGELIS_"))
			{
				QStringList parseMAGELIS = champs[4].split("MAGELIS_");
				if(parseMAGELIS[0] == "62121")
					infos->machine = "Presse55T";
				else if(parseMAGELIS[0] == "61998")
					infos->machine = "Presse 75T";
				else
					infos->machine = "Inconnue";
				//il y a de esapces a la fin donc oblige de splilt a " " 
				infos->programme = parseMAGELIS[1].split(" ").first();
			}
			ligne = streamParc2.readLine();
			champs = ligne.split("|");
		}
	}


	QFile fich(inputDirSap +"/ZIN41BV2.csv");
	if (!fich.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(nullptr, "Error", "Sap file is empty !");
		return;
	}


	QTextStream stream(&fich);

	while (!stream.atEnd())
	{
		ligne = stream.readLine();
		champs = ligne.split('\t');
		if (champs.size() > 13 && champs[1].contains(infos->article)) {
			QString nomenclature = champs[12];
			//pour l'instant on a que ces 4 types de materiaux
			if (nomenclature.contains("ABS5045"))
			{
				infos->materiel = "Matiere CPPS";
				break;
			}
			else if (nomenclature.contains("ABS5222"))
			{
				infos->materiel = "Matiere VPPS";
				break;
			}
			else if (nomenclature.contains("ABS5833"))
			{
				infos->materiel = "Matiere CPEEK";
				break;
			}
			else if (nomenclature.contains("BAC1532"))
			{
				infos->materiel = "Matiere CPEKK";
				break;
			}
			else
			{
				infos->materiel = "Inconnu";
				break;
			}
		}
	}
	//fermeture du fichier et du flux pour parcourir a nouveau le fichier
	fich.close();
}


void MainWindow::setTableConversion(infoConversion* infos, const QList<int> rowIds)
{
	int nRow = rowIds[rowIds.length() - 1] + 1;
	QTableView* tableInfos = ui->conversionTableView;
	QStandardItemModel* tableInfosModel = new QStandardItemModel(nRow, nInfo);

	//car on veut pouvoir modifier la taille sur cette colonne d'en tete
	tableInfosModel->setHorizontalHeaderItem(0, new QStandardItem(infosList[0]));

	for (int colHeader = 1; colHeader < nInfo; colHeader++)
	{
		tableInfosModel->setHorizontalHeaderItem(colHeader, new QStandardItem(infosList[colHeader]));

		//TODO chaque colonne du header doit completer l'espace sauf la premiere
		//QHeaderView *headerView = tableInfos->horizontalHeader();
		//headerView->setSectionResizeMode(colHeader, QHeaderView::Stretch);
	}

	for (const int rowId : rowIds)
	{
		for (int column = 0; column < nInfo ; column++)
		{
			//initialisation des 3 objets qui composeront le QTableWidgetItem

			QString textLabel;
			switch (column)
			{
				case 0:
				{
					textLabel = ui->svgFileTableWidget->item(rowId, column)->text();
					break;
				}
				case 1:
				{
					textLabel = infos->article;
					break;

				}
				case 2:
				{
					textLabel = infos->machine;
					break;
				}
				case 3:
				{
					textLabel = infos->programme;
					break;
				}
				case 4:
				{
					textLabel = infos->materiel;
					break;
				}
				default:
					break;
			}

			QStandardItem* infosConversionItem = new QStandardItem(textLabel);

			if (textLabel != "Inconnu" && textLabel != "Inconnue")
				infosConversionItem->setBackground(QColor(Qt::green));
			else
				infosConversionItem->setBackground(QColor(Qt::red));
			tableInfosModel->setItem(rowId, column, infosConversionItem);
			//tableInfos->setItem(rowId, column, infosConversionItem);
		}
	}
	tableInfos->setModel(tableInfosModel);
}

