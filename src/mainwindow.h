// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextTable>

#include "InfoCommune.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef QList<InfoCommune*> Index;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    // Les actions de l'onglet de départ
    void on_pushButton_load_csv_clicked();
    void on_comboBox_metropole_activated(int index);

    // les boutons 'print'
    void on_pushButton_DataPrint_clicked();
    void on_pushButton_ProportionnellePrint_clicked();
    void on_pushButton_MoyennePrint_clicked();
    void on_pushButton_BonusPrint_clicked();
    void on_pushButton_LimitationPrint_clicked();
    void on_pushButton_OptionsPrint_clicked();
    void on_pushButton_SummaryPrint_clicked();

    // les boutons 'pdf'
    void on_pushButton_DataPdf_clicked();
    void on_pushButton_ProportionnellePdf_clicked();
    void on_pushButton_MoyennePdf_clicked();
    void on_pushButton_BonusPdf_clicked();
    void on_pushButton_LimitationPdf_clicked();
    void on_pushButton_OptionsPdf_clicked();
    void on_pushButton_SummaryPdf_clicked();

  private:
    enum Modes { AVANT, APRES };

    void SetApropos();
    void processDataFile(const QString& filename, char separateur);
    void buildIndex();
    void buildFormats(QTextEdit* te);
    void step_AfficheListe();
    void step_Proportionnelle();
    void step_Moyenne();
    void step_Bonus();
    void step_Limitation();
    void step_Options();
    void step_Summary();
    void showSummary(QTextCursor cursor, const Index& sortBy);

    Index             getParEcart(Modes mode);
    [[nodiscard]] int majority() const {
        return (siegesMetropole % 2) != 0 ? ((siegesMetropole + 1) / 2) : (siegesMetropole / 2 + 1);
    }

    void setCell(QTextTable*             table,
                 int                     line,
                 int                     col,
                 const QString&          text,
                 const QTextCharFormat&  charFmt = defaultCharFormat,
                 const QTextBlockFormat& blkFmt  = rightBlockFormat);

    void setCell(QTextTable*             table,
                 int                     line,
                 int                     col,
                 int                     val,
                 const QTextCharFormat&  charFmt = defaultCharFormat,
                 const QTextBlockFormat& blkFmt  = rightBlockFormat);

    void setCell(QTextTable*             table,
                 int                     line,
                 int                     col,
                 double                  val,
                 const QTextCharFormat&  charFmt = defaultCharFormat,
                 const QTextBlockFormat& blkFmt  = rightBlockFormat);

    void setHtmlCell(QTextTable*             table,
                     int                     line,
                     int                     col,
                     const QString&          html,
                     const QTextBlockFormat& blkFmt = rightBlockFormat);

    Communes communes;
    Index    parNom;
    Index    parPopulation;
    int      nbrCommunes;
    int      popMetropole;
    int      siegesBase;
    int      siegesDroit;
    int      siegesMax;
    int      siegesMetropole;
    int      resteProportionnelle;
    int      sansSiege;
    int      redist;
    bool     canPrint;

    static QTextCharFormat  defaultCharFormat, boldCharFormat;
    static QTextBlockFormat defaultBlockFormat, rightBlockFormat, leftBlockFormat;
    static QTextTableFormat defaultTableFormat, tblFormat;

    Ui::MainWindow* ui;
};

extern QString nomMetropole;

#endif // MAINWINDOW_H
