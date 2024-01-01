// Copyright (C) 2023 owoDra

#pragma once

#include "Logging/LogMacros.h"

GACORE_API DECLARE_LOG_CATEGORY_EXTERN(LogGAC, Log, All);

#if !UE_BUILD_SHIPPING

#define GACLOG(FormattedText, ...) UE_LOG(LogGAC, Log, FormattedText, __VA_ARGS__)

#define GACENSURE(InExpression) ensure(InExpression)
#define GACENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GACENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GACCHECK(InExpression) check(InExpression)
#define GACCHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GACLOG(FormattedText, ...)

#define GACENSURE(InExpression) InExpression
#define GACENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GACENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GACCHECK(InExpression) InExpression
#define GACCHECK_MSG(InExpression, InFormat, ...) InExpression

#endif