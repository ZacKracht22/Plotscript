#include <QApplication>
#include "notebook_app.hpp"

//Main function that gets executed when the notebook executable is ran.
//Simply shows the NotebookApp and runs the event control loop.
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp noteApp;

  noteApp.show();
  
  return app.exec();
}

