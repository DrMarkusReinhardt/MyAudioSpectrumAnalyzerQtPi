/* Simpled audio spectrum analyzer program based on Qt for
 * the Raspberry Pi 3B+ with Pisound board
 *
 * History: Creation, Dr. Markus Reinhardt, July 2020
 *
 */
#include <QApplication>
#include "SimMainWindow.h"

using namespace MR_SIM;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  SimMainWindow w;
  w.resize(1500, 1100);
  w.show();

  return a.exec();
}
