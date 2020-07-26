/* Simpled demo program to show usage of plots in Qt
 *
 */
#include <QApplication>
#include "SimMainWindow.h"

using namespace MR_SIM;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  SimMainWindow w;
  w.resize(1400, 1100);
  w.show();

  return a.exec();
}
