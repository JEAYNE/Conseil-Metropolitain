// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"


void MainWindow::showSummary(QTextCursor cursor, const QList<InfoCommune*>& sortBy){
    QTextTable *table = cursor.insertTable(nbrCommunes+2, 12);
    table->setFormat(tblFormat);

    int sumPopulation = 0,
        sumSieges1 = 0,
        sumSieges2 = 0,
        sumSieges3 = 0,
        sumSieges4 = 0,
        sumSieges5 = 0,
        sumSieges  = 0;

    // Header
    setCell(table, 0,  0, "Communes",        boldCharFormat);
    setCell(table, 0,  1, "    Populations", boldCharFormat);
    setCell(table, 0,  2, "    (% pop)",     boldCharFormat);
    setCell(table, 0,  3, "    C1",          boldCharFormat);
    setCell(table, 0,  4, "    C2",          boldCharFormat);
    setCell(table, 0,  5, "    C3",          boldCharFormat);
    setCell(table, 0,  6, "    C4",          boldCharFormat);
    setCell(table, 0,  7, "    C5",          boldCharFormat);
    setCell(table, 0,  8, "    Sièges",      boldCharFormat);
    setCell(table, 0,  9, "    (% sièges)",  boldCharFormat);
    setCell(table, 0, 10, "    (% Ecart)",   boldCharFormat);
    setCell(table, 0, 11, "    Moyennes",    boldCharFormat);

    int line = 0;
    for( const auto *const commune : sortBy ){
        line ++;
        setCell(table, line,  0, commune->nom);
        setCell(table, line,  1, commune->population);
        setCell(table, line,  2, commune->rp);
        setCell(table, line,  3, commune->sieges1);
        setCell(table, line,  4, commune->sieges2);
        setCell(table, line,  5, commune->sieges3);
        setCell(table, line,  6, commune->sieges4);
        setCell(table, line,  7, commune->sieges5);
        setCell(table, line,  8, commune->totalSieges());
        setCell(table, line,  9, commune->rs_av);
        setCell(table, line, 10, commune->ecart_av);
        setCell(table, line, 11, commune->moyenne);
        sumPopulation += commune->population;
        sumSieges1 += commune->sieges1;
        sumSieges2 += commune->sieges2;
        sumSieges3 += commune->sieges3;
        sumSieges4 += commune->sieges4;
        sumSieges5 += commune->sieges5;
        sumSieges  += commune->totalSieges();
    }

    line ++;
    setCell(table, line,  0, "TOTAUX",       boldCharFormat);
    setCell(table, line,  1, sumPopulation);
    setCell(table, line,  2, 100.0);
    setCell(table, line,  3, sumSieges1);
    setCell(table, line,  4, sumSieges2);
    setCell(table, line,  5, sumSieges3);
    setCell(table, line,  6, sumSieges4);
    setCell(table, line,  7, sumSieges5);
    setCell(table, line,  8, sumSieges);
    setCell(table, line,  9, 100.0);
    setCell(table, line, 10, 0.0);
    setCell(table, line, 11, static_cast<float>(sumPopulation) / sumSieges);

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");
}


void MainWindow::step_Summary(){
#if 0
    auto te = ui->textEdit_Summary;
    te->clear();
#else
    auto * const te = getNewTextEdit(ui->tab_Summary);
#endif

    QTextCursor cursor(te->textCursor());

    cursor.insertHtml("<h1>Info générales</h1><br/>");
    QTextTable *table = cursor.insertTable(8, 2);
    table->setFormat(tblFormat);

    setCell(table, 0,  0, "METROPOLE :",                 boldCharFormat);
    setCell(table, 0,  1, nomMetropole);

    setCell(table, 1,  0, "Nombre de communes :",        boldCharFormat);
    setCell(table, 1,  1, QString::number(nbrCommunes));

    setCell(table, 2,  0, "Population totale :",         boldCharFormat);
    setCell(table, 2,  1, QString::number(popMetropole));

    setCell(table, 3,  0, "Nombre de sièges :",          boldCharFormat);
    auto txt = QString("<p style=\"font-size:12pt;\">Base: %1<br/>Plein droit: %2<br/>Maxi: %3</p>")
               .arg(siegesBase)
               .arg(siegesDroit)
               .arg(siegesMax);
    setHtmlCell(table, 3,  1, txt);

    InfoCommune* big = parPopulation[0];
    setCell(table, 4,  0, "VILLE CENTRE :",                    boldCharFormat);
    setCell(table, 4,  1, big->nom);

    setCell(table, 5,  0, "Population :",                      boldCharFormat);
    setCell(table, 5,  1,  QString::number(big->population));

    setCell(table, 6,  0, "Nombre de sièges :",                boldCharFormat);
    setCell(table, 6,  1, QString::number(big->totalSieges()));

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br/>");

    cursor.insertHtml("<h1>Ensemble des sièges obtenus par chaque commune à chaque étape</h1><br/>");

    const Modes mode = ui->comboBox_ModeCalcul->currentIndex()==0 ? AVANT : APRES;
    const auto *modeStr = mode==AVANT ? "AVANT" : "APRES";

    cursor.insertHtml( QString(
        "<p style=\"font-size:12pt;\">"
        "&#9888; Les sièges optionnels, attribués à l'onglet C.5, l'ont été sur la base de l'écart "
        "de représentativité %1 l'attribution de chaque siège.</p><br/>"
        ).arg(modeStr));

    // calcule l'etat final de chaque commune
    for( auto* const commune : std::as_const(parPopulation) ){
        // commune->rp;   // (100.0 * commune->population)/populationMetropole (ne varie pas)
        commune->moyenne = static_cast<float>(commune->population) / commune->totalSieges();
        commune->rs_av    = (100.0 * commune->totalSieges())/siegesMetropole;
        commune->ecart_av = 100 * (1.0 - commune->rp/commune->rs_av);
    }

    cursor.insertHtml("<h2>Communes classées par ordre alphabétique :</h1><br/>");
    showSummary(cursor, parNom);
    cursor.movePosition(QTextCursor::End);

    cursor.insertHtml("<h2>Communes classées par ordre décroissant de population :</h1><br/>");
    showSummary(cursor, parPopulation);

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
