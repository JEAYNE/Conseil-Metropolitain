// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "LoadCommunes.h"
#include "version.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTabBar>

#include <QTextBlockFormat>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , canPrint(false)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(APP_DESC);
    buildFormats(ui->textEdit_Data);
    SetApropos();
    ui->tabWidget->tabBar()->setStyleSheet(
        "QTabBar::tab:selected {"
        "  font-weight: bold;"
        "  color: #00ff00;"
    //    "  background-color: rgb(0,0,255);"
        "}"
    );
    ui->tabWidget->setCurrentWidget(ui->tab_Chargement);
}

MainWindow::~MainWindow(){
    delete ui;
}

void msgErrorBox(const QString& msg){
    QMessageBox msgBox(QMessageBox::Critical, "ERREUR", msg);
    msgBox.exec();
}

void msgInfoBox(const QString& msg){
    QMessageBox msgBox(QMessageBox::Information, "INFO", msg);
    msgBox.exec();
}

QString getDataPath(){
    QDir dir = QCoreApplication::applicationDirPath();
    QString cPath = dir.canonicalPath();
    int pos = cPath.lastIndexOf("/build/");
    if( pos>0 ){
        // dev
        cPath.truncate(pos);
        cPath.append("/data");
    }else{
        // prod
        cPath.append("/../donnees");
    }
    return cPath;
}

void MainWindow::on_comboBox_metropole_activated(int index){
    if( index==0 ) return;   // c'est la premiere ligne avec le message "Selectionnez ..."
    const auto ville = ui->comboBox_metropole->currentText();
    const auto dataFile = getDataPath().append("/2025/").append(ville).append(".csv");
    processDataFile(dataFile);
}

void MainWindow::on_pushButton_load_csv_clicked(){
    static QString loadFromPath;
    canPrint = false;

    if( loadFromPath.size()==0 ){
        loadFromPath = getDataPath();
    }
    const QString filename = QFileDialog::getOpenFileName(this,
        "Selectionner le fichier des communes et populations",
        loadFromPath,
        "Par défaut (*.csv);;Fichier texte (*.txt);;Tous (*.*)"
    );

    if( filename.size()==0 ){
        msgInfoBox("Pas de fichier sélectionné.");
        return;
    }

    QFileInfo fi(filename);
    loadFromPath = fi.absolutePath();

    processDataFile(filename);
}

void MainWindow::processDataFile(const QString& filename){

    communes = loadCommunes(filename, ';');
    if( communes.isEmpty() ){
        msgErrorBox(QString("Fichier: %1\n%2").arg(filename, "Aucune commune trouvée dans ce fichier."));
        return;
    }

    if( communes[0].population < 0 ){
        // Si la population de la premiere commune est négative c'est que le nom
        // contient un message d'erreur généré pendant la lecture du fichier csv
        msgErrorBox(QString("Fichier: %1\n%2").arg(filename, communes[0].nom));
        return;
    }

    nomMetropole = filename.split("/").last().split(".").first();

    //Ok, fichier chargé avec succés.
    buildIndex();

    step_AfficheListe();

    step_Proportionnelle();
    step_Moyenne();
    step_Bonus();
    step_Limitation();
    step_Options();
    step_Summary();

    canPrint = true;
    ui->tabWidget->setCurrentWidget(ui->tab_Data);

}

// Pour faciliter le parcours de la liste des communes on crée 2 index:
// - un pour accéder aux communes par ordre alphabétique croissant (A->Z)
// - un autre pour accéder aux communes par ordre décroissant (9->0) de population
// D'autre index plus spécifiques seront créés ailleurs
// Ces index sont des listes ordonnées de pointeurs pointant dans la liste des communes
void MainWindow::buildIndex(){
    // Initialiser l'index parNom d'apres l'ordre de lecture dans le fichier
    parNom.clear();
    for( auto item = communes.begin(); item != communes.end(); item++){
        parNom.push_back(&(*item));
    }

    // puis le trier par nom de commune
    std::sort(parNom.begin(), parNom.end(), [](const auto lhs, const auto rhs){
       return lhs->nom < rhs->nom;                  // ordre alphabétique A->B
    });

    // Cet index parNom servira par la suite de référence pour construire d'autres index.
    // En cas de valeurs identiques, si le tri utilisé est stable, le nom le plus petit
    // sera toujours le premier.

    // Dupliquer l'index parNom et trier la copie par population des communes
    parPopulation = parNom;
    std::sort(parPopulation.begin(), parPopulation.end(), [](const auto lhs, const auto rhs){
       return rhs->population < lhs->population;    // ordre décroissant 9->0
    });

    nbrCommunes = parNom.count();
}

// On capture le format par défaut d'un éditeur de texte
// et on derive quelques formats trés utilisés
void MainWindow::buildFormats(QTextEdit* te){
    QTextCursor cursor(te->textCursor());

    // Formatage des caracteres
    defaultCharFormat = cursor.charFormat();
    defaultCharFormat.setFontPointSize(12);

    boldCharFormat = defaultCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);

    // Formatage des blocks
    defaultBlockFormat = cursor.blockFormat();

    rightBlockFormat = defaultBlockFormat;
    rightBlockFormat.setAlignment(Qt::AlignRight);

    leftBlockFormat = defaultBlockFormat;
    leftBlockFormat.setAlignment(Qt::AlignLeft);

    // Formatage des tables
    QTextTable *table = cursor.insertTable(1, 1);
    defaultTableFormat = table->format();

    tblFormat = defaultTableFormat;
    tblFormat.setCellPadding(0);
    tblFormat.setLeftMargin(30);
    table->removeRows(0, table->rows());    // detruit la table
}

void MainWindow::setCell(QTextTable *table, int line, int col, const QString& text, const QTextCharFormat& charFmt, const QTextBlockFormat& blkFmt){
    QTextCursor cursor = table->cellAt(line, col).firstCursorPosition();
    cursor.insertText(text, charFmt);
    cursor.setBlockFormat(blkFmt);
}

void MainWindow::setCell(QTextTable *table, int line, int col, int val, const QTextCharFormat& charFmt, const QTextBlockFormat& blkFmt){
    setCell(table, line, col, QString::number(val),charFmt,blkFmt);
}

void MainWindow::setCell(QTextTable *table, int line, int col, double val, const QTextCharFormat& charFmt, const QTextBlockFormat& blkFmt){
    setCell(table, line, col, QString::number(val,'f',3),charFmt,blkFmt);
}

void MainWindow::setHtmlCell(QTextTable *table, int line, int col, const QString& html, const QTextBlockFormat& blkFmt){
    QTextCursor cursor = table->cellAt(line, col).firstCursorPosition();
    cursor.insertHtml(html);
    cursor.setBlockFormat(blkFmt);
}

//
// Variables Globale et Statique
//

QString nomMetropole;

QTextCharFormat  MainWindow::defaultCharFormat,
                 MainWindow::boldCharFormat;

QTextBlockFormat MainWindow::defaultBlockFormat,
                 MainWindow::rightBlockFormat,
                 MainWindow::leftBlockFormat;

QTextTableFormat MainWindow::defaultTableFormat,
                 MainWindow::tblFormat;
