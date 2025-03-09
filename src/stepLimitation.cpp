// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"

void MainWindow::step_Limitation() {
#if 0
    auto te = ui->textEdit_Limitation;
    te->clear();
#else
    auto * const te = getNewTextEdit(ui->tab_Limitation);
#endif

    QTextCursor cursor(te->textCursor());
    const char* p;

    cursor.insertHtml("<h1>Limitation à la moitié</h1><br/>");

    const int limite = static_cast<int>(siegesMetropole / 2.0);

    p = "<p style=\"font-size:12pt;\">"
        "Si une commune a plus de la moitié des sièges du conseil métropolitain, donc plus de %1 "
        "sièges dans le cas de la métropole de %2, les sièges au-delà de cette limite sont "
        "réattribués aux autres communes suivant la régle de la plus forte moyenne. Cette "
        "éventuelle redistribution ne change pas le nombre de sièges de plein droit qui reste à %3."
        "<br/><br/></p>";
    cursor.insertHtml(QString(p).arg(limite).arg(nomMetropole).arg(siegesDroit));

    InfoCommune* tooMuch = nullptr;
    for (auto* const commune : std::as_const(parPopulation)) {
        if (commune->totalSieges() > limite) {
            tooMuch = commune;
            break;
        }
    }

    if (tooMuch == nullptr) {
        p = "<p style=\"color:green; font-size:12pt;\">"
            "<b> &rArr; Aucune commune de la métropole de %1 n'est concernée par cette limitation.</b>"
            "<br/></p><br/>";
        cursor.insertHtml(QString(p).arg(nomMetropole));
        redist = 0;
    } else {
        redist = tooMuch->totalSieges() - limite;

        p = "<p style=\"font-size:12pt;\">"
            "La commune de %1, avec ses %2 sièges actuels sur %3, est concernée par cette limitation "
            "à %4 sieges. %5 sièges vont lui être retirés et réattribués aux autres communes avec la "
            "règle de la plus forte moyenne.</p><br/>";
        cursor.insertHtml(
            QString(p).arg(tooMuch->nom).arg(tooMuch->totalSieges()).arg(siegesMetropole).arg(limite).arg(redist));

        QMap<QString, InfoCommune*> attributions;

        for (int siege = 1; siege <= redist; siege++) {

            // Calcule la moyenne de chaque commune (Sauf la ville majoritaire) dans l'hypothese d'un siège en plus
            // et détermine la quelle aurait la plus forte moyenne.
            float        maxMoyenne = 0;
            InfoCommune* winner     = nullptr;
            for (auto* const commune : std::as_const(parNom)) {
                if (commune->nom == tooMuch->nom) {
                    continue;
                }
                const auto moyenne = static_cast<float>(commune->population) / (commune->totalSieges() + 1);
                commune->moyenne   = moyenne;
                if (moyenne > maxMoyenne) {
                    maxMoyenne = moyenne;
                    winner     = commune;
                }
            }

            // Construire un index decroissant des moyennes SANS la ville majoritaire
            QList<InfoCommune*> parMoyenne;
            for (auto* const commune : std::as_const(parNom)) {
                if (commune->nom == tooMuch->nom) {
                    continue;
                }
                parMoyenne.push_back(commune);
            }
            std::sort(parMoyenne.begin(), parMoyenne.end(), [](const auto lhs, const auto rhs) {
                return rhs->moyenne < lhs->moyenne; // ordre décroissant 9->0
            });

            //---- Table des moyennes
            p = "<h3>Nouvelles moyennes dans l'<u>hypothèse</u> de la réattribution du siège N° %1 :</h3>";
            cursor.insertHtml(QString(p).arg(siege));

            QTextTable* table = cursor.insertTable(parMoyenne.count() + 1, 6);
            table->setFormat(tblFormat);

            // Header
            setCell(table, 0, 0, "Communes",                     boldCharFormat);
            setCell(table, 0, 1, "    Populations",              boldCharFormat);
            setCell(table, 0, 2, "    Sieges\nactuels",          boldCharFormat);
            setCell(table, 0, 3, "        Moyennes\nactuelles",  boldCharFormat);
            setCell(table, 0, 4, "    Sieges    \nactuels+1",    boldCharFormat);
            setCell(table, 0, 5, "        Nouvelle\nMoyennes ↓", boldCharFormat);

            // Affiche la moyenne pour chaque communes avec un hypothetique siège en plus
            int line = 0;
            for( const auto * const commune : parMoyenne ){
                if( commune->nom == tooMuch->nom)
                    continue;

                line++;
                const int totalSieges = commune->totalSieges();
                setCell(table, line, 0, commune->nom);
                setCell(table, line, 1, commune->population);
                setCell(table, line, 2, totalSieges);
                if( totalSieges > 0 ){
                    setCell(table, line, 3, static_cast<float>(commune->population) / totalSieges);
                }else{
                    setCell(table, line, 3, "-"); // impossible a ce stade
                }
                setCell(table, line, 4, totalSieges+1);
                setCell(table, line, 5, commune->moyenne);
            }

            cursor.movePosition(QTextCursor::End);
            cursor.insertHtml("<br/><br>");

            // conclusion
            tooMuch->sieges4--;
            winner->sieges4++;
            p = "<h3>&nbsp; &nbsp; &rArr; Le siège N° %1 est donc réattribué à %2 car "
                "avec %3 sièges sa moyenne de %4 hab/siège sera la plus forte</h3><br/><br/>";
            cursor.insertHtml(QString(p).arg(siege).arg(winner->nom).arg(winner->totalSieges()).arg(winner->moyenne));
            attributions[winner->nom] = winner;
        }

        //---- Résumé des réattributions
        auto nbrWinners            = attributions.size();
        attributions[tooMuch->nom] = tooMuch; // rajoute la commune majoritaire qui est perdante

        p = "<h2>RESUMÉ de la réattribution à %1 communes (à la plus forte moyenne) "
            "de %2 siéges pris à %3 :</h2>";
        cursor.insertHtml(QString(p).arg(nbrWinners).arg(redist).arg(tooMuch->nom));

        QTextTable* table = cursor.insertTable(nbrWinners + 2, 4);
        table->setFormat(tblFormat);

        // Header
        setCell(table, 0, 0, "Communes ↑",          boldCharFormat);
        setCell(table, 0, 1, "    Populations",     boldCharFormat);
        setCell(table, 0, 2, "    Siège attribués", boldCharFormat);
        setCell(table, 0, 3, "    Sieges",          boldCharFormat);

        int line = 0;
        for (const auto* const commune : std::as_const(attributions)) {
            line++;
            setCell(table, line, 0, commune->nom);
            setCell(table, line, 1, commune->population);
            setCell(table, line, 2, commune->sieges4);
            setCell(table, line, 3, commune->totalSieges());
        }

        cursor.movePosition(QTextCursor::End);
        cursor.insertHtml("<br/><br>");
    }

    //---- L5211-6-1 IV 4°
    //---- Vérifier que conseillers métropolitains <= conseillers municipaux

    bool fatalErr = false;
    for (const auto* const commune : std::as_const(parPopulation)) {
        if (commune->totalSieges() > commune->cm) {
            p = "<p style=\"font-size:12pt; color:red;\"><b>ATTENTION</b> : "
                "La commune de %1 a %2 sièges, soit %3 de plus que ses %4 conseillers municipaux.<br/></p>";
            cursor.insertHtml(QString(p)
                                  .arg(commune->nom)
                                  .arg(commune->totalSieges())
                                  .arg(commune->totalSieges() - commune->cm)
                                  .arg(commune->cm));
            fatalErr = true;
        }
    }

    if (fatalErr) {
        p = "<p style=\"font-size:12pt; color:red;\"><br/>"
            "Dans ce cas, l'article L5211-6-1 IV 4° prévoit de refaire le processus "
            "d'attribution avec un nombre total de conseillés métropolitains plus petit, jusqu'à ce "
            "que cette commune dispose d'un nombre total de sièges inférieur ou égal à celui de ses "
            "conseillers municipaux.<br/>"
            "<center><b>"
            "MAIS CE CAS N'EST PAS PRIS EN CHARGE PAR CE PROGRAMME.<br/>"
            "Les résultats donnés par ce programme pour la métropole de %1 ne sont pas utilisables.<br/>"
            "</b></center></p>";
    } else {
        p = "<p style=\"font-size:12pt;\">"
            "<b>NOTE</b> : A aucune commune il n'a été attribué plus de sièges au conseil de la métrople "
            "de %1 que ce qu'elle a de conseillers municipaux.<br/></p>";
    }
    cursor.insertHtml(QString(p).arg(nomMetropole));

    //---- Etat aprés une éventuelle réattribution
    p = "<hr><h1>Répartition des %1 sieges de plein droit :</h1>"
        "<p style=\"font-size:12pt;\">"
        "En absence d'un <i>accord local</i> c'est la répartition qui sera mise en oeuvre.</b><br/>"
        "La majorité absolue est à %3 voix."
        "<h2>Classement des communes par population<br>"
        "<i>(qui doit induire le même classement par sièges)</i></h2>";
    cursor.insertHtml(QString(p).arg(siegesMetropole).arg(majority()));

    QTextTable *table = cursor.insertTable(nbrCommunes+2, 4);
    table->setFormat(tblFormat);

    setCell(table, 0, 0, "Communes",       boldCharFormat);
    setCell(table, 0, 1, "    Population ↓", boldCharFormat);
    setCell(table, 0, 2, "    Sièges ↓",     boldCharFormat);
    setCell(table, 0, 3, "    Moyenne",    boldCharFormat);

    int line = 0;
    for (auto* const commune : std::as_const(parPopulation)) {
        line++;
        setCell(table, line, 0, commune->nom);
        setCell(table, line, 1, commune->population);
        setCell(table, line, 2, commune->totalSieges());
        if (commune->totalSieges() > 0) {
            setCell(table, line, 3, static_cast<float>(commune->population) / commune->totalSieges());
        } else {
            // cas impossible a ce stade
            setCell(table, line, 3, "_"); // nbr de siege nul; calcul de la moyenne impossible
        }
    }

    line ++;
    setCell(table, line, 0, "TOTAL métropole",            boldCharFormat);
    setCell(table, line, 1, popMetropole,                 boldCharFormat);
    setCell(table, line, 2, siegesMetropole,              boldCharFormat);
    setCell(table, line, 3, popMetropole/siegesMetropole, boldCharFormat);

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br>");

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
