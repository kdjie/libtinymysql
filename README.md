# libtinymysql
这是一个基于mysqlclient和mysqlpp的轻量级实用开发库。




使用前，需要确保本机上已经安装好mysql的开发库。



对centos来说，先安装mysqlclent：

yum install mysql-devel

然后继续源码安装mysqlpp：

自已去网站上下载 https://tangentsoft.com/mysqlpp/home

编译安装

./configure

make

make install

默认安装目录

头文件 /usr/local/include/mysql++

库 /usr/local/lib



对ubuntu来说，都可以从源安装：

apt-get install libmysqlclient-dev

apt-get install libmysql++-dev



然后，下载本开发库，编译：

make

生成libtinymysql.a库文件





本开发库的几个接口和类说明：

IMysqlDAO 这是mysql执行DAO查询的接口，它包含三个方法：

virtual bool storeQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...) = 0;

virtual bool useQuery(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...) = 0;

virtual bool execute(const std::string& strHandleName, IQueryCallBack* pCb, void* pArg, const char* pOriSQL, ...) = 0;

分别表示执行store、use、execute三种操作。

各参数说明：

strHandleName 表示指定连接的句柄名称。句柄是在程序中预设的连接参数名称，通过这个名称可以找到对应的连接。在后面的样例中可以看到句柄的用法。

IQueryCallBack 表示应用层方法集合回调，并且包启了以下方法：

virtual void onUseResult(const mysqlpp::UseQueryResult& result, void* pArg) = 0;

virtual void onStoreResult(const mysqlpp::StoreQueryResult& result, void* pArg) = 0;

virtual void onSimpleResult(const mysqlpp::SimpleResult& result, void* pArg) = 0;

virtual void onException(const mysqlpp::BadQuery& bad) = 0;

virtual void onPreview(const std::string& strSQL) = 0;

分别表示use、store、execute、异常时回调的结果集，通过用mysqlpp的结果集进行操作，可以取出自己想要的数据。



CSyslogReport 这是syslog输出类，在centos下默认输出到/var/log/message中。



CMysqlConnPool 这是mysql连接池类，用来管理连接句柄及实际连接。





样例程序：

#include <stdio.h>

#include <stdlib.h>



#include "libtinymysql/tinymysql.h"



using namespace tinymysql;



// 实现onStoreResult方法，这里演示检索结果集

struct MyQueryCallBack : public IAbsQueryCallBack

{

    // onPreview回调sql预览

    virtual void onPreview(const std::string& strSQL)

    {

        printf("sql:[%s] \n", strSQL.c_str());

    }



    virtual void onStoreResult(const mysqlpp::StoreQueryResult& result, void* pArg)

    {

        printf("=> \n");



        for (int i = 0; i < result.size(); ++i)

        {

            const mysqlpp::Row& row = result[i];



            for (int j = 0; j < row.size(); ++j)

            {

                printf("%s |", row[j].c_str());

            }



            printf("\n");

        }

    }

};



int main(int argc, char* argv[])

{

    // 日志对象

    CSyslogReport syslog;



    // 连接池对象

    CMysqlConnPoolTLS pool;

    pool.setLogReport(&syslog);



    // 连接句柄初使化，这里声明了一个名叫test的句柄

    SHandleParam testParam;

    testParam.setParam("192.168.29.11", "mytest", "123456", 3306, "test2", "utf8");

    pool.setHandleParam("test", testParam); 



    // DAO对象初使化

    CMysqlDAO mysql;

    mysql.setLogReport(&syslog);

    mysql.setMysqlConnPool(&pool);



    // DAO对照执行store查询

    MyQueryCallBack MyCB;

    MyCB.setPreview(true);

    mysql.storeQuery("test", &MyCB, NULL, "select iClassHistoryID,iClassType,iClassVersion,dtCreateTime,dtModifyTime from tbClassHistory where iClassHistoryID > %0", 0);



    return 0;

}



编译：

g++ -o testmysql testmysql.cpp -DMYSQLPP_MYSQL_HEADERS_BURIED -lmysqlpp libtinymysql/libtinymysql.a

-DMYSQLPP_MYSQL_HEADERS_BURIED声明使用mysqlpp

-lmysqlpp表示链接mysqlpp库

libtinymysql/libtinymysql.a以实际的路径为准

执行结果：

sql:[select iClassHistoryID,iClassType,iClassVersion,dtCreateTime,dtModifyTime from tbClassHistory where iClassHistoryID > 0] 

=> 

1 |1 |1 |2018-06-11 15:18:54 |0000-00-00 00:00:00 |
