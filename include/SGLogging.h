//
//  SGLogging.h
//
//  Copyright 2014 ON Semiconductor.
//  All rights reserved. This software and/or documentation is licensed by ON Semiconductor under
//  limited terms and conditions. The terms and conditions pertaining to the software and/or documentation are available at
//  http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf (“ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software”).
//  Do not use this software and/or documentation unless you have carefully read and you agree to the limited terms and conditions.
//  By using this software and/or documentation, you agree to the limited terms and conditions.
//
//  Copyright 2019 ON Semiconductor
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef SGLOGGING_H
#define SGLOGGING_H
#include <litecore/c4.h>
#include <litecore/Logging.hh>

#define Q_DECLARE_C4_LOGGING_CATEGORY(name) \
    extern C4LogDomain &name();

#define Q_C4_LOGGING_CATEGORY(name, ...) \
    C4LogDomain &name() \
    { \
        static litecore::LogDomain logdomain(__VA_ARGS__, litecore::LogLevel::Debug); \
        static C4LogDomain c4logdomain = (C4LogDomain)&logdomain; \
        return c4logdomain; \
    }

#ifdef SHOW_DATABASE_MESSAGES

#if !defined(QT_NO_DEBUG_OUTPUT)
#  define qC4Debug(category, FMT, ...) C4LogToAt(category(), kC4LogDebug,   FMT, ## __VA_ARGS__)
#else
#  define qC4Debug(category, FMT, ...)
#endif

#if !defined(QT_NO_INFO_OUTPUT)
#  define qC4Info(category, FMT, ...) C4LogToAt(category(), kC4LogInfo,   FMT, ## __VA_ARGS__)
#else
#  define qC4Info(category, FMT, ...)
#endif

#if !defined(QT_NO_WARNING_OUTPUT)
#  define qC4Warning(category, FMT, ...) C4LogToAt(category(), kC4LogWarning,   FMT, ## __VA_ARGS__)
#else
#  define qC4Warning(category, FMT, ...)
#endif

#define qC4Critical(category, FMT, ...) C4LogToAt(category(), kC4LogError,   FMT, ## __VA_ARGS__)

#else

#  define qC4Debug(category, FMT, ...)
#  define qC4Info(category, FMT, ...)
#  define qC4Warning(category, FMT, ...)
#  define qC4Critical(category, FMT, ...)

#endif

#endif //SGLOGGING_H
