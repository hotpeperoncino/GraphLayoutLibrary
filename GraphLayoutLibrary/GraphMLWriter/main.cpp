#include <QtCore/QCoreApplication>
#include "testgraphmlwriter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestGraphmlWriter testWriter;

    testWriter.test(5000);

    return a.exec();
}
