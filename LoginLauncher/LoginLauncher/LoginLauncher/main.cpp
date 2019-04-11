#include "LoginLauncher.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LoginLauncher w;
	w.show();
	return a.exec();
}
