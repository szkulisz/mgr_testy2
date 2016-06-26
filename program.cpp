#include "program.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include "assert.h"

#define CHECK(sts,msg)  \
  if (sts == -1) {      \
    perror(msg);        \
    exit(-1);           \
  }

Program::Program(int loop, int period, int timer, bool save, bool load, bool rtKernel, QObject *parent)
    : QObject(parent),
      mLoad(load),
      mLoopNumber(loop),
      mPeriod(period)
{
//    save = 1;
//    mSave = 1;
//    mLoopNumber = 10000;
//    mPeriod = 6100000;
//    mHighPrio = 1;
//    timer = 2;

        if (mLoad) {
            mChildPid = fork();
            if (mChildPid == 0) {
                char *argv[] = {(char*)"/usr/bin/stress",
                                (char*)"--cpu", (char*)"1",(char*)"--io", (char*)"1",
                                (char*)"--vm", (char*)"1",(char*)"--vm-bytes", (char*)"128M",
                                (char*)"--hdd", (char*)"1",(char*)"--hdd-bytes", (char*)"128M",
                                0};
                execv("/usr/bin/stress",argv);
            }
        }

    int sts;
    struct sched_param param;
    sts = sched_getparam(0, &param);
    CHECK(sts,"sched_getparam");
    param.sched_priority = (sched_get_priority_max(SCHED_FIFO));
    sts = sched_setscheduler(0, SCHED_FIFO, &param);
    CHECK(sts,"sched_setscheduler");
    std::cout << "proces ma ID: " << QThread::currentThreadId() << " i priorytet: " << param.sched_priority << std::endl;

    mName =  QString::number(period);
    if (rtKernel) {
        mName += "_rt_";
    } else {
        mName += "_norm_";
    }
    if (load) {
        mName += "with_";
    } else {
        mName += "without_";
    }
    if (mHighPrio) {
        mName += "hi_";
    } else {
        mName += "low_";
    }
    switch (timer) {
    case 0:
        mName += "qt";
        break;
    case 1:
        mName += "pos";
        break;
    case 2:
        mName += "nano";
        break;
    default:
        std::cout << "Incorrect -t value " << timer << std::endl;
        std::cout << "Exit" << std::endl;
        QCoreApplication::quit();
        break;
    }

    connect(&mQTimer, &QTimer::timeout, this, &Program::onTimeout);
    connect(&mPosixTimer, &PosixTimer::timeout, this, &Program::onTimeout);
    connect(&mNanoTimer, &NanoTimer::timeout, this, &Program::onTimeout);

    mProfiler.startLogging(period, loop, save, "logs/" + mName + ".txt");
    mProfiler.startProfiling();
    switch (timer) {
    case 0:
        mQTimer.start(mPeriod/1000);
        break;
    case 1:
        mPosixTimer.start(mPeriod);
        break;
    case 2:
        mNanoTimer.start(mPeriod);
        break;
    default:
        break;
    }



}

Program::~Program()
{

}



void Program::onTimeout()
{
    mProfiler.updatePeriodProfiling();

    ++mCounter;
//    mTest = timer_getoverrun(mPosixTimer.mTimerID);
//    if (mTest != 0 && mCounter!=1) {
//        mOverrunCounter++;
//        if (mTest>mMaxOverrun){
//            mMaxOverrun = mTest;
//            mMaxNano = mProfiler.getDifferenceInNanoseconds();
//            mMaxCounter = mCounter;
//        }
//    }

    if ((mPeriod>=1000000) ? true : (mCounter%(1000000/mPeriod) == 0)) {
        std::cout << "\r" << mCounter << ' ' << mName.toStdString() << std::flush;
    }
    if (mCounter >= mLoopNumber) {
        if (mLoad) {
            if (system("pkill -f stress") == -1 ) {
                perror("pkill");
                if (system("pkill -f -9 stress") == -1 ) {
                    perror("pkill -9");
                }
            }
        }
        std::cout << "\noverrunCounter: " << mOverrunCounter <<std::endl;
        std::cout << "maxOverrun: " << mMaxOverrun << std::endl;
        std::cout << "mMaxNano: " << mMaxNano << std::endl;
        std::cout << "maxCounter: " << mMaxCounter << std::endl;
        mQTimer.stop();
        mPosixTimer.stop();
        mNanoTimer.stop();
        mProfiler.saveLogFile();
        QCoreApplication::quit();
    }

    mProfiler.updateHandlerTimeProfiling();
}

