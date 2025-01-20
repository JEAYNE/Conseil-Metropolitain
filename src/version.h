// Copyright (C) 2025 JEAYNE@GitHub
// SPDX-License-Identifier: GNU General Public License v3.0 only

#ifndef VERSION_H
#define VERSION_H

#define _str(a) #a
#define STRINGIFY(a) _str(a)

#define CAT_STR(A,B) A ## B
#define CAT_MACRO(A,B) CAT_STR(A,B)

#include "build_info.h"     // auto generated

#define ORG_NAME            "JEAYNE@GitHub"
#define APP_DESC            "Représentation au Conseil Métropolitain"
#define APP_NAME            PROJECT_NAME

#define APP_VERSION_MAJOR   2
#define APP_VERSION_MINOR   0

#define APP_REVISION_MAJOR  COMPILE_YY                             //  YY [22,23,24 ...]
#define APP_REVISION_MINOR  CAT_MACRO(COMPILE_MONTH,COMPILE_DAY2)  // MDD [101,102..  1231]

#define APP_VERSION  STRINGIFY(APP_VERSION_MAJOR)  "." STRINGIFY(APP_VERSION_MINOR)
#define APP_REVISION STRINGIFY(APP_REVISION_MAJOR) "." STRINGIFY(APP_REVISION_MINOR)

#define APP_RELEASE  APP_VERSION "." APP_REVISION

#endif // VERSION_H
