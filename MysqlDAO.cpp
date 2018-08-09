//============================================================================
// Name        : MysqlDAO.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#include "MysqlDAO.h"
#include "MysqlPool.h"
#include "FuncHelper.h"

using namespace std;
using namespace mysqlpp;
using namespace tinymysql;

// 操作接口实现

bool CMysqlDAO::storeQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...)
{
    va_list va;
    va_start(va, pOriSQL);
    bool bRet = __entrySQL(SQL_STORE, strHandleName, pCb, pArg, pOriSQL, va);
    va_end(va);

    return bRet;
}

bool CMysqlDAO::useQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...)
{
     va_list va;
     va_start(va, pOriSQL);
     bool bRet = __entrySQL(SQL_USE, strHandleName, pCb, pArg, pOriSQL, va);
     va_end(va);

     return bRet;
}

bool CMysqlDAO::execute(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...)
{
    va_list va;
    va_start(va, pOriSQL);
    bool bRet = __entrySQL(SQL_EXECUTE, strHandleName, pCb, pArg, pOriSQL, va);
    va_end(va);

    return bRet;
}

// 统一SQL入口
bool CMysqlDAO::__entrySQL(const ESQLType& emSQLType, const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, va_list& va)
{
    // 获取连接

	IMysqlConn* pConn = getMysqlConnPool()->getConn(strHandleName);
	if (pConn == NULL)
	{
		getLogReport()->log(Error, "[CMysqlDAO::__entrySQL] handle name:[%s], can't connect", strHandleName.c_str());
		return false;
	}

	// 解析SQL

	std::string strRealSQL;
	MAP_PARAM_t mapParam;

	try
	{
		__praseSQL(pOriSQL, va, strRealSQL, mapParam);
	}
	catch (const mysqlpp::BadQuery& bad)
	{
		if (pCb)
			pCb->onException(bad);
		return false;
	}

	// 执行SQL

	try
	{
		__goSQL(emSQLType, pConn, pCb, pArg, strRealSQL.c_str(), mapParam);
		// 回收连接
		pConn->gc();
		return true;
	}
	catch (const mysqlpp::BadQuery& bad)
	{
		getLogReport()->log(Error, "[CMysqlDAO::__entrySQL] handle name:[%s], occr a exception:[%s]", strHandleName.c_str(), bad.what());

		mysqlpp::Connection& conn = pConn->conn();
		if (conn.errnum() > 0 && conn.errnum() < 2018)
		{
			getLogReport()->log(Error, "[CMysqlDAO::__entrySQL] handle name:[%s], error:%u[%s], try reconnect...", strHandleName.c_str(), conn.errnum(), conn.error());

			// 尝试重连
			bool bRet = pConn->reconect();
			if (bRet)
			{
				getLogReport()->log(Error, "[CMysqlDAO::__entrySQL] handle name:[%s], reconnect ok, and retry go sql", strHandleName.c_str());

				try
				{
					__goSQL(emSQLType, pConn, pCb, pArg, strRealSQL.c_str(), mapParam);
					// 回收连接
					pConn->gc();
					return true;
				}
				catch (const mysqlpp::BadQuery& bad)
				{
					getLogReport()->log(Error, "[CMysqlDAO::__entrySQL] handle name:[%s], retry go sql failed, sql:[%s]", strHandleName.c_str(), strRealSQL.c_str());

					if (pCb)
						pCb->onException(bad);
				}
			}
			else
			{
				getLogReport()->log(Error, "[CMysqlDAO::__entrySQL] handle name:[%s], reconnect failed", strHandleName.c_str());
			}
		}
	}

    // 销毁连接
	pConn->destroy();
	return false;
}

// 解析SQL
bool CMysqlDAO::__praseSQL(const char* pOriSQL, va_list& va, std::string& strRealSQL, MAP_PARAM_t& mapParam)
{
	strRealSQL.reserve(strlen(pOriSQL) * 2);

	int nPos = 0;

    // 解析SQL

	while (pOriSQL[nPos])
	{
		if (pOriSQL[nPos] == '%')
		{
			// 检查是否数字
			if (!isNumber(pOriSQL[nPos+1]))
			{
				throw mysqlpp::BadQuery("invalid sql format");
				return false;
			}

			// 解析数字序号
			uint32_t uNum = atoi(&pOriSQL[nPos+1]);
			uint32_t uNumLen = getNumLength(uNum);

			// 该参数长度（格式如 %1q 或 %2）
			uint32_t uParamLen = 1 + uNumLen;

			// 拷贝参数格式前缀
			strRealSQL.append(pOriSQL+nPos, uParamLen);

			// 处理参数格式后缀类型
			switch (pOriSQL[nPos+uParamLen])
			{
			case 'q':
				{
					mapParam[uNum] = SParamValue(PARAM_STRING);
					strRealSQL.push_back('q');
					uParamLen ++;
				}
				break;
			case 'b':
				{
					mapParam[uNum] = SParamValue(PARAM_BINARY);
					strRealSQL.push_back('q');
					uParamLen ++;
				}
				break;
			case 'l':
				{
					mapParam[uNum] = SParamValue(PARAM_INT64);
					strRealSQL.push_back('l');
					uParamLen ++;
				}
				break;
			default:
				{
					mapParam[uNum] = SParamValue(PARAM_INT32);
				}
			}

			nPos += uParamLen;
		}
		else
		{
			strRealSQL.push_back(pOriSQL[nPos]);
			++ nPos;
		}
	}

	// 解析参数

	for (MAP_PARAM_t::iterator iter = mapParam.begin(); iter != mapParam.end(); ++iter)
	{
		SParamValue& stParam = iter->second;

		switch (stParam.emType)
		{
		case PARAM_INT32:
			{
				stParam.uValue32 = va_arg(va, uint32_t);
			}
			break;
		case PARAM_INT64:
			{
				stParam.uValue64 = va_arg(va, uint64_t);
			}
			break;
		case PARAM_STRING:
			{
				stParam.strValue = va_arg(va, char*);
			}
			break;
		case PARAM_BINARY:
			{
				stParam.strValue = *(va_arg(va, std::string*));
			}
			break;
		default:
			break;
		}
	}

	return true;
}

// 执行SQL
bool CMysqlDAO::__goSQL(const ESQLType& emSQLType, IMysqlConn* pConn,
							 IQueryCallBack* pCb, void* pArg, const char* pRealSQL, const MAP_PARAM_t& mapParam)
{
	mysqlpp::SQLQueryParms params;
	for (MAP_PARAM_t::const_iterator c_iter = mapParam.begin(); c_iter != mapParam.end(); ++c_iter)
	{
		const SParamValue& stParam = c_iter->second;

		switch (stParam.emType)
		{
		case PARAM_INT32:
			{
				params << stParam.uValue32;
			}
			break;
		case PARAM_INT64:
			{
				params << (ulonglong)stParam.uValue64;
			}
			break;
		case PARAM_STRING:
		case PARAM_BINARY:
			{
				params << stParam.strValue;
			}
			break;
		default:
			break;
		}
	}

	mysqlpp::Query q = pConn->query();
	q << pRealSQL;
	q.parse();

	if (pCb && pCb->isPreview())
	{
		pCb->onPreview( q.str(params) );
	}

	switch (emSQLType)
	{
	case CMysqlDAO::SQL_STORE:
		{
			mysqlpp::StoreQueryResult result = q.store(params);
			if (pCb)
				pCb->onStoreResult(result, pArg);
		}
		break;
	case CMysqlDAO::SQL_USE:
		{
			mysqlpp::UseQueryResult result = q.use(params);
			if (pCb)
				pCb->onUseResult(result, pArg);
		}
		break;
	case CMysqlDAO::SQL_EXECUTE:
		{
			SimpleResult result = q.execute(params);
			if (pCb)
				pCb->onSimpleResult(result, pArg);
		}
		break;
	}

	return true;
}
