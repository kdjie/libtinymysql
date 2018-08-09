//============================================================================
// Name        : MysqlConn.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include "IMysql.h"
#include "Logger.h"

namespace tinymysql
{
    class CMysqlConn
        : public IMysqlConn
        , public ILogReportAware
        , public IMysqlConnPoolAware
    {
    protected:
        std::string m_strHandleName;
        mysqlpp::Connection m_Conn;

    public:
        CMysqlConn(const std::string& strHandleName, IMysqlConnPool* pConnPool);
        virtual ~CMysqlConn();

        // mysql访问相关
        virtual const std::string& getHandleName();
        virtual mysqlpp::Connection& conn();
        virtual mysqlpp::Query query();

        // 连接管理操作
        virtual bool connect();
        virtual bool reconect();
        virtual void gc();
        virtual void destroy();

    protected:
        // 连接MYSQL
        bool __connect();
    };
}
