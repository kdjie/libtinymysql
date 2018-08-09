//============================================================================
// Name        : MysqlPool.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#include "MysqlPool.h"
#include "MysqlConn.h"

using namespace std;
using namespace mysqlpp;
using namespace tinymysql;

CMysqlConnPool::CMysqlConnPool()
{
}
CMysqlConnPool::~CMysqlConnPool()
{
}

// 设置句柄参数
void CMysqlConnPool::setHandleParam(const std::string& strHandleName, const SHandleParam& stParam)
{
    m_mapHandleParam[strHandleName] = stParam;
}

// 获取句柄参数
const SHandleParam& CMysqlConnPool::getHandleParam(const std::string& strHandleName)
{
    MAP_HANDLEPARAM_t::iterator c_iter = m_mapHandleParam.find(strHandleName);
    if (c_iter != m_mapHandleParam.end())
        return c_iter->second;

    return m_stParamNull;
}

// 获取连接对象
IMysqlConn* CMysqlConnPool::getConn(const std::string& strHandleName)
{
    const SHandleParam& stParam = getHandleParam(strHandleName);
	if (!stParam.isValid())
	{
		getLogReport()->log(Error, "[CMysqlConnPool::getConn] invalid handle name:[%s]", strHandleName.c_str());
		return NULL;
	}

    // 从线程局部存储中获取
	MAP_HANDLEPCONN_t* pMapHandlePConn = __getMapHandlePConn();
	MAP_HANDLEPCONN_t::iterator iter = pMapHandlePConn->find(strHandleName);
	if (iter != pMapHandlePConn->end())
	    return iter->second;

    // 取不到，新建
	CMysqlConn* pNew = new CMysqlConn(strHandleName, this);
	pNew->setLogReport(getLogReport());
    if (!pNew->connect())
    {
        delete pNew;
        return NULL;
    }

    // 插入到线程局部存储中
    pMapHandlePConn->insert( std::make_pair(strHandleName, pNew) );
    return pNew;
}

// 回收连接对象
void CMysqlConnPool::gcConn(IMysqlConn* pConn)
{
    // nothing
}

// 销毁连接对象
void CMysqlConnPool::destroyConn(IMysqlConn* pConn)
{
    // 从线程局部存储中清理
    MAP_HANDLEPCONN_t* pMapHandlePConn = __getMapHandlePConn();
    pMapHandlePConn->erase( pConn->getHandleName() );

    // 释放连接
    delete pConn;
}

// 获取当前线程的连接对象表
CMysqlConnPool::MAP_HANDLEPCONN_t* CMysqlConnPool::__getMapHandlePConn()
{
    MAP_HANDLEPCONN_t* pMapHandlePConn = m_tsdMapHandlePConn.get();
    if (pMapHandlePConn == NULL)
    {
        pMapHandlePConn = new MAP_HANDLEPCONN_t;
        m_tsdMapHandlePConn.reset(pMapHandlePConn);
    }

    return pMapHandlePConn;
}
