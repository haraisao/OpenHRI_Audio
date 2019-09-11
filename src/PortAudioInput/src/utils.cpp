/*

*/
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

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


char **check_rtc_config(int& argc, char **argv){
  int i=0;

  if((i=check_f_option(argc, argv)) == 0){
    char *dirname = std::getenv("OPENHRI_ROOT");
    if (dirname != NULL){
#if _WIN32
      std::string conf_file = std::string(dirname)+ std::string("\\etc\\rtc.conf");
#else
      std::string conf_file = std::string(dirname) + std::string("/etc/rtc.conf");
#endif
      if(checkFileExistence(conf_file)){
        argv = (char **)std::realloc(argv, sizeof(char **) * (argc+2));
        argv[argc++] = "-f";
        argv[argc++] = strdup((char *)conf_file.c_str());
      }
    }
  }
  return argv;
}