// -*- C++ -*-
/*!
 * @file SamplingRateConverterComp.cpp
 * @brief Standalone component
 * @date $Date$
 *
 * $Id$
 */

#include "SamplingRateConverter.h"


void MyModuleInit(RTC::Manager* manager)
{
  SamplingRateConverterInit(manager);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("SamplingRateConverter");

  if (comp==NULL)
  {
    std::cerr << "Component create failed." << std::endl;
    abort();
  }

  return;
}

int check_f_option(int argc, char ** argv){
  for(int i=0; i < argc; i++){
    if(!strcmp(argv[i], "-f")){
      return i;
    }
  }
  return 0;
}

bool checkFileExistence(const std::string& str)
{
    std::ifstream ifs(str);
    return ifs.is_open();
}

/*
    M A I N
 */
int main (int argc, char** argv)
{
  RTC::Manager* manager;

  manager = RTC::Manager::init(argc, argv);
  int i=0;

  if((i=check_f_option(argc, argv)) == 0){
#if _WIN32
    std::string conf_file = std::string(std::getenv("OPENHRI_ROOT")) + std::string("\\rtc.conf");
#else
    std::string conf_file = std::string(std::getenv("OPENHRI_ROOT")) + std::string("/rtc.conf");
#endif
    if(checkFileExistence(conf_file)){ 
      argv = (char **)std::realloc(argv, argc+2);
      argv[argc++] = "-f";
      argv[argc++] = (char *)conf_file.c_str();
    }
  }
  
  // Initialize manager
  manager->init(argc, argv);

  // Set module initialization proceduer
  // This procedure will be invoked in activateManager() function.
  manager->setModuleInitProc(MyModuleInit);

  // Activate manager and register to naming service
  manager->activateManager();

  // run the manager in blocking mode
  // runManager(false) is the default.
  manager->runManager();

  // If you want to run the manager in non-blocking mode, do like this
  // manager->runManager(true);

  return 0;
}
