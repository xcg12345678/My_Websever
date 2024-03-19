#include <iostream>
#include "my_sql.h"
using namespace std;

my_sql::~my_sql()
{
    destory_sql();
}
void my_sql::init(std::string url, std::string username, std::string password, std::string DateBaseName, int port, int max_con)
{
    for (int i = 0; i < max_con; i++)
    {
        MYSQL *conn = NULL;
        conn = mysql_init(conn);
        if (conn == NULL)
        {
            LOG_ERROR("MYSQL Error");
        }
        if (mysql_real_connect(conn, url.c_str(), username.c_str(), password.c_str(), DateBaseName.c_str(), port, NULL, 0) == NULL)
        {
            LOG_ERROR("MySQL Error");
        }
        con_sqls.push_back(conn);
    }
    m_revers = sem(max_con);
}
my_sql *my_sql::get_sql()
{
    static my_sql instance;
    return &instance;
}
MYSQL *my_sql::get_con()
{
    MYSQL *conn = NULL;
    if (con_sqls.size() == 0)
    {
        return NULL;
    }
    m_revers.wait();
    mutex.lock();
    conn = con_sqls.front();
    con_sqls.pop_front();
    mutex.unlock();
    return conn;
}
bool my_sql::ReleaseConnection(MYSQL *con)
{
    if (NULL == con)
        return false;
    mutex.lock();
    con_sqls.push_back(con);
    mutex.unlock();
    m_revers.post();
    return true;
}
void my_sql::destory_sql()
{
    mutex.lock();

    list<MYSQL *>::iterator it;
    for (it = con_sqls.begin(); it != con_sqls.end(); ++it)
    {
        MYSQL *conn = *it;
        mysql_close(conn);
    }

    con_sqls.clear();
    mutex.unlock();
}

conn_sql::conn_sql(MYSQL **SQL, my_sql *con_pool)
{
    *SQL = con_pool->get_con();
    conn = *SQL;
    poll_RAII = con_pool;
}

conn_sql::~conn_sql()
{
    poll_RAII->ReleaseConnection(conn);
}