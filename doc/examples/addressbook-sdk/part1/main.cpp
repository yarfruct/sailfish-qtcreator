//! [main function]
#include <QApplication>
#include "addressbook.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AddressBook w;
    w.show();
    return a.exec();
}
//! [main function]
