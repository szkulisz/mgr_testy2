#include "posixtimer.h"

PosixTimer::PosixTimer(int test, QObject *parent) : QObject(parent),mTest(test)
{
    // Clear the sa_mask
    sigemptyset(&this->mSignalAction.sa_mask);
    // set the SA_SIGINFO flag to use the extended signal-handler function
    this->mSignalAction.sa_flags = SA_SIGINFO;

    // Define sigaction method
    // This function will be called by the signal
    this->mSignalAction.sa_sigaction = PosixTimer::timeoutHandler;


    // Define sigEvent
    // This information will be forwarded to the signal-handler function
    memset(&this->mSignalEvent, 0, sizeof(this->mSignalEvent));
    // With the SIGEV_SIGNAL flag we say that there is sigev_value
    this->mSignalEvent.sigev_notify = SIGEV_SIGNAL;
    // Now it's possible to give a pointer to the object
    this->mSignalEvent.sigev_value.sival_ptr = (void*) this;
    // Declare this signal as Alarm Signal
    this->mSignalEvent.sigev_signo = SIGALRM;

    // Install the Timer
    if (timer_create(CLOCK_MONOTONIC, &this->mSignalEvent, &this->mTimerID)!= 0) {
        perror("Could not create the timer");
        exit(1);
    }

    // Finally install tic as signal handler
    if (sigaction(SIGALRM, &this->mSignalAction, NULL)) {
        perror("Could not install new signal handler");
    }
}

void PosixTimer::start(int period) {
//    long long freq_nanosecs = period * 1000000;
    mTimerSpec.it_value.tv_sec = period / 1000000;
    mTimerSpec.it_value.tv_nsec = (period * 1000) % 1000000000;
    mTimerSpec.it_interval.tv_sec = period / 1000000;
    mTimerSpec.it_interval.tv_nsec = (period * 1000) % 1000000000;

    // Set the timer and therefore it starts...
    if (timer_settime(this->mTimerID, 0, &this->mTimerSpec, NULL) == -1) {
        perror("Could not start timer:");
    }

}

void PosixTimer::stop()
{
    mTimerSpec.it_value.tv_sec = 0;
    mTimerSpec.it_value.tv_nsec = 0;
    mTimerSpec.it_interval.tv_sec = 0;
    mTimerSpec.it_interval.tv_nsec = 0;

    // Set the timer and therefore it stops...
    if (timer_settime(this->mTimerID, 0, &this->mTimerSpec, NULL) == -1) {
        perror("Could not stop timer:");
    }
}

void PosixTimer::timeoutHandler(int, siginfo_t *si, void *) {
    // get the pointer out of the siginfo structure and asign it to a new pointer variable
    PosixTimer *ptrTimerClass =
    reinterpret_cast<PosixTimer *> (si->si_value.sival_ptr);

    // call the member function
    emit ptrTimerClass->timeout();

}
