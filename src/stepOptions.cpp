// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editTools.h"

/*
Quand, dans le cadre de l’accord local, on attribue les sièges optionnels de manière 'algorithmique',
par opposition à 'politique', il y a deux manières de procéder. Dans les deux cas on tient compte
du rapport entre les représentativités en siège et en population (c'est la loi). Mais ce qui change
c'est à quel moment on calcule ce rapport. Est-ce AVANT ou APRES l'attribution d'un siège ?

1) La méthode AVANT tient compte de l'état du quel on sort la commune en lui attribuant un siège.

2) La méthode APRES tient compte de l'état dans le quel on met la commune en lui l'attribution un siège.
C'est la méthode utilisée dans le calcul ‘à la plus forte moyenne’.

Les résultats des 2 méthodes sont trés différents.

* Exemple: si une commune est dans un 'mauvais' état, mais que l'attribution d'un siège la met
dans un 'trop bon état', alors la méthode APRES ne lui attribuera pas le siège, alors que la méthode
AVANT le lui attribuera. Ce phénomène concerne surtout les communes avec peu de sièges. Ajouter un
siège à une commune qui n'en a qu'un double son effectif (+100%) alors qu'attribuer ce siège à une
commune qui en a déjè 10 n'augmente son effectif que de +10%, ces 10% ne modifient pas vraiment son
état. S'il était mauvais il devient juste un peu moins mauvais. Pour améliorer réellement son
état il faudrait lui attribuer plusieurs sièges.

* La méthode AVANT peut don faire que l’attribution d’un siège à une commune la fasse passer d’une
mauvaise à une forte représentativité en sièges ; parfois trop forte car créant un écart de plus de
20% avec la représentativité en population.

* La méthode APRES n’a pas le problème décrit ci-dessus mais elle peut tout attribuer à une commune
fortement sous représenté, typiquement à cause de la limitation à la moitié des sièges (cas de Toulouse).
Ce phénomène déclenche un autre problème qui est le dépassement en siège au conseil métropolitain
du nombre d’élus du conseil municipal.
*/

QList<InfoCommune*> MainWindow::getParEcart(Modes mode) {
    // calcule les écarts de représentativité entre population et sièges avant et après attribution d'un siège
    // Voir note mathématique dans InfoCommune.h
    //   avant ==> ts/sm          (représentativité actuelle)
    //   après ==> (ts+1)/(sm+1)  (représentativité après attribution du siège; même méthode que la plus forte moyenne)
    // le parametre 'mode' permet de choisir sur quel écart est basé l'index qui est retourné
    for (auto* const commune : std::as_const(parPopulation)) {
        commune->moyenne = commune->population / commune->totalSieges(); // hab/siège
        commune->rp      = (100.0 * commune->population) / popMetropole; // % représentation pop (invariant)
        // Avant attribution
        commune->rs_av    = (100.0 * commune->totalSieges()) / siegesMetropole; // % représentation en sièges avant
        commune->ecart_av = 100.0 * (commune->rs_av / commune->rp - 1.0);       // % ecart entre rs_av et rp
        // Apres attribution
        commune->rs_ap
            = (100.0 * (commune->totalSieges() + 1)) / (siegesMetropole + 1); // % représentation en sièges apres
        commune->ecart_ap = 100.0 * (commune->rs_ap / commune->rp - 1.0);     // % ecart entre rs_ap et rp
    }
    // Construire un index croissant des écarts avant ou des écarts après
    auto parEcart = parPopulation;
    if (mode == AVANT) {
        std::sort(parEcart.begin(), parEcart.end(), [](const auto lhs, const auto rhs) {
            return lhs->ecart_av < rhs->ecart_av; // ordre croissant -9 -> +9
        });
    } else {
        std::sort(parEcart.begin(), parEcart.end(), [](const auto lhs, const auto rhs) {
            return lhs->ecart_ap < rhs->ecart_ap; // ordre croissant -9 -> +9
        });
    }
    return parEcart;
}

void MainWindow::step_Options() {
#if 0
    auto te = ui->textEdit_Options;
    te->clear();
#else
    auto* const te = getNewTextEdit(ui->tab_Options);
#endif

    QTextCursor cursor(te->textCursor());
    const int   siegesOptionnels = siegesMetropole / 10; // Nombre maximum de sièges optionnels
    const char* p;

    p = "<p style=\"font-size:12pt;\">"
        "Les communes <i>peuvent</i> créer et répartir un nombre de sièges supplémentaires "
        "inférieur ou égal à 10% du nombre total de sièges de plein droit. La loi n'impose "
        "aucun mode calcul à condition que l'ensemble des sièges attribués à chaque commune ne "
        "s'écarte pas de plus de 20% de la proportion de sa population dans la métropole. "
        "Il existe deux dérogations :<br/>"
        "1. Lorsque l'attribution d'un siège réduit l'écart sans pour autant le faire passer sous "
        "la barre des 20%.<br/>"
        "2. Lorsqu'un <u>second</u> siège serait attribué à une commune ayant bénéficié d'un seul "
        "siège à l'étape de la proportionnelle à la plus forte moyenne. (Onglets C.1 et C.2)"
        "<br/><br/></p>";
    cursor.insertHtml(p);

    p = "<h3>Les %1 sièges de plein droit déja attibués donnent la possibilité de créer au "
        "maximun %2 sièges.</h3><br/>";
    cursor.insertHtml(QString(p).arg(siegesMetropole).arg(siegesOptionnels));

    //---- recherche des exceptions possibles
    int cntK2 = 0;
    for (auto* const commune : std::as_const(parPopulation)) {
        if ((commune->totalSieges() == 1) && (commune->sieges1 + commune->sieges2) == 1) {
            commune->k2 = true;
            cntK2++;
        }
    }
    if (cntK2 == 0) {
        p = "<h3>Aucune commune ne peut bénificier d'un second siège au titre de la dérogation N°2</h3><br/>";
        cursor.insertHtml(p);
    } else {
        p = "<p><h2>Liste des %1 communes pouvant bénéficier d'un second siège au titre de la "
            "dérogation N°2<br/>(classées par population décroissante)</h2><br>";
        cursor.insertHtml(QString(p).arg(cntK2));

        QTextTable* table = cursor.insertTable(cntK2 + 1, 3);
        table->setFormat(tblFormat);

        setCell(table, 0, 0, "Communes", boldCharFormat);
        setCell(table, 0, 1, "    Populations ↓", boldCharFormat);

        int line = 0;
        for (auto* const commune : std::as_const(parPopulation)) {
            if (commune->k2) {
                line++;
                setCell(table, line, 0, commune->nom + "  *");
                setCell(table, line, 1, commune->population);
            }
        }

        cursor.movePosition(QTextCursor::End);
        cursor.insertHtml("<br/><br/>");

        p = "<p style=\"font-size:12pt;\"> ATTENTION : D'après le considérant N°10 de la décision du conseil "
            "constitutionnel <a href=\"https://www.conseil-constitutionnel.fr/decision/2015/2015711DC.htm\">"
            "2015-711 DC</a> du 5 mars 2015, attribuer un siège à une commune de cette liste oblige à en "
            "faire de même pour toutes les autres communes classées avant elle dans cette liste.<br/></p><br/>";
        cursor.insertHtml(p);
    }

    p = "<p style=\"font-size:12pt;\">"
        "NOTE : Chaque création d'un nouveau siège a pour effet de :<br/>"
        "- augmenter la taille du conseil métropolitain,<br/>"
        "- augmenter le seuil de majorité,<br/>"
        "- augmenter la représentativité en sièges de la commune à qui le siège est attribué,<br/>"
        "- diminuer la représentativité en sièges de <u>toutes</u> les autres communes à cause de "
        "l'augmentation du nombre de conseillers métropolitains.<br/>"
        "<br/>"
        "<b>Les attributions de sièges ci-dessous sont optionnelles "
        "et basées sur une logique purement arithmétique, conforme à la loi et son esprit, "
        "mais d’autres approches sont possibles.</b><br/>"
        "<br/>"
        "Ci-dessous les communes sont classées d'après leurs écarts "
        "de représentativité entre sièges et population :<br/>"
        "- un écart négatif indique une sous représentativité en siéges de la commune.<br/>"
        "- un écart positif indique une sur représentativité en sièges de la commune.<br/>"
        "</p><br/>";
    cursor.insertHtml(p);

    const Modes mode    = ui->comboBox_ModeCalcul->currentIndex() == 0 ? AVANT : APRES;
    const auto* modeStr = mode == AVANT ? "AVANT" : "APRES";

    if (mode == APRES && redist > 0) {
        p = "<p style=\"color:red; font-size:12pt;\">"
            "ATTENTION: Suite à la réaffectation de %1 sièges effectuée à l'étape précédente, "
            "le mode 'APRES attribution' risque de donner des résultats non conformes à l'esprit "
            "de la loi dans son affectation des %2 nouveaux siège.<br/></p><br/>";
        cursor.insertHtml(QString(p).arg(redist).arg(siegesOptionnels));
    }

    p = "<h1>Attribution des %1 sièges optionnels sur la base de l'écart de représentativité %2 l'attribution<h1>";
    cursor.insertHtml(QString(p).arg(siegesOptionnels).arg(modeStr));

    p = "<p style=\"font-size:12pt;\">"
        "<b>Etat initial :</b> Nombre total de sièges = %1;  Majorité absolue = %2 voix.<br/></p>";
    cursor.insertHtml(QString(p).arg(siegesMetropole).arg(majority()));

    //---- Simulation arithmetique d'attribution de tous les sièges optionnels

    QMap<QString, InfoCommune*> attributions;

    for (int siege = 1; siege <= siegesOptionnels; siege++) {

        // Calcule des écarts (avant et après)
        // recupère la liste triée, d'après les écarts avant ou après, suivant le mode
        auto parEcart = getParEcart(mode);

        QTextTable* table = cursor.insertTable(nbrCommunes + 1, 7);
        table->setFormat(tblFormat);

        // Header
        setCell(table, 0, 0, "Communes", boldCharFormat);
        setCell(table, 0, 1, "    Populations", boldCharFormat);
        setCell(table, 0, 2, "    (% Pop)", boldCharFormat);
        setCell(table, 0, 3, "    Sièges\navant", boldCharFormat);
        setCell(table, 0, 4, "    (% Sièges)", boldCharFormat);
        setCell(table, 0, 5, "          Ecarts\navant", (mode == AVANT ? boldCharFormat : defaultCharFormat));
        setCell(table, 0, 6, "          Ecarts\naprès", (mode == APRES ? boldCharFormat : defaultCharFormat));

        int line = 0;
        for (const auto* const commune : std::as_const(parEcart)) {
            line++;
            QString tag = commune->k2 ? " *" : "  ";
            setCell(table, line, 0, commune->nom + tag);
            setCell(table, line, 1, commune->population);
            setCell(table, line, 2, commune->rp);
            setCell(table, line, 3, commune->totalSieges());
            setCell(table, line, 4, commune->rs_av);
            setCell(table, line, 5, commune->ecart_av);
            setCell(table, line, 6, commune->ecart_ap);
        }

        cursor.movePosition(QTextCursor::End);
        cursor.insertHtml("<br/><br/>");

        // CREATION effective du siège et calcul
        auto* winner              = parEcart[0];
        attributions[winner->nom] = winner;
        winner->sieges5++;
        siegesMetropole++;

        p = "<p style=\"font-size:12pt;\">"
            "&nbsp; &nbsp; &rArr; Le siège optionnel N° %1 est attribué à <b>%2</b> "
            "pour son écart de %3 % %4 attribution.<br/></p>";
        cursor.insertHtml(QString(p)
                              .arg(siege)
                              .arg(winner->nom)
                              .arg((mode == AVANT ? winner->ecart_av : winner->ecart_ap), 0, 'f', 2)
                              .arg(modeStr));

        const auto* when = siege < siegesOptionnels ? "Nouvel état" : "Etat final";

        p = "<br/><hr width=\"50%\">"
            "<p style=\"font-size:12pt;\">"
            "<b>%1 : </b> Nombre total de sièges = %2;  Majorité absolue = %3 voix.<br/></p>";
        cursor.insertHtml(QString(p).arg(when).arg(siegesMetropole).arg(majority()));
    }

    siegesMax = siegesMetropole;

    // Si le mode courant est APRES les valeur sont dans xxx_ap
    // et si le mode courant est AVANT les valeurs courante sont dans xxx_av
    // On refait le calcul pour que l'etat final soit dasn xxx_av
    auto parEcart = getParEcart(AVANT);

    // Affiche l'etat final
    QTextTable* table = cursor.insertTable(nbrCommunes + 1, 6);
    table->setFormat(tblFormat);

    // Header
    setCell(table, 0, 0, "Communes",                   boldCharFormat);
    setCell(table, 0, 1, "    Populations",            boldCharFormat);
    setCell(table, 0, 2, "    (% Pop)",                boldCharFormat);
    setCell(table, 0, 3, "    Sièges\nfinal",          boldCharFormat);
    setCell(table, 0, 4, "    (% Sièges)",             boldCharFormat);
    setCell(table, 0, 5, "          Ecarts ↑\nfinaux", boldCharFormat);

    int line = 0;
    for (const auto* const commune : std::as_const(parEcart)) {
        line++;
        QString tag = commune->k2 ? " *" : "  ";
        setCell(table, line, 0, commune->nom + tag);
        setCell(table, line, 1, commune->population);
        setCell(table, line, 2, commune->rp);
        setCell(table, line, 3, commune->totalSieges());
        setCell(table, line, 4, commune->rs_av);
        setCell(table, line, 5, commune->ecart_av);
    }

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br/>");

    //---- vérifier si la recommandation du Conseil Constitutionnel est respectée.
    // Dans la liste des communes bénéficiaires de l'exception N°2 on ne doit pas trouver de
    // commune n'ayant pas bénéficiées d'un second sièges suivie d'une commune en ayant bénéficié.
    int                cnt        = 0;
    const InfoCommune* communeRef = nullptr;
    for (auto* const commune : std::as_const(parPopulation)) {
        if (!commune->k2)
            continue;
        if ((communeRef == nullptr) && (commune->sieges5 == 0)) {
            // Premiere commune (donc la plus grosse) du groupe K2 SANS sieges5
            // a partir de là on ne doit pas trouver de commune du groupe K2 AVEC sieges5
            communeRef = commune;
        }
        if ((communeRef != nullptr) && (commune->sieges5 > 0)) {
            // Commune du groupe K2 AVEC sieges5 a tord car
            // elle apparait après (donc plus petite) que communeRef qui elle est SANS sieges5
            cnt++;
            p = "<p style=\"font-size:12pt;\">"
                "ATTENTION! %1 (population %2) vient de recevoir un second siège "
                "alors que %3 (population %4) n'en a pas reçu, bien que plus peuplée.";
            cursor.insertHtml(QString(p)
                                  .arg(commune->nom)
                                  .arg(commune->population)
                                  .arg(communeRef->nom)
                                  .arg(communeRef->population));
        }
    }
    if (cnt == 0) {
        cursor.insertHtml("<h1>OK</h1><br/>");
    }

    //---- Resumé des attributions

    p = "<h1>RESUMÉ de l'attribution à %1 communes des %2 sièges optionnels créés :</h1><br/>"
        "<p style=\"font-size:12pt;\">"
        "Avec attribution des sièges sur la base de l'écart de représentativité %3 l'attribution.</p><br/>";
    cursor.insertHtml(QString(p)
                          .arg(attributions.size())
                          .arg(siegesOptionnels)
                          .arg(ui->comboBox_ModeCalcul->currentIndex() == 0 ? "AVANT" : "APRES"));

    table = cursor.insertTable(attributions.size() + 1, 6);
    table->setFormat(tblFormat);

    // Header
    setCell(table, 0, 0, "Communes ↑", boldCharFormat);
    setCell(table, 0, 1, "    Populations", boldCharFormat);
    setCell(table, 0, 2, "    Sièges\nattribués", boldCharFormat);
    setCell(table, 0, 3, "    Sièges\nfinal", boldCharFormat);
    setCell(table, 0, 4, "      Ecart\nfinal", boldCharFormat);
    setCell(table, 0, 5, "    Valide", boldCharFormat);

    line = 0;
    for (const auto* const commune : std::as_const(attributions)) {
        line++;
        const char* status = nullptr;
        if (commune->ecart_av <= 20.0) {
            status = "OUI";
        } else if (commune->k2) {
            status = "oui";  // Oui par dérogation
        } else {
            status = "NON !";
        }
        QString tag = commune->k2 ? " *" : "  ";
        setCell(table, line, 0, commune->nom + tag);
        setCell(table, line, 1, commune->population);
        setCell(table, line, 2, commune->sieges5);
        setCell(table, line, 3, commune->totalSieges());
        setCell(table, line, 4, commune->ecart_av);
        setCell(table, line, 5, status);
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<br/><br/>");

    //---- Mettre le curseur en haut de page
    cursor.movePosition(QTextCursor::Start);
    te->setTextCursor(cursor);
}
