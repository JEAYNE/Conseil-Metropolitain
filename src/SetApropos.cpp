// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#undef main
#include "version.h"

void MainWindow::SetApropos() {
    ui->label_prj->setText(PROJECT_NAME);
    ui->label_vers->setText(APP_RELEASE);
    ui->label_git->setText(STRINGIFY(GIT_HEAD) "@" STRINGIFY(GIT_BRANCH) " " STRINGIFY(GIT_STATUS));
    auto txt = QString("%1/%2/%3").arg(COMPILE_DAY).arg(COMPILE_MONTH).arg(COMPILE_YYYY);
    ui->label_compil->setText(txt);
    ui->label_env->setText(COMPILE_ENV);
}
