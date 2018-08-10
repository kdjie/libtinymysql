//============================================================================
// Name        : MysqlPoolTLS.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include "IMysql.h"
#include "Logger.h"
#include "Specific.h"
#include <map>

namespace tinymysql
{
    class CMysqlConnPoolTLS
        : public IMysqlConnPool
        , public ILogReportAware
    {
    protected:
        // 句柄参数列表
        typedef std::map<std::string, SHandleParam> MAP_HANDLEPARAM_t;
        MAP_HANDLEPARAM_t m_mapHandleParam;
        SHandleParam m_stParamNull;

        // 线程局部存储句柄连接
        typedef std::map<std::string, IMysqlConn*> MAP_HANDLEPCONN_t;
        TTSD<MAP_HANDLEPCONN_t> m_tsdMapHandlePConn;

    public:
        CMysqlConnPoolTLS() {}
        virtual ~CMysqlConnPoolTLS() {}

        // 设置句柄参数
        void setHandleParam(const std::string& strHandleName, const SHandleParam& stParam);
        // 获取句柄参数
        virtual const SHandleParam& getHandleParam(const std::string& strHandleName);

        // 获取连接对象
        virtual IMysqlConn* getConn(const std::string& strHandleName);
        // 回收连接对象
        virtual void gcConn(IMysqlConn* pConn);
        // 销毁连接对象
        virtual void destroyConn(IMysqlConn* pConn);

    protected:
        // 获取当前线程的连接对象表
        MAP_HANDLEPCONN_t* __getMapHandlePConn();
    };
}
