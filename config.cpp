#include "config.hpp"

#include <iostream>
#include <iomanip>
#include <QtXml>
#include <stdexcept>

Config::Config(const QString &configFile)
  : _segmentNb(0),
    pathToConfig( QCoreApplication::applicationDirPath() + "/" + configFile),
    currentConfigNumber(0)
{
  // Setting path datas
//  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
  QString dirPath = QCoreApplication::applicationDirPath() + "/";
  QFile xmlFile(pathToConfig);

  if(!xmlFile.open(QIODevice::ReadOnly)) {
    // setting  default config
    qDebug() << QString("Error opening config file : ") + pathToConfig
             << endl
             << QString("Making a default configuration file...")
             << endl;
    if(!xmlFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
      throw(std::string("Error : unable to create the configuration file"));
    }
      makeConfigFile();
  }
  
  // Fills the XML document
  else if (!_tree.setContent(&xmlFile)) {
    xmlFile.close();
    throw(std::string("Error creating the DOM document : check the configuration"
                      " file for any XML mistake and solve them, or delete the"
                      " file\n"));
  }
  xmlFile.close();

  currentConfig = _tree.documentElement().firstChildElement();
  setConfig(currentConfigNumber);
  updateConfig();
}
/** 
 * Seeks the wanted configuration in the XML, and set is as the current one 
 * using its name. It does not update the instance variables
 * (cf. updateConfig())
 *
 * @param configName Name of the configuration wanted
 */
bool Config::setConfig(const QString configName) {
  bool isNumber = false;
  unsigned int confNumber = configName.toInt(&isNumber);
  QDomElement tmpConfig;

  if (isNumber && setConfig(confNumber)) {
    return true;
  }
  tmpConfig = _tree.documentElement().firstChildElement();
  while(QString::compare(configName,QString(tmpConfig.attribute("name"))) \
        != 0){
    if(tmpConfig.isNull())
      return false;
    tmpConfig = tmpConfig.nextSiblingElement("config");
  }
  if(tmpConfig.isNull())
    return false;
  currentConfig = tmpConfig;
  return true;
}

/** 
 * Seeks the wanted configuration in the XML, and set is as the current one 
 * using its number. It does not update the instance variables
 * (cf. updateConfig())
 *
 * @param configNum Number of the configuration wanted
 */
bool Config::setConfig(const unsigned int configNum){
  unsigned int i = 0;
  if(configNum == lastConfig) {
    currentConfig = _tree.documentElement().lastChildElement();
    return true;
  }
  currentConfig = _tree.documentElement().firstChildElement();

  for(i = 0; i < configNum; i++){
    if(currentConfig.isNull())
      return false;
    currentConfig = currentConfig.nextSiblingElement();
  }
  if(currentConfig.isNull())
    return false;
  return true;
}


/**
 * Get all the configuration informations from the current configuration in the
 * XML structure, and set the instance variables accordingly. It has to be done
 * once a new configuration has been set.
 */
void Config::updateConfig() {
  _height = 
    currentConfig.attribute("height", QString("%1").arg(DEF_HEIGHT)).toInt();

  _width = 
    currentConfig.attribute("width", QString("%1").arg(DEF_WIDTH)).toInt();

  _windowSizeCoeff = 
    currentConfig.attribute("windowSizeCoeff",         
			    QString("%1").             
			    arg(DEF_WINDOWSIZECOEFF)).toInt();

  _FPS = 
    currentConfig.attribute("fps",
			    QString("%1").arg(DEF_FPS)).toInt();

  _viscosity = 
    currentConfig.attribute("viscosity", 
			    QString("%1").      
			    arg(DEF_VISCOSITY)).toFloat() * 1e-5;

  _diff = 
    currentConfig.attribute("diff",
			    QString("%1").arg(DEF_DIFF)).toFloat();

  _dt = 
    currentConfig.attribute("dt",
			    QString("%1").arg(DEF_DT)).toFloat();

  _name =
    currentConfig.attribute("name",
			    QString("noName"));
 
  _protected = 
    (currentConfig.attribute("protected",                    
			     QString("false"))               
     == QString("true"));

  if (!currentConfig.firstChildElement("description").isNull()) {
    _description = 
      currentConfig.firstChildElement("description").text().trimmed();
  }
  else {
    _description = "";
  }
  
  QDomElement currentConfigSon = currentConfig.firstChildElement("state");
  _dens_file = currentConfigSon.attribute("density", QString(""));
  _velX_file = currentConfigSon.attribute("velX", QString(""));
  _velY_file = currentConfigSon.attribute("velY", QString(""));
  _dens_src_file = currentConfigSon.attribute("density_src", QString(""));
  _velX_src_file = currentConfigSon.attribute("velX_src", QString(""));
  _velY_src_file = currentConfigSon.attribute("velY_src", QString(""));
  updateObstacles();
}


/**
 * From the current configuration, generates all the attached obstacle segments
 */
void Config::updateObstacles() {
  //segments are sons of obstacles which is a son of config
  QDomElement currentConfigSon = currentConfig.firstChildElement("obstacles");
  currentConfigSon = currentConfigSon.firstChildElement("segment");
  
  //counting of segment
  while(!(currentConfigSon.isNull())){
    _segmentNb++;
    currentConfigSon = currentConfigSon.nextSiblingElement("segment");
  }

  // stores the values
  _segmentValues = new int*[_segmentNb];
  _segmentNb = 0;
  currentConfigSon = currentConfig.firstChildElement("obstacles");
  currentConfigSon = currentConfigSon.firstChildElement("segment");

  while(!(currentConfigSon.isNull())){
    _segmentValues[_segmentNb] = new int[5];
    _segmentValues[_segmentNb][0] = 
      currentConfigSon.attribute("Ax",   QString("%1").arg(5)).toInt();
    _segmentValues[_segmentNb][1] = 
      currentConfigSon.attribute("Ay",   QString("%1").arg(8)).toInt();
    _segmentValues[_segmentNb][2] = 
      currentConfigSon.attribute("Bx",   QString("%1").arg(5)).toInt();
    _segmentValues[_segmentNb][3] = 
      currentConfigSon.attribute("By",   QString("%1").arg(16)).toInt();
    _segmentValues[_segmentNb][4] = 
      currentConfigSon.attribute("width",QString("%1").arg(5)).toInt();
    currentConfigSon = currentConfigSon.nextSiblingElement("segment");
    _segmentNb++;
  }
}


/**
 * Returns the heigth of the matrix or the screen according to visualSize.
 */
unsigned int Config::getHeight(bool visualSize) const {
  return visualSize ?  _windowSizeCoeff * _height :  _height;
}

/**
 * Returns the heigth of the width or the screen according to visualSize.
 */
unsigned int Config::getWidth(bool visualSize) const {
  return visualSize ?  _windowSizeCoeff * _width :  _width;
}

/**
 * Returns the coefficient of the resolution of the screen
 */
unsigned int Config::getCoeff() const {
  return _windowSizeCoeff;
}

/**
 * Returns the FPS of the simulation
 */
unsigned int Config::getFPS() const {
  return _FPS;
}

/**
 * Returns the viscosity of the fluid
 */
float Config::getViscosity() const{
  return _viscosity;
}

/**
 * Returns the temporal scale of the simulation
 */
float Config::getDt() const{
  return _dt;
}

/**
 * Returns the diffusion of the fluid
 */
float Config::getDiff() const{
  return _diff; 
}

/**
 * Returns the path of the file where is stored the density of the fluid
 */
const char * Config::getDensFile(){
  return _dens_file.toStdString().c_str();
}

/**
 * Returns the path of the file where is stored the X-coordinates of
 * the fluid velocity
 */
const char * Config::getVelXFile(){
    return _velX_file.toStdString().c_str();
}

/**
 * Returns the path of the file where is stored the Y-coordinates of
 * the fluid velocity
 */
const char * Config::getVelYFile(){
    return _velY_file.toStdString().c_str();
}


/**
 * Returns the path of the file where is stored the sources of density
 */
const char * Config::getDensSrcFile(){
  return _dens_src_file.toStdString().c_str();
}

/**
 * Returns the path of the file where is stored the X-coordinates of
 * the sources of velocity
 */
const char * Config::getVelXSrcFile(){
    return _velX_src_file.toStdString().c_str();
}

/**
 * Returns the path of the file where is stored the Y-coordinates of
 * the sources of velocity
 */
const char * Config::getVelYSrcFile(){
    return _velY_src_file.toStdString().c_str();
}

/**
 * Sets the height of the matrix which is going to be used
 *
 * @param h Height wanted for the matrix
 */
void Config::setHeight(const unsigned int h) {
  if (h <= 1) {
    throw(std::invalid_argument(std::string("Height should be an integer"
                                            " strictly greater than 1")));
  }
  _height = h;
}

/**
 * Sets the width of the matrix which is going to be used
 *
 * @param w Width wanted for the matrix
 */
void Config::setWidth(const unsigned int w) {
  if(w <= 1) {
    throw(std::invalid_argument(std::string("Width should be an integer strictly"
                                            " greater than 1")));
  }
  _width = w;
}

/**
 * Sets the coefficient for the window size
 *
 * @param c Coefficient wanted for window
 */
void Config::setCoeff(const unsigned int c) {
  if(c == 0) {
    throw(std::invalid_argument(std::string("Coefficient for the window size" 
                                            " should be an integer strictly"
                                            " greater than" 
                                            "0")));
  }
  _windowSizeCoeff = c;
}

/**
 * Sets the number of frame per second for the simulation
 *
 * @param f Number of FPS wanted for the simulation
 */
void Config::setFPS(const unsigned int f) {
  if (f == 0) {
    throw(std::invalid_argument(std::string("FPS should be an integer strictly"
                                            " greater than 0")));
  }
  _FPS = f;
}

/**
 * Sets the viscosity of the fluid
 *
 * @param v Viscosity wanted for the fluid
 */
void Config::setViscosity(const float v) {
  _viscosity = v * 1e-5; 
  // the coefficient is there to avoid dealing with very little values
     
}

/**
 * Sets the temporal scale of the simulation
 *
 * @param d Temporal scale wanted for the simulation
 */
void Config::setDt(const float d) {
  _dt = d;
}

/**
 * Sets the diffusion of the fluid
 *
 * @param d diffusion wanted for the fluid
 */
void Config::setDiff(const float d) {
  _diff = d; 
}

/**
 * Sets the name of the current configuration
 *
 * @param name Name going to be used for the current configuration
 */
void Config::setName(QString name){
  _name = name;
}

/**
 * Protection is here to prevent any unintented modification of the
 * configuration, for instance when an interesting phenomenon has been found.
 * Nonetheless, it is possible to "switch off" the protection and modify it.
 *
 * @param protection True in order to protect the configuration
 */
void Config::setProtection(const bool protection) {
  _protected = protection;
}

/**
 * Sets the adress of the file where the density of the fluid
 * is going to be stored.
 *
 * @param file Adress of the file where the data are going to be stored
 */
void Config::setDensFile(QString file){
  _dens_file = file;
}

/**
 * Sets the adress of the file where the X-coordinates of the fluid velocity
 * is going to be stored.
 *
 * @param file Adress of the file where the data are going to be stored
 */
void Config::setVelXFile(QString file){
  _velX_file = file;
}

/**
 * Sets the adress of the file where the Y-coordinates of the fluid velocity
 * is going to be stored.
 *
 * @param file Adress of the file where the data are going to be stored
 */
void Config::setVelYFile(QString file){
  _velY_file = file;
}


/**
 * Sets the adress of the file where the density of the fluid
 * is going to be stored.
 *
 * @param file Adress of the file where the data are going to be stored
 */
void Config::setDensSrcFile(QString file){
  _dens_src_file = file;
}

/**
 * Sets the adress of the file where the X-coordinates of the fluid velocity
 * is going to be stored.
 *
 * @param file Adress of the file where the data are going to be stored
 */
void Config::setVelXSrcFile(QString file){
  _velX_src_file = file;
}

/**
 * Sets the adress of the file where the Y-coordinates of the fluid velocity
 * is going to be stored.
 *
 * @param file Adress of the file where the data are going to be stored
 */
void Config::setVelYSrcFile(QString file){
  _velY_src_file = file;
}

/**
 * It takes the configuration values from the instance variables and put them
 * into the XML. It has to been done to save configurations on the disk. If a
 * protection is set, changes will only be effective in memory.
 *
 * @param segList List of the segment constituting the obstacles
 */
void Config::writeConfig(std::list<Segment*> segList) {
  QDomElement newConfig;

  currentConfig.setAttribute("height", _height);
  currentConfig.setAttribute("width",_width);
  currentConfig.setAttribute("windowSizeCoeff", _windowSizeCoeff);
  currentConfig.setAttribute("fps", _FPS);
  currentConfig.setAttribute("viscosity", _viscosity);
  currentConfig.setAttribute("dt", _dt);
  currentConfig.setAttribute("diff", _diff);

  if (_protected) {
    currentConfig.setAttribute("protected", "true" );
  }
  else {
    currentConfig.setAttribute("protected", "false");
  }

  QDomElement currentConfigSon = currentConfig.firstChildElement("state");
  if(!(currentConfigSon.isNull())){
    currentConfigSon.setAttribute("density",_dens_file);
    currentConfigSon.setAttribute("velX",_velX_file);
    currentConfigSon.setAttribute("velY",_velY_file);
    currentConfigSon.setAttribute("density_src",_dens_src_file);
    currentConfigSon.setAttribute("velX_src",_velX_src_file);
    currentConfigSon.setAttribute("velY_src",_velY_src_file);
  }
  else{
    //creation of a new node named state
    newConfig = _tree.createElement("state");
    newConfig.setAttribute("density",_dens_file);
    newConfig.setAttribute("velX",_velX_file);
    newConfig.setAttribute("velY",_velY_file);
    newConfig.setAttribute("density_src",_dens_src_file);
    newConfig.setAttribute("velX_src",_velX_src_file);
    newConfig.setAttribute("velY_src",_velY_src_file);
    currentConfig.appendChild(newConfig);
  }

  currentConfigSon = currentConfig.firstChildElement("obstacles");
  if(currentConfigSon.isNull()){
    //creation of a new node named obstacles
    newConfig = _tree.createElement("obstacles");
    currentConfig.appendChild(newConfig);
  }

  currentConfig = currentConfig.firstChildElement("obstacles");
  currentConfigSon = currentConfig.firstChildElement("segment");

  //the current segments are deleted from the current configuratio
  while(!(currentConfigSon.isNull())){
    currentConfig.removeChild(currentConfigSon);
    currentConfigSon = currentConfig.firstChildElement("segment");
  }

  //creation of the new node of segments
  std::list<Segment*>::iterator iter;
  for(iter = segList.begin(); iter != segList.end(); iter++){
    newConfig = _tree.createElement("segment");
    newConfig.setAttribute("Ax",(*iter)->getA0());
    newConfig.setAttribute("Ay",(*iter)->getA1());
    newConfig.setAttribute("Bx",(*iter)->getB0());
    newConfig.setAttribute("By",(*iter)->getB1());
    newConfig.setAttribute("width",(*iter)->getLength());
    currentConfig.appendChild(newConfig);
  }

  if(!_protected) {
    QFile config (pathToConfig);
    config.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream configStream(&config);
    
    configStream << _tree.toString(2); // indentation = 2
    config.close();
    std::cerr << "Configurations written" << std::endl;
  }
  else {
    std::cerr << "Selected configuration is protected, nothing will change on" 
      "disk" << std::endl;
  }
}


/**
 * Lists every configuration, with details, in the terminal.
 */
void Config::listConfigs() {
  using namespace std;
    if(_tree.documentElement().firstChildElement("config").isNull()) {
    cout << "\tAucune configuration sauvegardée." << endl;
    exit(EXIT_SUCCESS);
  }


  cout << setfill(' ')
       << setw(6)  << left << "Num."
       << setw(12)  << "Nom"
       << setw(8) << "Haut."
       << setw(8) << "Larg."
       << setw(6) << "Coef."
       << setw(8) << "Visc."
       << setw(8) << "Dt"
       << setw(8) << "Diff."
       << setw(6) << "Prot."
       << endl;

  bool notFinish = true;
  for (int i = 0; notFinish ; i++) {
    cout << setw(6) << left << i
         << setw(12)  << _name.toStdString()
         << setw(8) << _height
         << setw(8) << _width
         << setw(6) << _windowSizeCoeff
         << setw(8) << _viscosity
         << setw(8) << _dt
         << setw(8) << _diff
         << setw(6) << ( _protected ? "true" : "false")
         << endl;
    if (_description != "") {
      cout << "\t" << _description.toStdString() << endl;
    }
    else {
      cout << endl;;
    }
    notFinish = setConfig(i + 1);
    updateConfig();
  }
  exit(EXIT_SUCCESS);
}

/**
 * Sets the default configuration
 */
void Config::makeDefaultConfig() {
  _height = DEF_HEIGHT;
  _width = DEF_WIDTH;
  _windowSizeCoeff = DEF_WINDOWSIZECOEFF;
  _FPS = DEF_FPS;
  _viscosity = DEF_VISCOSITY;
  _dt = DEF_DT;
  _diff = DEF_DIFF;
  _name =  QString("default");
}

/**
 * Append a new configuration to the file
 */
void Config::createConfig() {
  //makeConfig();
  currentConfig = _tree.createElement("config");
  currentConfig.setAttribute("name", _name);
  currentConfig.setAttribute("height", _height);
  currentConfig.setAttribute("width", _width);
  currentConfig.setAttribute("windowSizeCoeff", _windowSizeCoeff);
  currentConfig.setAttribute("fps", _FPS);
  currentConfig.setAttribute("viscosity", _viscosity);
  currentConfig.setAttribute("dt", _dt);
  if(_tree.firstChildElement().appendChild(currentConfig).isNull()) {
    std::cerr << "Failed to create the new configuration." << std::endl;
  }
  else {
    std::cerr << "Configuration created" << std::endl;
  }
}

void Config::deleteConfig() {
  if(currentConfig.isNull()) {
    std::cerr << "No configuration to remove." << std::endl;
  }
  else {
  _tree.documentElement().removeChild(currentConfig);
    writeConfig();
  }
}

/**
 * Creates a default configuration file
 */
void Config::makeConfigFile() {
  makeDefaultConfig();
  QDomElement elem;
  elem = _tree.createElement("configs");
  _tree.appendChild(elem);
  createConfig();
  writeConfig();
}

Config::~Config(){
  delete[] _segmentValues;
}
