#include <QApplication>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "GUI.hpp"
#include "SimplePrint.hpp"
#include "ColorPrint.hpp"
#include "CurvePrint.hpp"
#include "ParticlesPrint.hpp"
#include "../config.hpp"
#include "LeapMotion.h"

/*
 * Prints the informations about the command line
 *
 * @param s String which is going to be printed
 */
void usage (char *s){

  using namespace std;

  cout << left << setfill(' ') << "Usage : " << s << endl;
  cout << setw(35) << "\t[-config <name | number>]"
       << setw(38) << right << "(set the selected config)"
       << left << endl;
  cout << setw(35) << "\t[-newconfig]"
       << setw(38) << right << "(create a new config)" << left << endl;
  cout << setw(35) << "\t[-r     <(integer) X resolution>" << endl;
  cout << setw(35) << "\t\t<(integer) Y resolution>]" << endl;
  cout << setw(35) << "\t[-scale <(integer) window scale>]" << endl;
  cout << setw(35) << "\t[-fps   <(integer) FPS limit>]" << endl;
  cout << setw(35) << "\t[-dt    <(float) time precision>]" << endl;
  cout << setw(35) << "\t[-visc  <(float) viscosity>]" << endl;
  cout << setw(35) << "\t[-diff  <(float) diffusion>]" << endl;
  cout << setw(35) << "\t[-p     <(int) number of particles>]" << endl;
  cout << setw(35) << "\t[-vectors]" << setw(38) << right
       << "(display velocity field)"
       << left << endl;
  cout << setw(35) <<  "\t[-nomouse]" << setw(38)
       << right << "(disable mouse velocity modification)" << left << endl;
  cout << setw(35) << "\t[-nosimulation]" << setw(38) 
       << right << "(disable simulation)" << left << endl;
  cout << setw(35) << "\t[-save]"
       << setw(38) << right << "(save changes on disk)" << left << endl;
  cout << setw(35) << "\t[-remove]"
       << setw(38) << right << "(remove configuration)" << left << endl;
  cout << setw(35) << "\t[-lconfig]" << setw(38)
       << right << "(list all configurations)" << left <<  endl;
  cout << setw(35) << "\t[-h | --help]" << setw(38) << right << "(display this)"
       << left << endl;
}

/**
 * Checks the number of parameters of an option
 */
inline void check_nb_params(int arg, int argc, char **argv, int n){
  if (arg + n >= argc){
    std::cerr << "Error: Argument '" << argv[arg] << "' requires " << n
              << " parameters." << std::endl;
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
}

#define ARG_IS(arg_name) (strcmp((argv[arg]+1), arg_name) == 0)

int main(int argc, char *argv[]){
  QApplication app(argc, argv);

  /* * *  default config * * */
  bool drawVelocityField = false;
  bool enableMouseMove = true;
  Config *configuration = NULL;
  bool write = false;
  bool execute = true;
  unsigned int nbParticles = 500;
  try {
    configuration = new Config();
  }
  catch(const std::string &error) {
    std::cerr << "Error : " << error << std::endl;
    exit(EXIT_FAILURE);
  }

  /* * * arguments parsing * * */

  try {
    for (int arg = 1; arg < argc; arg++) {
      if (argv[arg][0] != '-'){
        std::cerr << "Error: wrong parameter provided : '" << argv[arg] << "'."
                  << std::endl;
        usage(argv[0]);
        exit(EXIT_FAILURE);
      }
      // r:
      if (ARG_IS("r")){
        check_nb_params(arg, argc, argv,2);
        configuration->setHeight(atoi(argv[arg + 1]));
        configuration->setWidth(atoi(argv[arg+2]));
        arg+=2; // skip the next 2 parameters
      }
      // dt:
      else if (ARG_IS("dt")){
        check_nb_params(arg, argc, argv, 1);
        configuration->setDt(atof(argv[arg+1]));
        arg++;
      }
      // visc
      else if (ARG_IS("visc")){
        check_nb_params(arg, argc, argv, 1);
        configuration->setViscosity(atof(argv[arg+1]));
        arg++;
      }
      // diff
      else if (ARG_IS("diff")){
        check_nb_params(arg, argc, argv, 1);
        configuration->setDiff(atof(argv[arg+1]));
        arg++;
      }
      // scale
      else if (ARG_IS("scale")){
        check_nb_params(arg, argc, argv, 1);
        configuration->setCoeff(atoi(argv[arg+1]));
        arg++;
      }
      // fps
      else if (ARG_IS("fps")){
        check_nb_params(arg, argc, argv, 1);
        configuration->setFPS( atoi(argv[arg+1]));
        arg++;
      }
      // p
      else if (ARG_IS("p")){
        check_nb_params(arg, argc, argv, 1);
        nbParticles = atoi(argv[arg+1]);
        arg++;
      }
      // vectors
      else if (ARG_IS("vectors")){
        drawVelocityField = true;
      }
      // help
      else if (ARG_IS("h") || ARG_IS("-help")){
        usage(argv[0]);
        exit(EXIT_SUCCESS);
      }
      // nomouse
      else if (ARG_IS("nomouse")){
        enableMouseMove = false;
      }
      else if (ARG_IS("nosimulation")) {
        execute = false;
      }
      else if (ARG_IS("save")) {
          write = true;
        }
      else if(ARG_IS("remove")) {
          configuration->deleteConfig();
          exit(EXIT_SUCCESS);
        }
      // configuration number
      else if(ARG_IS("config")) {
        if(arg == 1) {
          configuration->setConfig(QString(argv[arg+1]));
          configuration->updateConfig();
        }
      
        else {
          std::cerr << "Error : config should be first argument" << std::endl;
          exit(EXIT_FAILURE);
        }
          arg++;
      }
      else if(ARG_IS("newconfig")) {
          if(arg == 1) {
            configuration->createConfig();
            configuration->setConfig(Config::lastConfig);
          }
          else {
            std::cerr << "Error : newconfig should be first argument"
                      << std::endl;
            exit(EXIT_FAILURE);
          }
        }
      // configurations list
      else if(ARG_IS("lconfig")) {
          configuration->listConfigs();
          arg++;
        }
      else if(ARG_IS("reset")) {
        configuration->makeDefaultConfig();
        arg++;

      }
      //default:
      else {
        std::cerr << "Error: wrong argument '" << argv[arg] << "'.\n"
                  << std::endl;
        usage(argv[0]);
        exit(EXIT_FAILURE);
      }
    }
  }
  catch (const std::invalid_argument &argument) {
    std::cerr << "Invalid argument : " << argument.what() << std::endl;
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }


//    if(write) {
//      configuration->writeConfig();
//    }

    if (!execute) {
      delete configuration;
      return EXIT_SUCCESS;
    }
    //Print *p = new SimplePrint();
    Print *p1 = new ColorPrint(true); // enable antialiasing
    Print *p2 = new CurvePrint();
    Print *p3 = new ParticlesPrint(nbParticles,
				    configuration->getWidth(),
				    configuration->getHeight(),
				    true,   // antialiasing
				    true);  // enable trail

    GUI* myWin = new GUI(NULL, "Fluid",                                 \
                         p1, *configuration,
                         drawVelocityField, enableMouseMove);

    myWin->addPrintMode(p2);
    myWin->addPrintMode(p3);

    myWin->show();

    int ret = app.exec();
    delete p1;
    delete p2;
    delete p3;
    delete myWin;
    delete configuration;
    return ret;
  }


