#include "nanotimer.h"

NanoTimer::NanoTimer(QObject *parent) : QObject(parent)
{
    connect(&mHelperThread, &HelperThread::timeout, this, &NanoTimer::timeout);

}

void NanoTimer::start(int period)
{
    mHelperThread.mPeriod = period;
    mHelperThread.start();

}

void NanoTimer::stop()
{
    mHelperThread.mContinue = false;
    mHelperThread.quit();
    mHelperThread.wait();
}

