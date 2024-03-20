#include <iostream>
#include <string.h>
#include <cstring>
using namespace std;
int main()
{
    char text[] = {"GET https:///favicon.ico HTTP/1.1\r\n"}; // GET / HTTP/1.1\r\n

    char *m_url = strpbrk(text, " \t"); // 获取第一个空格后字符
    cout << m_url << endl;
    *m_url++ = '\0';
    char *m_method = text;
    cout << m_method << endl; // 跳跃空格后
    m_url += strspn(m_url, " \t");
    cout << m_url << endl;
    char *m_version = strpbrk(m_url, " \t");
    cout << m_version << endl;
    *m_version++ = '\0';
    m_version += strspn(m_version, " \t"); // 跳跃空格后
    cout << m_version << endl;
    m_url += 8;
    m_url = strchr(m_url, '/');
    cout << m_url << endl;
    char *m_host;
    char m_text[] = "Host: 192.168.145.128:9006\r\n";
    char *mm_text = m_text;
    if (strncasecmp(mm_text, "Host:", 5) == 0)
    {
        mm_text += 5;
        mm_text += strspn(mm_text, " \t");
        m_host = mm_text;
    }
    cout << m_host << endl;
    return 0;
}