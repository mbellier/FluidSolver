#ifndef CONFIG_HPP
#define CONFIG_HPP

#define DEF_HEIGHT 130
#define DEF_WIDTH 130
#define DEF_WINDOWSIZECOEFF 3
#define DEF_FPS 60
#define DEF_VISCOSITY 1
#define DEF_DT 1
#define DEF_DIFF 0
#define DEF_PROTECTION false
#define DEF_SEGMENTNB 0

#include <QtXml>
#include "./solver/Segment.hpp"

/**
 * Class storing all the parameters about the fluid and the simulation
 */
class Config {
public:
  Config(const QString &configFile = QString("configs.xml"));
  ~Config();

  unsigned int getHeight(bool visualSize = false) const;
  unsigned int getWidth(bool visualSize = false) const;
  unsigned int getCoeff() const;
  unsigned int getFPS() const;
  float getViscosity() const;
  float getDt() const;
  float getDiff() const;

  const char *getDensFile();
  const char *getVelXFile();
  const char *getVelYFile();
  const char *getDensSrcFile();
  const char *getVelXSrcFile();
  const char *getVelYSrcFile();

  inline int getSegmentNb(){return _segmentNb;}
  inline int **getSegmentValues(){return _segmentValues;}

  void setHeight(const unsigned int height = DEF_HEIGHT) ;
  void setWidth(const unsigned int width = DEF_WIDTH) ;
  void setCoeff(const unsigned int coeff = DEF_WINDOWSIZECOEFF) ;
  void setFPS(const unsigned int fps = DEF_FPS) ;
  void setViscosity(const float viscosity = DEF_VISCOSITY) ;
  void setDt(const float dt = DEF_DT) ;
  void setDiff(const float diff = DEF_DIFF);
  void setProtection(const bool protection = DEF_PROTECTION);
  void setName(QString name);

  void setDensFile(QString);
  void setVelXFile(QString);
  void setVelYFile(QString);
  void setDensSrcFile(QString);
  void setVelXSrcFile(QString);
  void setVelYSrcFile(QString);

  void updateConfig();
  void createNewConfig(unsigned int height, unsigned int width,         \
                       unsigned int windowSizeCoeff, unsigned int fps,  \
                       float viscosity, float dt);
  //void deleteConfig(unsigned int confNumber);
  void listConfigs();
  bool setConfig(const QString confName = QString("0"));
  bool setConfig(const unsigned int confNumber = 0);
  void writeConfig(std::list<Segment*> segList = std::list<Segment*>());
  void createConfig();
  void deleteConfig();
  void makeDefaultConfig();
  
  static const unsigned int lastConfig = -1; 
private: 
  unsigned int _height;
  unsigned int _width;
  unsigned int _windowSizeCoeff;
  unsigned int _FPS;
  float _viscosity;
  float _dt;
  float _diff;
  int _segmentNb;
  int **_segmentValues;
  bool _protected;
  QString _description;
  QString _name;
  QString _dens_file;
  QString _velX_file;
  QString _velY_file;
  QString _dens_src_file;
  QString _velX_src_file;
  QString _velY_src_file;

  QString pathToConfig;
  QString currentConfigName;
  unsigned int currentConfigNumber;
  QDomElement currentConfig;
  QDomNode currentNode;
  QDomDocument _tree;
  
 
  bool isNotReadable();
  void updateObstacles();
  void makeConfigFile();
};


#endif // CONFIG_HPP
