// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#ifndef INFOCOMMUNE_H
#define INFOCOMMUNE_H

#include <QString>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * La partie accord local de la loi est formulée en termes de rs et rp mais il y a une
 * autre manière de calculer l'écart en utilisant la moyenne déja trés utilisée par la loi :
 * rs/rp = (totalSieges()/siegesMetropole)/(population/populationMetropole)
 *       = (totalSieges()/siegesMetropole)*(populationMetropole/population)
 *       = (totalSieges()/population)     *(populationMetropole/siegesMetropole)
 *       = moyenneMetropole / moyenne
 * Ecart = 100 * (rs/rp - 1) = 100 * (moyenneMetropole/moyenne - 1)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

struct InfoCommune {

    explicit InfoCommune(QString nom, int population = -1);

    QString nom;
    int     population;
    int     sieges1; // proportionelle simple; onglet C1
    int     sieges2; // plus forte moyenne; onglet C2
    int     sieges3; // d'office; onglet C3
    int     sieges4; // réattribution; onglet C4
    int     sieges5; // optionnels; onglet C5
    int     cm;      // nombre de conseillers municipaux (D'après Article L2121-2)
    bool    k2{};    // concernée ou pas par l'exception N°2 au 20%

    //-- calculs internes --
    double moyenne{}; // population/totalSieges()               (habitants par siège)
    double rp{};      // 100 * population/populationMetropole   (Ne varie pas)
    // avant
    double rs_av{};    // 100 * totalSieges()/siegesMetropole
    double ecart_av{}; // 100 * (rs_av/rp - 1)
    // apres
    double rs_ap{};    // 100 * (totalSieges()+1)/(siegesMetropole+1)
    double ecart_ap{}; // 100 * (rs_ap/rp - 1)

    [[nodiscard]] int totalSieges() const { return sieges1 + sieges2 + sieges3 + sieges4 + sieges5; }
};

typedef QList<InfoCommune> Communes;

#endif // INFOCOMMUNE_H
