// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"

void MainWindow::step_AfficheListe(){
#if 0
    auto te = ui->textEdit_Data;
    te->clear();
#else
    auto * const te = getNewTextEdit(ui->tab_Data);
#endif

    QTextCursor cursor(te->textCursor());

    cursor.insertHtml(QString("<p><h1>Liste des %1 communes de la métropole de %2</h1><br/></p>")
                      .arg(nbrCommunes)
                      .arg(nomMetropole));

    //|
    //| Première liste triée alphabétiquement
    //|

    cursor.insertHtml("<p><h1>1) Classées par ordre alphabétique</h1></p>");

    QTextTable *table1 = cursor.insertTable(nbrCommunes+2, 2);
    table1->setFormat(tblFormat);

    // Header
    setCell(table1, 0, 0, "Communes",        boldCharFormat);
    setCell(table1, 0, 1, "    Populations", boldCharFormat);

    // donnée de population pour chaque communes de la metropole
    int line = 0;
    popMetropole = 0;
    for (const auto * const commune : std::as_const(parNom)) {
        line++;
        popMetropole += commune->population;

        setCell(table1, line, 0, commune->nom);
        setCell(table1, line, 1, commune->population);
    }

    // Dernière ligne : total
    line++;
    setCell(table1, line, 0, "TOTAL métropole", boldCharFormat);
    setCell(table1, line, 1, popMetropole,      boldCharFormat);

    //|
    //| Deuxième liste triée par population
    //|

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><p><h1>2) Classées par population</h1></p>");

    QTextTable *table2 = cursor.insertTable(nbrCommunes+2, 2);
    table2->setFormat(tblFormat);

    // Header
    setCell(table2, 0, 0, "Communes",        boldCharFormat);
    setCell(table2, 0, 1, "    Populations", boldCharFormat);

    // total
    setCell(table2, 1, 0, "TOTAL métropole", boldCharFormat);
    setCell(table2, 1, 1, popMetropole,      boldCharFormat);

    // donnée de population pour chaque communes de la métropole
    line = 2;
    for (auto const * const commune : std::as_const(parPopulation)) {
        setCell(table2, line, 0, commune->nom);
        setCell(table2, line, 1, commune->population);
        line++;
    }

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
