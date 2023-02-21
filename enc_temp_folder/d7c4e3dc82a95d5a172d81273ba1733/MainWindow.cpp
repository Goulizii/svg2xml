#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include<QCheckBox>
#include <QDir>
#include<QHBoxLayout>
#include<QtWidgets>
#include <QTransform>
#include <QPainterPath>
#include <string>
#include "MainWindow.h"
#include <QtXml/QDomDocument>



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
	//la liste qui va stocker les paths
	QList<QString> pathsFromFile;
	for (int i = 0; i < listFileSvg.count(); ++i) {
		/*TOTO*/
		QList<QList<QString>> polyLines;
		pathsFromFile = getPathsFromFile(listFileSvg[i]->text());
		//pathToPolyLine(pathsFromFile, &polyLines);
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


QList<QString> MainWindow::getPathsFromFile(QString filePath)
{
	QList<QString> listPaths;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return listPaths;

	QXmlStreamReader xml(&file);
	while (!xml.atEnd() && !xml.hasError())
	{
		// recherche des éléments de type "path"
		if (xml.name() == "path")
		{
			//on recupere le path complet pour l'ajouter a listPaths
			QString path = xml.attributes().value("d").toString();
			if (!path.isEmpty())
				qDebug() << path;
				listPaths.append(path);
		}
		xml.readNext();
	}
	if (xml.hasError())
		return listPaths;

	file.close();
	return listPaths;
}


void MainWindow::pathToPolyLine(QList<QString> paths, QList<QString>* polyLines) {
	for (int i = 0; i < paths.count(); ++i)
		;

}