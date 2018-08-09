//============================================================================
// Name        : MysqlDAO.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <map>

#include "IMysql.h"
#include "Logger.h"

namespace tinymysql
{
    // Mysql数据访问接口实现

    enum EParamType
    {
        PARAM_INT32 = 1,
        PARAM_INT64 = 2,
        PARAM_STRING = 3,
        PARAM_BINARY = 4,

        PARAM_UNKOWN = -1
    };

    struct SParamValue
    {
        EParamType emType;

        uint32_t uValue32;
        uint64_t uValue64;
        std::string strValue;

        SParamValue(EParamType _emType = PARAM_INT32)
        {
            emType = _emType;

            uValue32 = 0;
            uValue64 = 0;
            strValue = "";
        }
    };

    // SQL参数列表
    typedef std::map<uint32_t, SParamValue> MAP_PARAM_t;

    class CMysqlDAO
        : public IMysqlDAO
        , public ILogReportAware
        , public IMysqlConnPoolAware
    {
    public:
        // 操作接口实现
        virtual bool storeQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...);
        virtual bool useQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...);
        virtual bool execute(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...);

    private:
        // SQL操作类型
        enum ESQLType
        {
            SQL_STORE,
            SQL_USE,
            SQL_EXECUTE
        };

        // 统一SQL入口
        bool __entrySQL(const ESQLType& emSQLType, const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, va_list& va);

        // 解析SQL
        bool __praseSQL(const char* pOriSQL, va_list& va, std::string& strRealSQL, MAP_PARAM_t& mapParam);

        // 执行SQL
        bool __goSQL(const ESQLType& emSQLType, IMysqlConn* pConn, IQueryCallBack* pCb, void* pArg,
            const char* pRealSQL, const MAP_PARAM_t& mapParam);
    };
}
