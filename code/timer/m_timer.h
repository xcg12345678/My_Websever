#ifndef M_TIMER_H
#define M_TIMER_H
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
using namespace std;
class util_timer;
class client_time
{
public:
    int sockfd;
    sockaddr_in address;
    util_timer *m_timer;
};
class util_timer
{
public:
    util_timer() : prev(NULL), next(NULL) {}

public:
    void (*cbfunc)(client_time *); // 回调函数
    timer_t expire;
    client_time *userdata;
    util_timer *prev;
    util_timer *next;
};
class sort_timer_lst // 调整时间
{
public:
    sort_timer_lst();
    ~sort_timer_lst();

    void add_timer(util_timer *timer);
    void adjust_timer(util_timer *timer);
    void del_timer(util_timer *timer);
    void tick();

private:
    void add_timer(util_timer *timer, util_timer *lst_head);

    util_timer *head;
    util_timer *tail;
};
class unitls
{
public:
    void init(int time_slot);
    static void sig_handle(int sig);
    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);
    void addsig(int sig, void(handler)(int), bool restart = true);
    void setsig_noblock(int fd);
    void timer_handler();
    void show_error(int connfd, const char *info);
    static int *u_nipipe;
    static int e_poolfd;
    int TIME_SLOT;
    sort_timer_lst m_time_list;
};
void cbfunc(client_time *userdata);
#endif // !TIMER_H
