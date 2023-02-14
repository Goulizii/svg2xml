#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include<QCheckBox>
#include <QDir>
#include<QHBoxLayout>
#include<QtWidgets>
#include <map>
#include <string>
#include "MainWindow.h"
#include <QtXml/QDomDocument>



using namespace std;


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

	QString currentPath = QDir::currentPath();
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
	//ouverture de la boite de dialogue pour séléctionner les fihcier svg
	QStringList fileNames = QFileDialog::getOpenFileNames(this, "Sélectionnez un ou plusieurs fichiers", QDir::currentPath(), "Fichiers SVG (*.svg)");

	//ajout des fichiers a la QlistWidget
	int row;
	foreach(const QString& fileName, fileNames) {
		if(table->findItems(fileName, Qt::MatchWrap | Qt::MatchWildcard).isEmpty()) {
			row = table->rowCount();
			table->insertRow(row);
			QTableWidgetItem* fileItem = new QTableWidgetItem(fileName);
			
			// Création du widget personnalisé pour centré la checkbox
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
	QString directory = QFileDialog::getExistingDirectory(this, tr("Sélectionner un répertoire"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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

	//creation d'une map avec comme clé un nom de fichier et comme valeur une liste des paths de ce fichier
	map<QString, QList<QString>> filesPaths;
	for (int i = 0; i < listFileSvg.count(); ++i) {
		filesPaths[listFileSvg[i]->text()] = getPathFromFile(listFileSvg[i]->text());
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

QList<QList<QString>> MainWindow::getPathFromFile(QString filePath) {
	// Ouverture du Fichier et creation de la liste Paths
	QDomDocument document;
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		document.setContent(&file);
		file.close();
	}
	//Obtention de l'element racine
	QDomElement xmlroot = document.firstChildElement();
	//Reperage des paths
	QDomNodeList paths = xmlroot.elementsByTagName("path");
	// Creation d'une liste contenant des listes contenant les contenues des paths en String
	QList<QList<QString>> listPaths;
	// Parcours de l'ensemble des paths
	for (int i = 0; i < paths.count(); ++i)
	{
		// Pour chaque path on crée un item
		QDomNode itemnode = paths.at(i);
		if (itemnode.isElement())
		{
			//conversion de item to element
			QDomElement itemle = itemnode.toElement();
			// detection de l'attribut qui nous intéresse
			QString item_txt = itemle.attribute("d");
			// separation des elements grace a l espace
			QList<QString> list_element_path = item_txt.split(' ');
			// stock dans une liste de liste
			listPaths.append(list_element_path);
		}
	}
	retlistPaths;
}

void MainWindow::detect_letter(QList<QList<QString> > List_paths)
{
	QList <int> List_L_count;
	QString l_letter = "L";
	//Detection du nombre de L dans chaque path
	for (int i = 0; i < List_paths.size(); i++)
	{
		// comptage du nombre
		int count_l = List_paths[i].count(l_letter);
		// stockage dans une liste
		List_L_count.append(count_l);

	}

	QList <int> List_A_count;
	QString a_letter = "A";
	//Detection du nombre de A dans chaque path
	for (int i = 0; i < List_paths.size(); i++)
	{
		// comptage du nombre
		int count_a = List_paths[i].count(a_letter);
		// stockage dans une liste
		List_A_count.append(count_a);

	}
	// On cree ensuite les points en fonction des lettres (Par manque d'exemple il faut ajouter les lettre manquantes dans la suite du développement)
	//create_liste_pt(List_paths, List_L_count, List_A_count);

}

