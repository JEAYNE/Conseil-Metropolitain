// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"


//|
//| Article L5211-6-1 IV 1
//| 1° Les sièges à pourvoir prévus au tableau du III sont répartis entre les communes
//| à la représentation proportionnelle à la plus forte moyenne ...
//|

// A l'étape précédente un premier groupe de sièges a été attribué à la proportionnelle simple.
// Note: Le nombre de sièges etait arrondie à l'entier inférieur ( 3.1 ==> 3;  3.9 ==> 3)
//       si on alloue ainsi X sièges on a statistiquement X/2 sièges qui ne seront pas alloués
//       à cause de l'arrondi à l'entier inférieur.
// En pratique on a $reste sièges non alloués à l'étape précédente que l'on va attribuer
// un à un à cette étape selon la régle de la plus forte moyenne.
// Pour chaque commune on calcule la 'moyenne' (popCommune /(siègeCommune +1))
// Note: ceci n'est pas une moyenne mais c'est ainsi que l'usage nome ce coefficient dans ce
//       contexte. Il est parfois appelé 'quotient electoral'.
// Le siège est attribué à la commune qui a la plus forte 'moyenne'.
// Cette 'moyenne' représente le nombre d'habitants par siège.
// En attribuant le siège à la commune qui a la plus forte 'moyenne' on favorise la commune
// la moins bien representé (celle qui a le plus d'habitants par siège).
//
// Ceci correspond à la méthode de Jefferson ou méthode D'Hondt décrite sur Wikipedia
// https://fr.wikipedia.org/wiki/Scrutin_proportionnel_plurinominal#M%C3%A9thodes_de_la_plus_forte_moyenne
//

void MainWindow::step_Moyenne(){
#if 0
    auto te = ui->textEdit_Moyenne;
    te->clear();
#else
    auto * const te = getNewTextEdit(ui->tab_Moyenne);
#endif

    QTextCursor cursor(te->textCursor());

    QMap<QString, InfoCommune*> attributions;

    const float Q = static_cast<float>(popMetropole) / siegesMetropole;
    cursor.insertHtml( QString(
        "<h1>Proportionnelle à la plus forte moyenne</h1>"
        "<p style=\"font-size:12pt;\">"
        "Le terme '<i>moyenne</i> d'une commune' désigne le rapport '<i>Population municipale de la commune</i>' "
        "divisée par '<i>nombre de sièges de la commune au conseil métropolitain</i>'.<br/>"
        "<b>&rArr; Plus la moyenne d'une commune est forte plus ses habitants sont faiblement représentés.</b><br/>"
        "Attribuer un siège à une commune fait baisser sa moyenne et donc améliore la "
        "représentativité de ses habitants.<br/><br/>"
        "Pour comparaison la moyenne métropolitaine est à ce stade de %1 hab/siège.</p><br/>"
    ).arg(Q,0,'f',2));

    cursor.insertHtml( QString(
        "<h1>2/2) Attribution des %1 siéges restants à la plus forte moyenne</h1><br/>"
    ).arg(resteProportionnelle));

    for( int siege=1; siege<=resteProportionnelle; siege++){
        // Calcule la moyenne de chaque commune dans l'hypothese d'un siège en plus
        // et détermine la quelle aurait la plus forte moyenne.
        float maxMoyenne = 0;
        InfoCommune* winner = nullptr;
        for( auto *const commune : std::as_const(parNom) ){
            const auto moyenne = static_cast<float>(commune->population) / (commune->totalSieges() + 1);
            commune->moyenne = moyenne;
            if( moyenne > maxMoyenne ){
                maxMoyenne = moyenne;
                winner = commune;
            }
        }

        // Construire un index decroissant des moyennes
        auto parMoyenne = parNom;
        std::sort(parMoyenne.begin(), parMoyenne.end(), [](const auto lhs, const auto rhs){
           return rhs->moyenne < lhs->moyenne;    // ordre décroissant 9->0
        });

        //---- Table des moyennes
        cursor.insertHtml( QString(
            "<h3>Nouvelles moyennes dans l'<u>hypothèse</u> de l'attribution "
            "du siège N° %1 :</h3>"
        ).arg(siege));

        QTextTable *table = cursor.insertTable(nbrCommunes+1, 6);
        table->setFormat(tblFormat);

        // Header
        setCell(table, 0, 0, "Communes",                    boldCharFormat);
        setCell(table, 0, 1, "    Populations",             boldCharFormat);
        setCell(table, 0, 2, "    Sieges\nactuels",         boldCharFormat);
        setCell(table, 0, 3, "        Moyennes\nactuelles", boldCharFormat);
        setCell(table, 0, 4, "    Sieges    \nactuels+1",   boldCharFormat);
        setCell(table, 0, 5, "        Nouvelles\nMoyennes", boldCharFormat);

        // Affiche la moyenne pour chaque communes avec un hypothetique siège en plus
        int line = 0;
        for( const auto * const commune : parMoyenne ){
            line++;
            const int totalSieges = commune->totalSieges();
            setCell(table, line, 0, commune->nom);
            setCell(table, line, 1, commune->population);
            setCell(table, line, 2, totalSieges);
            if( totalSieges > 0 ){
                setCell(table, line, 3, static_cast<float>(commune->population) / totalSieges);
            }else{
                setCell(table, line, 3, "-");
            }
            setCell(table, line, 4, totalSieges+1);
            setCell(table, line, 5, commune->moyenne);
        }

        cursor.movePosition(QTextCursor::End);
        cursor.insertHtml("<br/><br>");

        // conclusion
        winner->sieges2 ++;
        cursor.insertHtml( QString(
            "<h3>&nbsp; &nbsp; &rArr; Le siège N° %1 est donc attribué à %2 car sa moyenne "
            "de %3 hab/siège, avec %4 sièges, sera la plus forte</h3><br/><br/>"
        ).arg(siege).arg(winner->nom).arg(winner->moyenne).arg(winner->totalSieges()));
        attributions[winner->nom] = winner;
    }

    //---- Résumé des attributions
    const int nbrWinners = attributions.size();

    cursor.insertHtml( QString(
        "<h3>RESUME de l'attribution à %1 communes (à la plus forte moyenne) des %2 siéges "
        "restants aprés la proportionnelle simple :</h3>"
    ).arg(nbrWinners).arg(resteProportionnelle));

    QTextTable *table = cursor.insertTable(nbrWinners+1, 3);
    table->setFormat(tblFormat);

    setCell(table, 0, 0, "Communes",             boldCharFormat);
    setCell(table, 0, 1, "    Sièges attribués", boldCharFormat);
    setCell(table, 0, 2, "    Sièges",           boldCharFormat);

    int line = 0;
    for( auto * const commune : std::as_const(attributions) ){
        line++;
        setCell(table, line, 0, commune->nom);
        setCell(table, line, 1, commune->sieges2);
        setCell(table, line, 2, commune->totalSieges());
    }

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");

    //---- Liste finale triée par sièges puis population si meme nombre de siège
    // auto parSieges = parNom;
    // std::sort(parSieges.begin(), parSieges.end(), [](const auto lhs, const auto rhs){
    //    return(
    //        (rhs->totalSieges() < lhs->totalSieges())    // ordre décroissant 9->0
    //        ||
    //        ( (rhs->totalSieges() == lhs->totalSieges()) && (rhs->population < lhs->population) )
    //    );
    // });

    cursor.insertHtml( QString(
        "<h1>RESULTATS de la proportionnelle à la plus forte moyenne.<h1>"
        "<h2>Classement des communes par population<br>"
        "<i>(qui doit induire le même classement par sièges)</i></h2>"
    ));

    table = cursor.insertTable(nbrCommunes+2, 4);
    table->setFormat(tblFormat);

    setCell(table, 0, 0, "Communes",       boldCharFormat);
    setCell(table, 0, 1, "    Population", boldCharFormat);
    setCell(table, 0, 2, "    Sièges",     boldCharFormat);
    setCell(table, 0, 3, "    Moyenne",    boldCharFormat);

    line = 0;
    sansSiege = 0;
    for( auto * const commune : std::as_const(parPopulation) ){
        line++;
        setCell(table, line, 0, commune->nom);
        setCell(table, line, 1, commune->population);
        setCell(table, line, 2, commune->totalSieges());
        if( commune->totalSieges() > 0 ){
            setCell(table, line, 3, static_cast<float>(commune->population) / commune->totalSieges());
        }else{
            setCell(table, line, 3, "-");
            sansSiege++;
        }
    }

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");

    cursor.insertHtml(QString("<p style=\"font-size:12pt;\">"
                              "NOTES: Les %1 sièges ont tous été attribués à la proportionelle à "
                              "la plus forte moyenne, </p>" ).arg(siegesMetropole));

    if( sansSiege>0 ){
        cursor.insertHtml(QString("<p style=\"font-size:12pt;\">"
                              "mais à ce stade %1 des %2 communes (soit %4% des communes) n'ont "
                              "toujours pas de siège. "
                              "Cela tient au fait que leurs populations sont nettement inférieures "
                              "à la <i>moyenne</i> métropolitaine de %5 habitants pour un siège."
                              "</p>"
                             ).arg(sansSiege)                                    // %1
                              .arg(nbrCommunes)                                  // %2
                              .arg((100.0 * sansSiege) / nbrCommunes, 0, 'f', 1) // %3
                              .arg(static_cast<int>(Q)));                        // %4
    }else{
        cursor.insertHtml(QString("<p style=\"font-size:12pt;\">"
                                  "et aucune commune n'est sans siège.</p>"));
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br/>");

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
