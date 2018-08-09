//============================================================================
// Name        : FuncHelper.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <stdio.h>
#include <stdlib.h>

namespace tinymysql
{
    // 帮助函数

    inline bool isNumber(const char* pChar)
    {
        return (*pChar >= '0' && *pChar <= '9');
    }

    inline bool isNumber(const char Ch)
    {
        return (Ch >= '0' && Ch <= '9');
    }

    inline unsigned int getNumLength(unsigned int uNum)
    {
        char szTmp[32] = {0};
        return sprintf(szTmp,"%u", uNum);
    }
}
