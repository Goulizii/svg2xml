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


using namespace std;
QString currentPath;

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

	currentPath  = QDir::currentPath();
	ui->RepSelectLabel->setText(currentPath);

	ui->toutCheckBox->setChecked(true);
	
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
	//ouverture de la boite de dialogue pour selectionner les fihcier svg
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
	ui->RepSelectLabel->setText(directory);
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
	QList<QTableWidgetItem*> listFileSvg = getCheckedFile();
	//la liste qui va stocker les chemins
	QList<QList<QString>> pathsFromFile;
	for (int i = 0; i < listFileSvg.count(); ++i) {
		/*TOTO*/
		QList<QPoint> polyLines;
		pathsFromFile = getPathsFromFile(listFileSvg[i]->text());
		pathToPolyLine(pathsFromFile, &polyLines);
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
	QList<QList<QString>> listChemin;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return listChemin;

	QXmlStreamReader xml(&file);
	while (!xml.atEnd() && !xml.hasError())
	{
		// recherche des éléments de type "chemin"
		if (xml.name() == "path")
		{
			//on recupere le chemin complet pour l'ajouter a listChemin
			QList<QString> chemin = xml.attributes().value("d").toString().split(" ");
			if (chemin[0] != "")
			{
				//qDebug() << chemin;
				listChemin.append(chemin);
			}
		}
		xml.readNext();
	}
	if (xml.hasError())
		return listChemin;

	file.close();
	return listChemin;
}


void MainWindow::pathToPolyLine(const QList<QList<QString>> chemins, QList<QPoint>* polyLines) {

	for (int i = 0; i < chemins.count(); i++)
	{
		//qDebug() << chemins[i];
		for (int j = 0; j < chemins[i].count(); j++)
		{
			//Parcours de tous les chemins pour créer la liste des points convertis en pixel 
			QString lettre = chemins[i][j];

			if (lettre == "M" || lettre == "m")
			{

				int pixelX = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);
				polyLines->append(point);
			}
			else if (lettre == "L" || lettre == "l")
			{
				int pixelX = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);
				polyLines->append(point);
			}
			else if (lettre == "H" || lettre == "h")
			{
				int pixelX = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);
				polyLines->append(point);
			}
			else if (lettre == "V" || lettre == "v")
			{
				int pixelX = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);
				polyLines->append(point);
			}
			else if (lettre == "C" || lettre == "c")
			{
				//2 point de controle et 1 point pour la fin

				int pixelC1X = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelC1Y = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				int pixelC2X = round(chemins[i][j + 3].toDouble() / 3.0235602);
				int pixelC2Y = round((chemins[i][j + 4].toDouble() - 447) / 2.77083333);
				int pixelEndX = round(chemins[i][j + 5].toDouble() / 3.0235602);
				int pixelEndY = round((chemins[i][j + 6].toDouble() - 447) / 2.77083333);

				QPoint pointC1(pixelC1X, pixelC1Y);
				QPoint pointC2(pixelC2X, pixelC2Y);
				QPoint pointEnd(pixelEndX, pixelEndY);
				polyLines->append(pointC1);
				polyLines->append(pointC2);
				polyLines->append(pointEnd);
			}
			else if (lettre == "S" || lettre == "s")
			{
				//1 point de controle et 1 point pour la fin

				int pixelC1X = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelC1Y = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				int pixelEndX = round(chemins[i][j + 3].toDouble() / 3.0235602);
				int pixelEndY = round((chemins[i][j + 4].toDouble() - 447) / 2.77083333);

				QPoint pointC1(pixelC1X, pixelC1Y);
				QPoint pointEnd(pixelEndX, pixelEndY);
				polyLines->append(pointC1);
				polyLines->append(pointEnd);
			}
			else if (lettre == "Q" || lettre == "q")
			{
				//1 point de controle et 1 point pour la fin

				int pixelC1X = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelC1Y = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				int pixelEndX = round(chemins[i][j + 3].toDouble() / 3.0235602);
				int pixelEndY = round((chemins[i][j + 4].toDouble() - 447) / 2.77083333);

				QPoint pointC1(pixelC1X, pixelC1Y);
				QPoint pointEnd(pixelEndX, pixelEndY);
				polyLines->append(pointC1);
				polyLines->append(pointEnd);
			}
			else if (lettre == "T" || lettre == "t")
			{
				int pixelX = round(chemins[i][j + 1].toDouble() / 3.0235602);
				int pixelY = round((chemins[i][j + 2].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);
				polyLines->append(point);
			}
			else if (lettre == "A" || lettre == "a")
			{
				int pixelX = round(chemins[i][j + 6].toDouble() / 3.0235602);
				int pixelY = round((chemins[i][j + 7].toDouble() - 447) / 2.77083333);
				QPoint point(pixelX, pixelY);
				polyLines->append(point);
			}
		}
	}

	
	//supression des doublons qui se suivent
	//TODO pas sur de l'utilité
	delDuplicate(polyLines);
	*polyLines = triPolyLines(polyLines);
	qDebug() << (*polyLines);
}

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