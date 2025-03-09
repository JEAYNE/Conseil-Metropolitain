// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "LoadCommunes.h"
#include "InfoCommune.h"

#include <QFile>
#include <QString>
#include <QTextStream>

// Analyse une ligne au format CSV
// Gère les champs délimités par " (utilisé quand le champ contient le délimiteur)
// Ne gère pas les "" à l'interieur des "
QList<QString> parseCSVLine(const QString& line, const char delim) {
    QList<QString> fields;
    QString        field;

    bool inQuotes = false;
    for (auto c : line) {
        if (!inQuotes && (c == delim)) {
            field = field.trimmed();
            fields.push_back(field);
            field.clear();
        } else if (c == '"') {
            inQuotes = !inQuotes;
        } else {
            field += c;
        }
    }

    field = field.trimmed();
    fields.push_back(field);

    return fields;
}

// Charge un fichier CSV contenant la liste des communes et leurs population
// En cas d'erreur le premier enregistrement de la liste a une population négative
// et un message d'erreur à la place du nom de commune.
Communes loadCommunes(const QString& filename, const char delim) {
    Communes    data;
    const char* p;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        p = "Ne peut pas ouvrir le fichier %1\nERREUR %2: %3";
        data.push_front(InfoCommune(QString(p).arg(filename).arg(file.error()).arg(file.errorString())));
        return data;
    }
    QTextStream stream(&file);

    int lineCount = 0;
    while (!stream.atEnd()) {
        QString     line = stream.readLine();
        lineCount++;

        // Ignore les lignes vides ou commençant par #
        line = line.simplified();
        if (line.length() == 0)
            continue;
        if (line.at(0) == '#')
            continue;

        // Décodage de la ligne CSV avec deux champs
        const auto fields = parseCSVLine(line, delim);
        if (fields.size() != 2) {
            p = "ligne %1: Nombre de champs invalide. 2 attendus mais %2 trouvés.";
            data.push_front(InfoCommune(QString(p).arg(lineCount).arg(fields.size())));
            return data;
        }

        // Vérification du premier champ: le nom
        if (fields[0].length() == 0) {
            p = "ligne %1: Le premier champ 'Nom' est vide.";
            data.push_front(InfoCommune(QString(p).arg(lineCount)));
            return data;
        }
        const auto& nom = fields[0];
        for (const auto& item : std::as_const(data)) {
            if (nom == item.nom) {
                p = "ligne %1: Ce nom de commune est dupliqué.";
                data.push_front(InfoCommune(QString(p).arg(lineCount)));
                return data;
            }
        }

        // Vérification du deuxième champ: la population
        if (fields[1].length() == 0) {
            p = "ligne %1: Le second champ 'Population' est vide.";
            data.push_front(InfoCommune(QString(p).arg(lineCount)));
            return data;
        }
        bool       ok;
        const auto population = fields[1].toInt(&ok);
        if ((!ok) || (population < 50) || (population > 3'000'000)) {
            p = "ligne %1: Le second champ 'Population' n'a pas une valeur valide.";
            data.push_front(InfoCommune(QString(p).arg(lineCount)));
            return data;
        }

        // ok, ligne valide ==> création d'une InfoCommune
        data.push_back(InfoCommune(nom, population));
    }

    file.close();
    return data;
}
