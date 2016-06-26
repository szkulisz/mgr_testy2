#ifndef PROFILER_H
#define PROFILER_H

#include <time.h>
#include <QFile>
#include <QString>
#include <QObject>


class Profiler : public QObject
{
    Q_OBJECT
public:
    explicit Profiler(QObject *parent = 0);
    ~Profiler();

    void startProfiling();
    void updatePeriodProfiling();
    void updateHandlerTimeProfiling();
    int getDifferenceInSeconds();
    int getDifferenceInMiliseconds();
    int getDifferenceInMicroseconds();
    long long getDifferenceInNanoseconds();
    void saveLogFile();

    void startLogging(int period, int loops, bool save, const QString &fileName);



private:
    bool mSave;
    int mPeriod;
    timespec mTimePrevious, mTimeActual, mTimerDifference;
    unsigned int mLoops;
    long long *mLogPeriodTable;
    unsigned int mLogPeriodTableIdx=0;
    long long *mLogHandlerTable;
    unsigned int mLogHandlerTableIdx=0;
    QFile mLogFile;
    QString mFileName;

    timespec countDifference(timespec start, timespec stop);

};





#endif // PROFILER_H
