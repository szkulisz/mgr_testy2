#ifndef NANOTIMER_H
#define NANOTIMER_H

#include <QObject>
#include <QThread>
#include <pthread.h>
#include <time.h>
#include <iostream>

#define CHECK(sts,msg)  \
  if (sts == -1) {      \
    perror(msg);        \
    exit(-1);           \
  }

class HelperThread : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        int sts;
        struct sched_param param;
        sts = sched_getparam(0, &param);
        CHECK(sts,"sched_getparam");
        param.sched_priority = (sched_get_priority_max(SCHED_FIFO));
        sts = sched_setscheduler(0, SCHED_FIFO, &param);
        CHECK(sts,"sched_setscheduler");
        std::cout << "Nanotimer ma ID: " << QThread::currentThreadId() << " i priorytet: " << param.sched_priority << std::endl;

        struct timespec next;
        clock_gettime(CLOCK_MONOTONIC, &next);
        while (mContinue) {
            timespec_add_us(&next, mPeriod);
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
            emit timeout();
        }
    }

    void timespec_add_us(struct timespec *t, long us)
    {
        t->tv_sec += us/1000000;
        t->tv_nsec += (us%1000000)*1000;
        if (t->tv_nsec > 1000000000) {
            t->tv_nsec = t->tv_nsec - 1000000000;// + ms*1000000;
            t->tv_sec += 1;
        }
    }
signals:
    void timeout();
public:
    bool mContinue = true;
    int mPeriod;

};

class NanoTimer : public QObject
{
    Q_OBJECT
public:
    explicit NanoTimer(QObject *parent = 0);
    void start(int period = 1000);
    void stop();

signals:
    void timeout();

public slots:

private:
    HelperThread mHelperThread;
};

#endif // NANOTIMER_H
