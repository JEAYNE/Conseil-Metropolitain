// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "editTools.h"
#include "version.h"

#include <QDateTime>
#include <QLayout>
#include <QTextBrowser>

extern QString nomMetropole;

// Si on réinsère des données dans un QTextEdit qui a déjà été affiché c'est extrêmement lent.
// La solution consiste à détruite le TextEdit précédent et à en créer un nouveau et y insérer le nouveau texte.
//
// C'est pourquoi ce bout de code réutilisant à chaque fois le même TextEdit (définit dans le form .ui)
//  | auto te = ui->textEdit_xxxx;
//  | te->clear();
//  | ...
//
// est remplacé par celui-ci qui détruit l'ancien TextEdit et en crée un nouveau à chaque fois :
//  | auto te = getNewTextEdit(ui->tab_Xxxxx);
//
// Dans ce cas la cette hierachie est obligatoire
//   parent                     (Widget parent)
//      +--> layout             (Obligatoire)
//          +--> autreWidget    (Facultatif)
//          +--> textEdit_xxxx  (Si présent doit être le dernier QWidget du layout)

QTextEdit *getNewTextEdit(QWidget *parent) {
    auto *layout = parent->layout();
    auto *lastChild = layout->takeAt(layout->count() - 1); // dernier item du layout
    if (lastChild != nullptr) {
        delete lastChild->widget();
        delete lastChild;
    }
#if 0
        auto te = new QTextEdit();
        layout->addWidget(te);
        te->setReadOnly(true);
        te->setTextInteractionFlags(Qt::TextBrowserInteraction);
#else
    auto *const te = new QTextBrowser(); // permet d'avoir les liens cliquables
    layout->addWidget(te);
    te->setReadOnly(true);
    te->setOpenExternalLinks(true);
#endif

    QTextCursor cursor(te->textCursor());
    cursor.movePosition(QTextCursor::Start);

    // Insere date et heure
    QDateTime dateTime = dateTime.currentDateTime();
    cursor.insertHtml(
        QString("<p align=\"right\">%1 - %2 - %3</p><p align=\"left\"><br/></p>")
            .arg(dateTime.toString("dddd d MMMM yyyy HH:mm:ss"), nomMetropole, APP_RELEASE));
    cursor.movePosition(QTextCursor::End);

    return te; // retourne soit un QTextEdit soit QTextBrowser (qui derive de TextEdit)
}
