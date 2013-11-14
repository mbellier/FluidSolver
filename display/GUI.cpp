#include "GUI.hpp"
#include "../config.hpp"
#include "../solver/FloatMatrix2D.hpp"
#include "../solver/FluidSolver2D.hpp"

#include "Leap.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

GUI::GUI(QWidget *parent,
	 QString name,
	 Print *p,
	 Config &configurationDatas,
	 bool drawVelocityField,
	 bool enableMouseMove)

  : QGLWidget(parent),
    _pause(false),
    b_Fullscreen(false),
    _drawVelocityField(drawVelocityField),
    _enableMouseMove(enableMouseMove),
    configuration(configurationDatas)
{

  /* default title */
  setWindowTitle(name);

  /* set window geometry */
  resize(configuration.getWidth(true), configuration.getHeight(true));

  if(configuration.getFPS() == 0){

    /* No refreshing */
    t_Timer = NULL;

  }
  else{

    /* new timer */
    t_Timer = new QTimer(this);
    connect(t_Timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));

    /* set interval in millisecond */
    t_Timer->start(1000 / configuration.getFPS());
  }

  /* new fluid */
  fluid = new FluidSolver(configuration.getWidth(), configuration.getHeight(), configurationDatas);
  if(strcmp("",configurationDatas.getVelXFile()) != 0)
    fluid->_u_prev->load(configurationDatas.getVelXFile());
  if(strcmp("",configurationDatas.getVelYFile()) != 0)
    fluid->_v_prev->load(configurationDatas.getVelYFile());
  if(strcmp("",configurationDatas.getDensFile()) != 0)
    fluid->_dens_prev->load(configurationDatas.getDensFile());

  /* print modes */
  _printModes.append(p);
  _currentPrintMode = 0;

  /* will be used by resized velocity field drawings */
  M1 = new FloatMatrix2D(fluid->_u->getSize(1) / k, fluid->_u->getSize(0) / k);
  M2 = new FloatMatrix2D(fluid->_v->getSize(1) / k, fluid->_v->getSize(0) / k);

  /* mouse parameters */
  setMouseTracking(true);
  coef = 0.5;
  mouseX = 0;
  mouseY = 0;
  dispMouseX = 0;
  dispMouseY = 0;
  pressing = false;
  emptying = false;
  waitingMousePress = false;

  /* dialog windows to save/load a configuration*/
  saveWin = new Dialog(this);
  saveWin->setWindowTitle("Save configuration");
  connect(saveWin, SIGNAL(accepted()), this, SLOT(saveConfig()));
  connect(saveWin, SIGNAL(rejected()), this, SLOT(desPause()));

  loadWin = new Dialog(this);
  loadWin->setWindowTitle("Open configuration");
  connect(loadWin, SIGNAL(accepted()), this, SLOT(loadConfig()));
  connect(loadWin, SIGNAL(rejected()), this, SLOT(desPause()));

}

/**
 * Initializes the openGL parameters
 */
void GUI::initializeGL(){
  glEnable     (GL_BLEND);
  glBlendFunc  (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel (GL_SMOOTH);
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth (1.0f);
  glEnable     (GL_DEPTH_TEST);
  glDepthFunc  (GL_LEQUAL);
  glHint       (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

/**
 * Resizes the openGL window.
 */
void GUI::resizeGL(int width, int height){
  if(height == 0)
    height = 1;
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/**
 * Draws the new state of the screen
 */
void GUI::paintGL(){

  /* clear opengl buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  /* * * drawings * * */

  /* (1) density  */

  _printModes[_currentPrintMode]->printMatrixScalar(*(fluid->_dens), *(fluid->_u), *(fluid->_v));

  /* (2) velocity */

  if (_drawVelocityField){

    /* adjusting display resolution of the velocity field */
    resizeMatrix(*M2, *(fluid->_v), k);
    resizeMatrix(*M1, *(fluid->_u), k);

    _printModes[_currentPrintMode]->printMatrixVector(*M1, *M2);
  }

  /* (3) obstacles   */

  _printModes[_currentPrintMode]->drawObstacle(fluid->_dens->getSize(1),
                                              fluid->_dens->getSize(0),
                                              *(fluid->_obstacles));

  /* (4) cursor   */

  _printModes[_currentPrintMode]->printSquare(mouseX,
		     mouseY,
		     (20 * coef) / fluid->_dens->getSize(1),
		     (20 * coef) / fluid->_dens->getSize(0),
		     width(),
		     height());

  /* (5) Leap Motion cursors */
  for (int i=0; i < fingers.size(); i++)
    _printModes[_currentPrintMode]->drawCircle(fingers[i].x / fluid->_dens->getSize(1) * width(),
                                               fingers[i].y / fluid->_dens->getSize(0) * height(),
                                               40,
                                               width(), height(),
                                               0.5f, 0.5f, 0.8f);
}

/** 
 * Creates a new obstacle defined by the dimension given
 *
 * @param sqrWidth Width of the obstacle wanted
 * @param sqrHeight Height of the obstacle wanted
 */
void GUI::addObstacle(float sqrWidth, float sqrHeight){
  /* matrix size */
  const int n = fluid->_dens->getSize(0);
  const int m = fluid->_dens->getSize(1);
  int xPos, yPos;

  /* position of the mouse on the grid */
  xPos = 1 +    ((float) mouseX / width() ) * m;
  yPos = 1 + n - ((float) mouseY / height()) * n;

  /* avoid out of bound positions */
  if (xPos < 0) xPos = 0;
  if (xPos > m) xPos = m;
  if (yPos < 0) yPos = 0;
  if (yPos > n) yPos = n;

  const int A1 =  MAX(2, yPos - (int)sqrHeight / 2);
  const int A0 =  MAX(2, xPos - (int)sqrWidth  / 2);
  const int B1 =  MIN(n-2, yPos + (int)sqrHeight / 2);
  const int B0 =  A0;
  
  sqrWidth = MIN(sqrWidth, m - 2 - A0 - 1);

  fluid->_obstacles->addSegment(A0, A1, B0, B1, sqrWidth);
}

/**
 * Refresh loop
 */
void GUI::timeOutSlot(){

  /* calculates simulation FPS */
  calculateFPS();

  /* updates window title */
  dispDens = fluid->_dens->get(dispMouseX, dispMouseY);
  dispVelX = fluid->_u->get(dispMouseX, dispMouseY);
  dispVelY = fluid->_v->get(dispMouseX, dispMouseY);
  QString title;
  if (_pause) title.sprintf("PAUSED");
  else title.sprintf("%.1f fps", fps);
  title.sprintf("%s - (%d ; %d) - Density = %.3f - Velocity = (%.1e ; %.1e)",
		title.toStdString().c_str(),
		dispMouseX,
		dispMouseY,
		dispDens,
		dispVelX,
		dispVelY);
  this->setWindowTitle(title);

  // TODO: attribute
  const float _fillingSpeed = 1.0 / 10;

  /* Mouse events */
  if(pressing)
    fillSquare(20 * coef, 20 * coef,  configuration.getDt() * _fillingSpeed, fluid->_dens);
  if(emptying)
    fillSquare(20 * coef, 20 * coef, -configuration.getDt() * _fillingSpeed, fluid->_dens);

  /* Leap Motion */
  Leap::PointableList pointables = leap.frame().pointables();
  Leap::InteractionBox iBox = leap.frame().interactionBox();
  float windowWidth = configuration.getWidth();
  float windowHeight = configuration.getHeight();

  fingers.clear();

  for( int p = 0; p < pointables.count(); p++ )
  {
      Leap::Pointable pointable = pointables[p];
      Leap::Vector normalizedPosition = iBox.normalizePoint(pointable.stabilizedTipPosition());
      normalizedPosition.x *= windowWidth;
      normalizedPosition.y *= windowHeight;
      float x = normalizedPosition.x;// * windowWidth;
      float y = normalizedPosition.y;// * windowHeight;

      if(pointable.touchZone() != Leap::Pointable::Zone::ZONE_NONE)
      {
        Leap::Vector vel = pointable.tipVelocity();
        fluid->_u->set(x,y, (vel.x)/windowWidth / 2);
        fluid->_v->set(x,y, (vel.y)/windowHeight / 2);
        if(pointable.touchDistance() <= 0)
        {
          //          gl::color(1, 0, 0, -pointable.touchDistance());

          int delta = 10;
          for (int i=-delta/2; i<delta/2;i++)
            for (int j=-delta/2; j<delta/2; j++)
              fluid->_dens->set(x-i,y-j, fluid->_dens->get(x-i,y-j)
                                + (0.5-pointable.touchDistance())/4);
        }

        fingers.append(normalizedPosition);
      }
      else
      {
//          gl::color(0, 0, 1, .05);
      }

      _printModes[_currentPrintMode]->drawCircle(x, y, 40, windowWidth, windowHeight, 1., 1., 1.);



  }

  if (!_pause){

    /* Solver computations */
    fluid->velStep (fluid->_u, fluid->_v, fluid->_u_prev, fluid->_v_prev,
		    configuration.getViscosity(), configuration.getDt());
    fluid->densStep(fluid->_dens, fluid->_dens_prev, fluid->_u, fluid->_v,
                    configuration.getDiff(), configuration.getDt());

    /* Reset matrices */
    fluid->_u_prev->fill(0);
    fluid->_v_prev->fill(0);
    fluid->_dens_prev->fill(0);

    fluid->_dens_prev->add(*(fluid->_dens_src));
    fluid->_u_prev->add(*(fluid->_u_vel_src));
    fluid->_v_prev->add(*(fluid->_v_vel_src));
  }

  /* drawings */
  updateGL();
}






/* * * * * * * * * * * * * * * * TOOLS * * * * * * * * * * * * * * * * * * */


/**
 * Toggles fullscreen
 */
void GUI::toggleFullWindow(){
  if(b_Fullscreen){
    showNormal();
    b_Fullscreen = false;
  }
  else{
    showFullScreen();
    b_Fullscreen = true;
  }
}

/**
 * Adds a new print mode to the list
 * 
 * @param p Print mode to add
 */
void GUI::addPrintMode(Print *p){
  _printModes.append(p);
}

/**
 * Calculates the frames per second
 */
void GUI::calculateFPS(){
  static QTime time;
  static unsigned int frameNumber = 1;
  unsigned int msec;
  msec = time.elapsed();
  if (msec < 1000) {
    frameNumber++;
  }
  else {
    fps = frameNumber * (1000 / (float)msec);
    frameNumber = 0;
    time.restart();
  }
}

/**
 * Fills a centered square in the matrix with a specific value
 * 
 * @param sqrWidth Width of the square to fill
 * @param sqrHeight Height of the square to fill
 * @param value Value to set in the matrix
 * @param matrix Matrix to modify
 * @param prev Indicates if you are seting a velocity source or not
 */
void GUI::fillSquare(unsigned int sqrWidth, 
		     unsigned int sqrHeight,
		     float value,
		     FloatMatrix2D* matrix,
		     bool prev){
  /* matrix size */
  const int n = matrix->getSize(0);
  const int m = matrix->getSize(1);
  int xPos, yPos;

  if(prev){
  /* previous position of the mouse on the grid */
    xPos =        ((float) firstPosX / width() ) * m;
    yPos =  n -   ((float) firstPosY / height()) * n;
  }
  else{
  /* position of the mouse on the grid */
    xPos = 1 +    ((float) mouseX / width() ) * m;
    yPos = 1 + n - ((float) mouseY / height()) * n;
  }

  /* avoid out of bound positions */
  if (xPos < 0) xPos = 0;
  if (xPos > m) xPos = m;
  if (yPos < 0) yPos = 0;
  if (yPos > n) yPos = n;


  /* center a square on the mouse cursor */
  const int jMin =  MAX(0, yPos - (int)sqrHeight / 2);
  const int iMin =  MAX(0, xPos - (int)sqrWidth  / 2);
  const int jMax =  MIN(n, yPos + (int)sqrHeight / 2);
  const int iMax =  MIN(m, xPos + (int)sqrWidth  / 2);

  /* fill the square */
  float newVal;
  for (int i = iMin; i < iMax; ++ i){
    for(int j = jMin; j < jMax; ++ j){

      newVal = matrix->get(i, j) + value;
      if(!prev)
        if (newVal < 0) newVal = 0;
      if(!(fluid->_obstacles->isInObstacles(i,j))){
        matrix->set( i, j, newVal);
      }
    }
  }
}



/**
 * Returns a matrix 'out' given a matrix 'in'.
 * The values of out are the average of the values of in
 * located in submatrix of size k.
 *
 * @param out Matrix resized
 * @param in Matrix to resize
 * @param k Reduction factor
 */
void GUI::resizeMatrix(FloatMatrix2D &out, const FloatMatrix2D  &in, int k){

  /* out size */
  const unsigned int newSize1 = in.getSize(0) / k;
  const unsigned int newSize2 = in.getSize(1) / k;

  /* size checking */
  if (out.getSize(0) != newSize1 || out.getSize(1) != newSize2) {
    fprintf(stderr, "Error in GUI::resizeMatrix : wrong size of output matrix.\n");
    return;
  }

  float average = 0;

  /* for each submatrix */
  for (unsigned int im = 0; im < newSize2; ++im){
    for (unsigned int jm = 0; jm < newSize1; ++jm){
      /* compute the average value of a submatrix */
      for (unsigned int iX = im * k; iX < (im + 1) * k; ++iX){
        for (unsigned int jX = jm * k; jX < (jm + 1) * k; ++jX){
          average += in.get(iX,jX);
        }
      }
      out.set(im, jm, average / ( k * k ));
      average = 0;
    }
  }
}

/* * * * * * * * * * * * * * * * EVENTS * * * * * * * * * * * * * * * * * * */


/**
 * Manages the events on mouse buttons.
 *
 * @param mouseEvent Event of the mouse related to a pressure
 */
void GUI::mousePressEvent(QMouseEvent *mouseEvent){
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  if(mouseEvent->buttons() == Qt::LeftButton){
    if(waitingMousePress == true){
      float dirX =     ((int)mouseX - firstPosX);
      float dirY =    -((int)mouseY - firstPosY);
      fillSquare(20 * coef, 20 * coef, 0.00001 * dirX, fluid->_u_vel_src, true);
      fillSquare(20 * coef, 20 * coef, 0.00001 * dirY, fluid->_v_vel_src, true);
      fillSquare(20 * coef, 20 * coef, 0.05, fluid->_dens_src, true);
      waitingMousePress = false;
    }
    else if(modifiers == Qt::ShiftModifier){
      firstPosX = mouseX;
      firstPosY = mouseY;
      waitingMousePress = true;
    }
    else if(modifiers == Qt::ControlModifier){
      fillSquare(20 * coef, 20 * coef, 0.05, fluid->_dens_src);
      //TO DO parmeter the value with dt
    }
    else{
      pressing = true;
    }
  }
  if(mouseEvent->buttons() == Qt::RightButton){
    if(modifiers == Qt::ControlModifier){
      addObstacle(20 * coef, 20 * coef);
    }
    else{
      emptying = true;
    }
  }
}

/**
 * Manages the events on mouse buttons when they are released.
 *
 * @param mouseEvent Event of the mouse related to a release
 */
void GUI::mouseReleaseEvent(QMouseEvent *mouseEvent) {
  if(mouseEvent->buttons() != Qt::LeftButton) {
    pressing = false;
  }
  if(mouseEvent->buttons() != Qt::RightButton) {
    emptying = false;
  }
}


/**
 * Manages the events on mouse movements.
 *
 * @param mouseEvent Event of the mouse related to a movement
 */
void GUI::mouseMoveEvent(QMouseEvent *mouseEvent){
  /* matrix size */
  const unsigned int n = (fluid->_dens)->getSize(0);
  const unsigned int m = (fluid->_dens)->getSize(1);

  /* mouse position */
  const unsigned int xPos =     (float) mouseEvent->x() / width()  * m;
  const unsigned int yPos = n - (float) mouseEvent->y() / height() * n;

 /* store mouse pos. */
  prevMouseX = mouseX;
  prevMouseY = mouseY;
  mouseX = mouseEvent->x();
  mouseY = mouseEvent->y();

  /* if the mouse is on the grid */
  if( xPos < m && yPos < n){
    /* update title information */
    dispMouseX = xPos;
    dispMouseY = yPos;

    if (_enableMouseMove && !_pause){
      /* mouse speed */
      float velX = ((int)(mouseX - prevMouseX) * coef); // caution: signed operation
      float velY = ((int)(mouseY - prevMouseY) * coef);

      /* update fluid speed */
      if(!(fluid->_obstacles->isInObstacles(xPos,yPos))){
      fluid->_u->set (xPos, yPos,
		      fluid->_u->get(xPos, yPos) + velX);
      fluid->_v->set (xPos, yPos,
		      fluid->_v->get(xPos, yPos) - velY);
      }
    }
  }
}

/**
 * Manages the events on mouse wheel.
 *
 * @param mouseEvent Event of the mouse related to its wheel
 */
void GUI::wheelEvent(QWheelEvent *mouseEvent){
  int n = (fluid->_dens)->getSize(0);
  int m = (fluid->_dens)->getSize(1);
  int pos = mouseEvent->delta();
  coef += (float) pos/120;
  if(coef > m/20){
    coef = m/20;
  }
  else if(coef > n/20){
    coef = n/20;
  }
  else if(coef < 0){
    coef = 0.1;
  }
}

/**
 * Saves the current configuration
 */
void GUI::saveConfig(){
    QString densFile, velXFile, velYFile;
    QString densSrcFile, velXSrcFile, velYSrcFile;
    QString configName = saveWin->getText();

    /* check if the config. already exists */
    bool test = configuration.setConfig(configName);
    if(!test){
      configuration.setName(configName);
      configuration.createConfig();
    }

    /* absolute file paths */
    densFile = QCoreApplication::applicationDirPath() 
      + "/" + configName + "_density";
    velXFile = QCoreApplication::applicationDirPath() 
      + "/" + configName + "_velX";
    velYFile = QCoreApplication::applicationDirPath() 
      + "/" + configName + "_velY";
    densSrcFile = QCoreApplication::applicationDirPath() 
      + "/" + configName + "_density_src";
    velXSrcFile = QCoreApplication::applicationDirPath() 
      + "/" + configName + "_velX_src";
    velYSrcFile = QCoreApplication::applicationDirPath() 
      + "/" + configName + "_velY_src";

    /* save matrices */
    fluid->_dens->save(densFile.toStdString().c_str());
    configuration.setDensFile(densFile);

    fluid->_u->save(velXFile.toStdString().c_str());
    configuration.setVelXFile(velXFile);

    fluid->_v->save(velYFile.toStdString().c_str());
    configuration.setVelYFile(velYFile);

    fluid->_dens_src->save(densSrcFile.toStdString().c_str());
    configuration.setDensSrcFile(densSrcFile);

    fluid->_u_vel_src->save(velXSrcFile.toStdString().c_str());
    configuration.setVelXSrcFile(velXSrcFile);

    fluid->_v_vel_src->save(velYSrcFile.toStdString().c_str());
    configuration.setVelYSrcFile(velYSrcFile);

    /* update the configuration file */
    configuration.writeConfig(fluid->_obstacles->getSegList());

    /* resume the simulation */
    _pause = false;
}

/**
 * Loads a new configuration
 */
void GUI::loadConfig(){
    QString densFile, velXFile, velYFile;
    QString configName = loadWin->getText();

    /* check the validity of the config name */
    bool test = configuration.setConfig(configName);
    if(!test){
      std::cerr << "Error: invalid config name '" << configName.toStdString();
      std::cerr << "'." << std::endl;
      _pause = false;
      return;
    }
    
    /* load the new config */
    configuration.updateConfig();

    /* if files provided, load them */
    if(strcmp("",configuration.getVelXFile()) != 0)
      fluid->_u->load(configuration.getVelXFile());

    if(strcmp("",configuration.getVelYFile()) != 0)
      fluid->_v->load(configuration.getVelYFile());

    if(strcmp("",configuration.getDensFile()) != 0)
      fluid->_dens->load(configuration.getDensFile());

    if(strcmp("",configuration.getVelXSrcFile()) != 0)
      fluid->_u_vel_src->load(configuration.getVelXSrcFile());

    if(strcmp("",configuration.getVelYSrcFile()) != 0)
      fluid->_v_vel_src->load(configuration.getVelYSrcFile());

    if(strcmp("",configuration.getDensSrcFile()) != 0)
      fluid->_dens_src->load(configuration.getDensSrcFile());

    /* overwrite the obstacles */
    delete fluid->_obstacles;
    fluid->_obstacles = new Obstacles(fluid->_dens->getSize(1),fluid->_dens->getSize(0),configuration);
    
    /* resume the simulation */
      _pause = false;
}

/**
 *  Manages keyboard events.
 * 
 * @param keyEvent Event of the keybord
 */
void GUI::keyPressEvent(QKeyEvent *keyEvent){
  Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
  switch(keyEvent->key()){
  case Qt::Key_Escape:
    close();
    break;

  case Qt::Key_F1:
    toggleFullWindow();
    break;

  case Qt::Key_S:
    if(modifiers == Qt::ControlModifier){
      _pause = true;
      saveWin->show();
    }
    break;
  case Qt::Key_O:
    if(modifiers == Qt::ControlModifier){
      _pause = true;
      loadWin->show();
    }
    break;
  case Qt::Key_Enter:  // Numpad Enter
  case Qt::Key_Return: // Classic Enter
    _currentPrintMode ++;
    _currentPrintMode %= _printModes.count();
    break;

  case Qt::Key_V:
    _drawVelocityField = !_drawVelocityField;
    break;

  case Qt::Key_Backspace: // reset fluid + sources + obstacles
    fluid->_obstacles->reset();
  case Qt::Key_R:         // reset sources + obstacles
    fluid->resetSources();
  case Qt::Key_F:         // reset only the fluid
    fluid->resetFluid();
    for (int i = 0; i < _printModes.count(); i++){
      _printModes[i]->reset();
    }
    break;

  case Qt::Key_M:
   _enableMouseMove = !_enableMouseMove;
    break;

  case Qt::Key_Plus:
    coef += .2;
    break;

  case Qt::Key_Minus:
    coef -= .2;
    if (coef < .1) coef = .1;
    break;

  case Qt::Key_Space:
    _pause = !_pause;
    for (int i = 0; i < _printModes.count(); i++){
      _printModes[i]->pause();
    }
    break;

  case Qt::Key_W:
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
    if(modifiers == Qt::ControlModifier){
      close();
    }
    break;
  }
}

GUI::~GUI(){
  delete t_Timer;
  delete fluid;
  delete M1;
  delete M2;
}
