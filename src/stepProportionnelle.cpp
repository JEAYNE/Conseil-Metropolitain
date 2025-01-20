// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"

void MainWindow::step_Proportionnelle(){

    //| Nombre initial de sieges
    //| Article L5211-6-1 III https://www.legifrance.gouv.fr/codes/article_lc/LEGIARTI000034116583
    //| Note: un test empêche d'executer cette fonction si siegesMetropole < 250'000

    if( popMetropole < 150'000 )
        siegesBase = 48;
    else if( popMetropole < 200'000 )
        siegesBase = 56;
    else if( popMetropole < 250'000 )
        siegesBase = 64;
    else if( popMetropole < 350'000 )
        siegesBase = 72;
    else if( popMetropole < 500'000 )
        siegesBase = 80;
    else if( popMetropole < 700'000 )
        siegesBase = 90;
    else if( popMetropole < 1'000'000 )
        siegesBase = 100;
    else siegesBase = 130;

    siegesMetropole = siegesBase;
    //|
    //| Article L5211-6-1 IV 1
    //| 1° Les sièges à pourvoir prévus au tableau du III sont répartis entre les communes
    //| à la représentation proportionnelle (à la plus forte moyenne ...)
    //|

    // La proportion de sieges d'une commune au sein de conseil metropolitain
    // doit etre egal à
    // la proportion de la population de la commune au sein de la population metropolitaine.
    //
    // Cette premiere etape est donc une regle de trois triviale.
    // On attribue a chaque commune un nombre de sieges proportionnel à la population de la commune:
    //
    // popCommune     siegesCommune
    // ------------ = --------------- ==> siegesCommune = popCommune * (siegesMetropole / popMetropole)
    // popMetropole   siegesMetropole
    //
    // Mais il est courant de présenter le quotient inverse Q=(pop_metropole / sieges_metropole)
    // qui represente le nombre d'habitants de la metropole par siege au conseil metropolitain.
    // C'est une valeur facile à comprendre. Exemple 6481 habitant par siege
    // On utilise alors la formule : siegesCommune = arrondi_inferieur(popCommune / Q)
    //
    const float Q = static_cast<float>(popMetropole) / siegesMetropole;
    int attrib = 0;
    for( auto & commune : communes ){
        const auto sieges = trunc(commune.population / Q);
        if( sieges>0 ){
            attrib += sieges;
            commune.sieges1 = sieges;
        }
    }
    resteProportionnelle = siegesMetropole - attrib;

    //|
    //| Affiche les résultats de la proportionnelle simple
    //|

#if 0
    auto te = ui->textEdit_Proportionnelle;
    te->clear();
#else
    auto * const te = getNewTextEdit(ui->tab_Proportionnelle);
#endif

    QTextCursor cursor(te->textCursor());

    cursor.insertHtml( QString(
        "<h1>Proportionnelle à la plus forte moyenne</h1>"
        "<p style=\"font-size:12pt;\">"
        "La population de la métropole de %1 étant de %2 habitants <b>%3 sieges</b> de son conseil "
        "métropolitains sont à attribuer à la proportionnelle à la plus forte moyenne."
        "</p><br/>"
    ).arg(nomMetropole).arg(popMetropole).arg(siegesMetropole));

    cursor.insertHtml( QString(
        "<h1>1/2) Proportionnelle simple</h1>"
        "<p style=\"font-size:12pt;\">"
        "Dans un premier temps la proportionnelle <i>simple</i> (règle de trois) permet d'attribuer "
        "%1 sièges sur les %3.<br/>Les %4 sièges restants seront attribués à l'étape suivante "
        "par la règle itérative de la <i>plus forte moyenne</i>.</p><br/>"
    ).arg(attrib).arg(siegesMetropole).arg(resteProportionnelle));

    cursor.insertHtml( QString(
        "<p style=\"font-size:12pt;\">"
        "Avec %1 sièges pour %2 habitants la <i>moyenne</i> métropolitaine est de %3 habitants/siege.<br/>"
        "Autant de fois une commune aura %3 habitants sur son territoire, autant de sieges "
        "lui seront attribués à cette étape.</p><br/>"
    ).arg(siegesMetropole).arg(popMetropole).arg(Q));

    const int nLines = communes.size();
    const int nCols = 3;

    //-- Classement alphabetique
    cursor.insertHtml("<h2>Classement des communes par ordre alphabétique :</h2>");

    QTextTable *table1 = cursor.insertTable(nLines+2, nCols);
    table1->setFormat(tblFormat);

    // Header
    setCell(table1, 0, 0, "Communes",        boldCharFormat);
    setCell(table1, 0, 1, "    Populations", boldCharFormat);
    setCell(table1, 0, 2, "    Sieges",      boldCharFormat);

    // donnée de population pour chaque communes de la metropole
    int line = 0;
    for( auto *commune : std::as_const(parNom) ){
        line++;
        setCell(table1, line, 0, commune->nom);
        setCell(table1, line, 1, commune->population);
        setCell(table1, line, 2, commune->sieges1);
    }

    // Dernière ligne : total
    line++;
    setCell(table1, line, 0, "TOTAL métropole", boldCharFormat);
    setCell(table1, line, 1, popMetropole,      boldCharFormat);
    setCell(table1, line, 2, attrib,            boldCharFormat);

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");

    //---- Classement par population
    cursor.insertHtml(
        "<h2>Classement des communes par ordre de population :<br/>"
        "(identique au classement par siège)</h2>"
    );

    QTextTable *table2 = cursor.insertTable(nLines+2, nCols);
    table2->setFormat(tblFormat);

    // Header
    setCell(table2, 0, 0, "Communes",        boldCharFormat);
    setCell(table2, 0, 1, "    Populations", boldCharFormat);
    setCell(table2, 0, 2, "    Sieges",      boldCharFormat);

    // nom, population, sieges1
    line = 0;
    for( auto *commune : std::as_const(parPopulation) ){
        line++;
        setCell(table2, line, 0, commune->nom);
        setCell(table2, line, 1, commune->population);
        setCell(table2, line, 2, commune->sieges1);
    }

    // Dernière ligne : total
    line++;
    setCell(table2, line, 0, "TOTAL métropole", boldCharFormat);
    setCell(table2, line, 1, popMetropole,      boldCharFormat);
    setCell(table2, line, 2, attrib,            boldCharFormat);

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br/>");

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
