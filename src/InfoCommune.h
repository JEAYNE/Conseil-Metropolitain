// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#ifndef INFOCOMMUNE_H
#define INFOCOMMUNE_H

#include <QString>

struct InfoCommune {
    explicit InfoCommune(QString nom, int population = -1);
    QString nom;
    int population;
    int sieges1;    // proportionelle simple; onglet C1
    int sieges2;    // plus forte moyenne; onglet C2
    int sieges3;    // d'office; onglet C3
    int sieges4;    // réattribution; onglet C4
    int sieges5;    // optionnels; onglet C5
    [[nodiscard]] int totalSieges() const { return sieges1 + sieges2 + sieges3 + sieges4 + sieges5; }
    int cm;         // nombre conseillés municipaux (D'apres Article L2121-2)
    // calculs internes
    double moyenne{}; // population/totalSieges()               (habitant par sièges)
    double rp{};      // 100 * population/populationMetropole   (Ne varie pas)
    // avant
    double rs_av{};    // 100 * totalSieges()/siegesMetropole
    double ecart_av{}; // 100 * (1 - rp/rs_av)
    // apres
    double rs_ap{};    // 100 * (totalSieges()+1)/(siegesMetropole+1)
    double ecart_ap{}; // 100 * (1 - rp/rs_ap)

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * La loi est formulée en terme de rp et rs mais il y a une autre manière
     * de calculer l'écart en utilisant la moyenne déja trés utilisée par la loi :
     * rp/rs = (population/populationMetropole) / (totalSieges()/siegesMetropole)
     *       = (population/populationMetropole) * (siegesMetropole/totalSieges())
     *       = (population/totalSieges()) * (siegesMetropole/populationMetropole)
     *       = moyenne / moyenneMetropole
     *  ecart = 100 * (1 - rp/rs) = 100 * ( 1 - moyenne/moyenneMetropole)
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
};

typedef QList<InfoCommune> Communes;

#endif // INFOCOMMUNE_H
