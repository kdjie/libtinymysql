//============================================================================
// Name        : MysqlConn.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#include "MysqlConn.h"

using namespace std;
using namespace mysqlpp;
using namespace tinymysql;

CMysqlConn::CMysqlConn(const std::string& strHandleName, IMysqlConnPool* pConnPool)
: m_strHandleName(strHandleName)
/*, m_Conn(false)*/
{
    setMysqlConnPool(pConnPool);
}

CMysqlConn::~CMysqlConn()
{
    m_Conn.disconnect();
}

// mysql访问相关

const std::string& CMysqlConn::getHandleName()
{
	return m_strHandleName;
}

mysqlpp::Connection& CMysqlConn::conn()
{
	return m_Conn;
}

mysqlpp::Query CMysqlConn::query()
{
	return m_Conn.query();
}

// 连接管理操作

bool CMysqlConn::connect()
{
	return __connect();
}

bool CMysqlConn::reconect()
{
	m_Conn.disconnect();
	return __connect();
}

void CMysqlConn::gc()
{
	getMysqlConnPool()->gcConn(this);
}

void CMysqlConn::destroy()
{
	getMysqlConnPool()->destroyConn(this);
}

// 连接MYSQL

bool CMysqlConn::__connect()
{
    const SHandleParam& stParam = getMysqlConnPool()->getHandleParam(m_strHandleName);
	if (!stParam.isValid())
	{
	    getLogReport()->log(Error, "[CMysqlConn::__connect] handle name:[%s] param is invalid, connect host:[%s:%u] user:[%s] pass:[***] database:[%s]",
            m_strHandleName.c_str(),
            stParam.strHost.c_str(), stParam.uPort16, stParam.strUser.c_str(), stParam.strDB.c_str());
		return false;
	}

    getLogReport()->log(Info, "[CMysqlConn::__connect] handle name:[%s]", m_strHandleName.c_str());
	getLogReport()->log(Info, "[CMysqlConn::__connect] set names %s", stParam.strCharSet.c_str());
	getLogReport()->log(Info, "[CMysqlConn::__connect] set reconnect true");
	getLogReport()->log(Info, "[CMysqlConn::__connect] connect host:[%s:%u] user:[%s] pass:[***] database:[%s]",
		stParam.strHost.c_str(), stParam.uPort16, stParam.strUser.c_str(), stParam.strDB.c_str());

	try 
	{
		m_Conn.set_option(new mysqlpp::SetCharsetNameOption(stParam.strCharSet.c_str()));
		m_Conn.set_option(new mysqlpp::ReconnectOption(true));

		m_Conn.connect(stParam.strDB.c_str(), stParam.strHost.c_str(), stParam.strUser.c_str(), stParam.strPass.c_str(), stParam.uPort16);
	}
	catch (const mysqlpp::ConnectionFailed& cf)
	{
		getLogReport()->log(Error, "[CMysqlConn::__connect] handle name:[%s] connect failed:[%s]", m_strHandleName.c_str(), cf.what());
		return false;
	}

	getLogReport()->log(Info, "[CMysqlConn::__connect] handle name:[%s] connect success!", m_strHandleName.c_str());
	return true;
}
