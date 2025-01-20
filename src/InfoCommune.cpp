// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "InfoCommune.h"

// clang-format off
InfoCommune::InfoCommune(QString nom, int population)
    : nom(nom)
    , population(population)
{
    sieges1 = sieges2 = sieges3 = sieges4 = sieges5 = 0;
    // Nombre de conseillés municipaux (Article L2121-2)
    if(      population <     100) cm =  7; // parfois 5
    else if( population <     500) cm = 11; // parfois 9
    else if( population <   1'500) cm = 15;
    else if( population <   2'500) cm = 19;
    else if( population <   3'500) cm = 23;
    else if( population <   5'000) cm = 27;
    else if( population <  10.000) cm = 29;
    else if( population <  20'000) cm = 33;
    else if( population <  30'000) cm = 35;
    else if( population <  40'000) cm = 39;
    else if( population <  50'000) cm = 43;
    else if( population <  60'000) cm = 45;
    else if( population <  80'000) cm = 49;
    else if( population < 100'000) cm = 53;
    else if( population < 150'000) cm = 55;
    else if( population < 200'000) cm = 59;
    else if( population < 250'000) cm = 61;
    else if( population < 300'000) cm = 65;
    else                           cm = 69;
}
// clang-format on
