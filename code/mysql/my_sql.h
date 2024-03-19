#ifndef MYSQL_H
#define MYSQL_H

#include <iostream>
#include <list>
#include <mysql/mysql.h>
#include "../log/log.h"
#include "../lock/locker.h"

using namespace std;
class my_sql
{
public:
    static my_sql *get_sql(); // 懒汉模式单例模式
    void destory_sql();
    bool ReleaseConnection(MYSQL *con);
    MYSQL *get_con();
    void init(string url, string username, string password, string DateBaseName, int port, int max_con);

private:
    my_sql();
    ~my_sql();
    list<MYSQL *> con_sqls;
    sem m_revers;
    locker mutex;
};

// RAII模式
class conn_sql
{
public:
    conn_sql(MYSQL **SQL, my_sql *con_pool);
    ~conn_sql();

private:
    MYSQL *conn;
    my_sql *poll_RAII;
};
#endif // !MYLSQ_H