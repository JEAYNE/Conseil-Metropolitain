// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"

void MainWindow::step_Bonus(){
#if 0
    auto te = ui->textEdit_Bonus;
    te->clear();
#else
    auto * const te = getNewTextEdit(ui->tab_Bonus);
#endif

    QTextCursor cursor(te->textCursor());

    cursor.insertHtml("<h1>Siège attribué d'office</h1><br/>");

    if( sansSiege == 0 ){
        cursor.insertHtml( QString(
            "<p style=\"font-size:12pt;\">"
            "Aucun siège n'est a attribuer d'office car toutes les communes de la métropole de %1 "
            "ont reçues un siège à l'étape précédente.<br/></p>"
        ).arg(nomMetropole));
        siegesDroit = siegesMetropole;
        return;
    }

    cursor.insertHtml( QString(
        "<p style=\"font-size:12pt;\">"
        "Un siège est attribué d'office à chacune des %1 communes qui, à l'issue des deux étapes "
        "de la proportionnelle à la plus forte moyenne, n'a toujours pas de siège.<br/>"
        "<i>NOTE : Ceci porte le nombre de sièges du conseil de la métropole de %2 de %3 à %4 (+%5%)</i>"
        "<br/></p>"
    ).arg(sansSiege)
     .arg(nomMetropole)
     .arg(siegesMetropole)
     .arg(siegesMetropole + sansSiege)
     .arg((100.0 * sansSiege) / siegesMetropole, 5, 'f', 2));

    QTextTable *table = cursor.insertTable(communes.size()+2, 4);
    table->setFormat(tblFormat);

    // Header
    setCell(table, 0, 0, "Communes",        boldCharFormat);
    setCell(table, 0, 1, "    Populations", boldCharFormat);
    setCell(table, 0, 2, "      +",         boldCharFormat);
    setCell(table, 0, 3, "    Sieges",      boldCharFormat);

    // nom, population, sieges3, flag
    int line = 0;
    //for( auto commune : parPopulation ){
    for( auto commune = parPopulation.rbegin(); commune!= parPopulation.rend(); ++commune){
        line++;
        if( (*commune)->totalSieges()==0){
            (*commune)->sieges3 = 1;
            siegesMetropole ++;
        }
        setCell(table, line, 0, (*commune)->nom);
        setCell(table, line, 1, (*commune)->population);
        setCell(table, line, 2, ((*commune)->sieges3==1 ? "+" : ""));
        setCell(table, line, 3, (*commune)->totalSieges());
    }

    // Dernière ligne : total
    line++;
    setCell(table, line, 0, "TOTAL métropole", boldCharFormat);
    setCell(table, line, 1, popMetropole,      boldCharFormat);
    setCell(table, line, 2, sansSiege,         boldCharFormat);
    setCell(table, line, 3, siegesMetropole,   boldCharFormat);

    siegesDroit = siegesMetropole;

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
