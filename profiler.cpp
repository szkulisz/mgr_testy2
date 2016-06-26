#include "profiler.h"
#include <cmath>
#include <QTextStream>
#include <iostream>

#define CHECK(sts,msg)  \
  if (sts == -1) {      \
    perror(msg);        \
    exit(-1);           \
  }


Profiler::Profiler(QObject *parent)
    : QObject(parent)
{
}



Profiler::~Profiler()
{

}

void Profiler::startProfiling()
{
    clock_gettime(CLOCK_MONOTONIC, &mTimePrevious);
}

void Profiler::updatePeriodProfiling()
{
    clock_gettime(CLOCK_MONOTONIC, &mTimeActual);
    mTimerDifference = countDifference( mTimePrevious, mTimeActual );
    mTimePrevious = mTimeActual;
    if (mSave) {
        if (mLogPeriodTableIdx <= mLoops) {
            mLogPeriodTable[mLogPeriodTableIdx++] = getDifferenceInNanoseconds();
        } else {
            std::cout << "Przekroczyles rozmiar tablicy..." << std::endl;
        }
    }
}

void Profiler::updateHandlerTimeProfiling()
{
    clock_gettime(CLOCK_MONOTONIC, &mTimeActual);
    mTimerDifference = countDifference( mTimePrevious, mTimeActual );
    if (mSave) {
        if (mLogHandlerTableIdx <= mLoops) {
            timespec temp = countDifference(mTimePrevious, mTimeActual);
            mLogHandlerTable[mLogHandlerTableIdx++] = 1000000000*temp.tv_sec + temp.tv_nsec;
        } else {
            std::cout << "Przekroczyles rozmiar tablicy..." << std::endl;
        }
    }
}

int Profiler::getDifferenceInSeconds()
{
    return std::round(getDifferenceInNanoseconds() / 1000000000.0);
}

int Profiler::getDifferenceInMiliseconds()
{
    return std::round(getDifferenceInNanoseconds() / 1000000.0);
}

int Profiler::getDifferenceInMicroseconds()
{
    return std::round(getDifferenceInNanoseconds() / 1000.0);
}

long long Profiler::getDifferenceInNanoseconds()
{
    long long periodInNs = mPeriod*1000;
    return (1000000000*mTimerDifference.tv_sec + mTimerDifference.tv_nsec) - periodInNs;
}

void Profiler::saveLogFile()
{
    if (mSave) {
        mLogFile.open(QFile::WriteOnly | QFile::Text);
        QTextStream logStream(&mLogFile);
        std::cout << "\nzapis do pliku" << std::endl;
        for(unsigned int i=1; i<mLoops-1; ++i)
            logStream << mLogPeriodTable[i] << ", " << mLogHandlerTable[i] << "\n";
        delete mLogPeriodTable;
        delete mLogHandlerTable;
        if (mLogFile.isOpen())
            mLogFile.close();
        std::cout << "koniec zapisu" << std::endl;
    }
}


void Profiler::startLogging(int period, int loops, bool save, const QString &fileName)
{
    mSave = save;
    if (mSave) {
        mPeriod = period;
        mLoops = loops;
        mLogPeriodTable = new long long[loops];
        mLogHandlerTable = new long long[loops];
        mLogFile.setFileName(fileName);
    }
}



timespec Profiler::countDifference(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}


