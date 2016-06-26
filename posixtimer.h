#ifndef POSIXTIMER_H
#define POSIXTIMER_H

#include <QObject>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

class PosixTimer : public QObject
{
    Q_OBJECT
public:
    explicit PosixTimer(int test = 0, QObject *parent = 0);
    void start(int period = 1000);
    void stop();


signals:
    void timeout();

public slots:

//private:
public:
    timer_t mTimerID;
    struct itimerspec mTimerSpec;
    sigset_t mSigBlockSet;
    struct sigevent mSignalEvent;
    struct sigaction mSignalAction;
    int mTest;

    /**
    * The signal handler function with extended signature
    */
    static void timeoutHandler(int, siginfo_t *si, void *);

};

#endif // POSIXTIMER_H
