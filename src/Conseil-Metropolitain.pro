QT += core gui widgets printsupport

CONFIG += c++17

# fail to compile if code uses deprecated APIs.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    InfoCommune.cpp \
    LoadCommunes.cpp \
    SetApropos.cpp \
    editTools.cpp \
    main.cpp \
    mainwindow.cpp \
    print.cpp \
    stepAfficheListe.cpp \
    stepBonus.cpp \
    stepLimitation.cpp \
    stepMoyenne.cpp \
    stepOptions.cpp \
    stepProportionnelle.cpp \
    stepSummary.cpp

HEADERS += \
    InfoCommune.h \
    LoadCommunes.h \
    editTools.h \
    mainwindow.h \
    version.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../Changelog.md \
    ../README.md \
    ../TODO.md \
    no.clang-format

RC_FILE = version.rc

RESOURCES += \
    resources.qrc
