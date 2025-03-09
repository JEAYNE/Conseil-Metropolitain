// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QLayout>
#include <QPdfWriter>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextEdit>

static QTextEdit *getTextEdit(QWidget *tab) {
    // le TextEdit étant crée dynamiquement on ne peut pas compter sur ui->myTextEdit->xx
    // voir explication dans editTools.cpp
    // Localiser dans l'onglet 'tab' le TextEdit à imprimer (dernier child du layout)
    auto *layout   = tab->layout();
    auto *lastItem = layout->itemAt(layout->count() - 1);
    return qobject_cast<QTextEdit *>(lastItem->widget()); // nullPtr si mauvais layout
}

//
// Print
//

static void printThisTab(QWidget *tab) {
    auto *const te = getTextEdit(tab);
    if (te == nullptr)
        return; // BUG: Le layout n'est pas bon :-(

    /////
    // #include <QPrintPreviewDialog>
    // QPrintPreviewDialog prev;
    // prev.exec();
    /////

    // Ouvrir la boite de dialogue de selection et configuration de l'imprimante
    QPrinter printer;
    printer.setPageSize(QPageSize::A4);                 // pas d'effet :-(
    printer.setPageOrientation(QPageLayout::Landscape); // pas d'effet :-(
    printer.setColorMode(QPrinter::GrayScale);          // pas d'effet :-(

    QPrintDialog dialog(&printer);                  // ,this)
    dialog.setWindowTitle("Conseil Métropolitain"); // pas d'effet :-(

    if (te->textCursor().hasSelection()) {
        dialog.setOptions(QAbstractPrintDialog::PrintSelection);
        dialog.setPrintRange(QAbstractPrintDialog::Selection);
    }

    // Si ok imprimer.
    if (dialog.exec() == QDialog::Accepted) {
        te->print(&printer);
    }
}

static void saveThisTab2pdf(QWidget *tab, const QString &fileName) {
    auto *const te = getTextEdit(tab);
    if (te == nullptr)
        return; // BUG: Le layout n'est pas bon :-(

    static QString saveToPath;
    if (saveToPath.size() == 0) {
        saveToPath = QDir::homePath();
    }

    QString pathName = saveToPath + "/" + nomMetropole + "_" + fileName;
    pathName         = QFileDialog::getSaveFileName(nullptr, "Fichier PDF à créer", pathName, "*.pdf");
    QPdfWriter pdfWriter(pathName);
    pdfWriter.setPageSize(QPageSize::A3);                // A4/A3 ok
    pdfWriter.setPageOrientation(QPageLayout::Portrait); // Portrait/Landscape ok
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));     // MAIS il y a toujours des marges !

    // Qt ne permet pas de ne prendre en compte que le texte selectionné.

    te->print(&pdfWriter);

    QFileInfo fi(pathName);
    saveToPath = fi.absolutePath();
}

void MainWindow::on_pushButton_DataPrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Data);
}

void MainWindow::on_pushButton_ProportionnellePrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Proportionnelle);
}

void MainWindow::on_pushButton_MoyennePrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Moyenne);
}

void MainWindow::on_pushButton_BonusPrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Bonus);
}

void MainWindow::on_pushButton_LimitationPrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Limitation);
}

void MainWindow::on_pushButton_OptionsPrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Options);
}

void MainWindow::on_pushButton_SummaryPrint_clicked() {
    if (canPrint)
        printThisTab(ui->tab_Summary);
}

//
// Save to PDF
//

void MainWindow::on_pushButton_DataPdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Data, "Données");
}

void MainWindow::on_pushButton_ProportionnellePdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Proportionnelle, "Proportionnelle-Simple");
}

void MainWindow::on_pushButton_MoyennePdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Moyenne, "Plus-Forte-Moyenne");
}

void MainWindow::on_pushButton_BonusPdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Bonus, "Sièges-d'Office");
}

void MainWindow::on_pushButton_LimitationPdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Limitation, "Limitation");
}

void MainWindow::on_pushButton_OptionsPdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Options, "Sièges-Optionnels");
}

void MainWindow::on_pushButton_SummaryPdf_clicked() {
    if (canPrint)
        saveThisTab2pdf(ui->tab_Summary, "Résumé");
}
