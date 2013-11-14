#ifndef GUI_H
#define GUI_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QString>
#include <QList>

#include "Print.hpp"
#include "Dialog.hpp"
#include "../config.hpp"
#include "../solver/FluidSolver2D.hpp"
#include "../solver/FloatMatrix2D.hpp"
#include "LeapMotion.h"


/**
 * Class herited from myGLWidget corresponding
 * to the window where the fluid is displayed.
 */
class GUI : public QGLWidget
{
  Q_OBJECT//Maccro used to define new SLOTS
  public:

  GUI(QWidget *parent,                          \
      QString name,       \
      Print *p,         \
      Config &configurationDatas,
      bool drawVelocityField,\
      bool enableMouseMove);
  ~GUI();

  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void keyPressEvent(QKeyEvent *keyEvent);
  void mousePressEvent(QMouseEvent *mouseEvent);
  void mouseReleaseEvent(QMouseEvent *mouseEvent);
  void mouseMoveEvent(QMouseEvent *mouseEvent);
  void wheelEvent(QWheelEvent *mouseEvent);
  void toggleFullWindow();
  void calculateFPS();

  void fillSquare(unsigned int sqrWidth, unsigned int sqrHeight, float value, FloatMatrix2D* matrix, bool prev = false);
  void addObstacle(float sqrWidth, float sqrHeight);
  void resizeMatrix(FloatMatrix2D &out, const FloatMatrix2D  &in, int k);
  void addPrintMode(Print *p);

  FluidSolver *fluid;


public slots:
  virtual void timeOutSlot();
  void saveConfig();
  void loadConfig();
  void desPause(){_pause = false;};

private:
  QTimer *t_Timer;         // used to refresh the window.
  bool _pause;             // pause the simulation
  bool b_Fullscreen;       // window fullscreen state
  bool _drawVelocityField; // toggle velocity field drawings
  float coef;              // 'radius' of the cursor
  int mouseX;     // horizontal position of the mouse
  int mouseY;     // hertical position of the mouse
  int prevMouseX; // previous event horizontal position of the mouse
  int prevMouseY; // previous event vertical position of the mouse
  bool pressing;           // is mouse left button pressed ?
  bool emptying;           // is mouse right button pressed ?
  bool _enableMouseMove;    // toggle mouse velocity modifications
  Config &configuration;
  bool waitingMousePress;   // waiting for button pressed?
  int firstPosX;
  int firstPosY;

  Dialog *saveWin;
  Dialog *loadWin;

  // LeapMotion
  Leap::Controller leap;
  QList <Leap::Vector> fingers;


  // information displayed in the title
  float fps;
  unsigned int dispMouseX, dispMouseY;
  float dispDens, dispVelX, dispVelY;

  // Print modes
  QList <Print *> _printModes;
  unsigned int _currentPrintMode;

  // Average matrices
  FloatMatrix2D *M1;
  FloatMatrix2D *M2;

  // Reduction factor
  static const int k = 5;
};

#endif // GUI_H
