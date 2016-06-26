#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "program.h"
#include <iostream>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("load_test");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("QTimer and POSIX timer performance tester");
    parser.addHelpOption();
    parser.addVersionOption();
    // -i -> interval
    QCommandLineOption period("i", QCoreApplication::translate("main","timer interval"),
                                QCoreApplication::translate("main","interval=USEC"));
    period.setDefaultValue("1000");
    parser.addOption(period);
    // -n -> umber of loops
    QCommandLineOption length("n", QCoreApplication::translate("main","number of loops"),
                              QCoreApplication::translate("main","loops"));
    length.setDefaultValue("10000");
    parser.addOption(length);
    // -t -> timer type
    QCommandLineOption timer("t", QCoreApplication::translate("main","timer type: 0=QT, 1=POSIX"),
                              QCoreApplication::translate("main","timer"));
    timer.setDefaultValue("0");
    parser.addOption(timer);
    // -s -> if save to file
    QCommandLineOption save("s", QCoreApplication::translate("main","indicate if save jitter to file"));
    parser.addOption(save);
    // -l -> if load is present
    QCommandLineOption load("l", QCoreApplication::translate("main","indicate if load is present in system"));
    parser.addOption(load);
    // -r -> Linux is RT patched
    QCommandLineOption kernel("r", QCoreApplication::translate("main","indicate if Linux is RT patched"));
    parser.addOption(kernel);
    parser.process(a);

    if ((parser.value(timer).toInt() == 0) && (parser.value(period).toInt() < 1000)) {
        std::cout << "QTimer has msec resolution, so minimum value for -i is 1000" << std::endl;
        std::cout << "Exit" << std::endl;
        return -1;
    }


    Program program(parser.value(length).toInt(), parser.value(period).toInt(), parser.value(timer).toInt(),
                parser.isSet(save), parser.isSet(load), parser.isSet(kernel));

    return a.exec();
}

