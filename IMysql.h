//============================================================================
// Name        : IMysql.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <stdint.h>
#include <string>
#include <mysql++/mysql++.h>

namespace tinymysql
{
    // 句柄参数定义
    struct SHandleParam
    {
        std::string strHost;
        std::string strUser;
        std::string strPass;
        uint16_t uPort16;
        std::string strDB;
        std::string strCharSet;

        SHandleParam()
        {
            setParam("127.0.0.1", "", "", 3306, "", "utf8");
        }

        void setParam(const std::string& _strHost, const std::string& _strUser, const std::string& _strPass,
            uint16_t _uPort16, const std::string& _strDB, const std::string& _strCharSet = "utf8")
        {
            strHost = _strHost;
            strUser = _strUser;
            strPass = _strPass;
            uPort16 = _uPort16;
            strDB = _strDB;
            strCharSet = _strCharSet;
        }

        bool isValid() const
        {
            return (strHost != "" && strUser != "" && strDB != "" && uPort16 != 0 && strDB != "");
        }
    };

    // MYSQL操作回调
    struct IQueryCallBack
    {
        virtual ~IQueryCallBack() {}

        virtual void setPreview(bool b) = 0;
        virtual bool isPreview() = 0;
        virtual void onUseResult(const mysqlpp::UseQueryResult& result, void* pArg) = 0;
        virtual void onStoreResult(const mysqlpp::StoreQueryResult& result, void* pArg) = 0;
        virtual void onSimpleResult(const mysqlpp::SimpleResult& result, void* pArg) = 0;
        virtual void onException(const mysqlpp::BadQuery& bad) = 0;
        virtual void onPreview(const std::string& strSQL) = 0;
    };

    struct IAbsQueryCallBack
        : public IQueryCallBack
    {
    protected:
        bool m_bPreview;

    public:
        IAbsQueryCallBack() : m_bPreview(false) {}

        virtual void setPreview(bool b) { m_bPreview = b; }
        virtual bool isPreview() { return m_bPreview; }
        virtual void onUseResult(const mysqlpp::UseQueryResult& result, void* pArg) {}
        virtual void onStoreResult(const mysqlpp::StoreQueryResult& result, void* pArg) {}
        virtual void onSimpleResult(const mysqlpp::SimpleResult& result, void* pArg) {}
        virtual void onException(const mysqlpp::BadQuery& bad) {}
        virtual void onPreview(const std::string& strSQL) {}
    };

    // MYSQL操作接口
    class IMysqlDAO
    {
    public:
        virtual ~IMysqlDAO() {}

        virtual bool storeQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...) = 0;
        virtual bool useQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...) = 0;
        virtual bool execute(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...) = 0;
    };

    class IMysqlDAOAware
    {
    protected:
        IMysqlDAO* m_pMysqlDAO;

    public:
        IMysqlDAOAware() : m_pMysqlDAO(NULL) {}
        virtual ~IMysqlDAOAware() {}

        void setMysqlDAO(IMysqlDAO* p) { m_pMysqlDAO = p; }
        IMysqlDAO* getMysqlDAO() { return m_pMysqlDAO; }
    };

    // MYSQL连接接口
    class IMysqlConn
    {
    public:
        virtual ~IMysqlConn() {}

        // mysql访问相关
        virtual const std::string& getHandleName() = 0;
        virtual mysqlpp::Connection& conn() = 0;
        virtual mysqlpp::Query query() = 0;

        // 连接管理操作
        virtual bool connect() = 0;
        virtual bool reconect() = 0;
        virtual void gc() = 0;
        virtual void destroy() = 0;
    };

    // MYSQL连接池接口
    class IMysqlConnPool
    {
    public:
        virtual ~IMysqlConnPool() {}

        // 获取句柄参数
        virtual const SHandleParam& getHandleParam(const std::string& strHandleName) = 0;

        // 获取连接对象
        virtual IMysqlConn* getConn(const std::string& strHandleName) = 0;
        // 回收连接对象
        virtual void gcConn(IMysqlConn* pConn) = 0;
        // 销毁连接对象
        virtual void destroyConn(IMysqlConn* pConn) = 0;
    };

    class IMysqlConnPoolAware
    {
    protected:
        IMysqlConnPool* m_pMysqlConnPool;
    public:
        IMysqlConnPoolAware() : m_pMysqlConnPool(NULL) {}
        virtual ~IMysqlConnPoolAware() {}

        void setMysqlConnPool(IMysqlConnPool* p){ m_pMysqlConnPool = p; }
        IMysqlConnPool* getMysqlConnPool() { return m_pMysqlConnPool; }
    };
}
