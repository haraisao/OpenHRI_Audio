// -*- C++ -*-
/*!
 * @file WavPlayerComp.cpp
 * @brief Standalone component
 * @date $Date$
 *
 * $Id$
 */

#include "WavPlayer.h"

#if defined(__linux)
static char WaveFileName[512*2]; 

class DialogWin : public Gtk::Window
{
    Gtk::Label m_label;
    Gtk::VBox m_box;
    Gtk::Button m_ok;
    Glib::ustring m_label_str;

public:
  DialogWin() 
  : m_ok("OK")
  {
    Gtk::FileChooserDialog diag( "ファイル選択", Gtk::FILE_CHOOSER_ACTION_OPEN );

    diag.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
    diag.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    switch( diag.run() ){
    case Gtk::RESPONSE_OK:
      m_label.set_text( diag.get_filename() );
      m_label_str = diag.get_filename();
      //strncpy(WaveFileName, (diag.get_filename()).c_str(), (diag.get_filename()).size());
      strncpy(WaveFileName, (diag.get_filename()).c_str(), strlen(diag.get_filename().c_str()));
      break;
    case Gtk::RESPONSE_CANCEL:
      m_label.set_text( "Cancel" );
      m_label_str= "Cancel";
      //strncpy(WaveFileName, m_filename.c_str(), m_filename.size());
      break;
    }
    //add( m_label );
    add( m_box );
    m_box.pack_start(m_label);
    m_ok.signal_clicked().connect(sigc::mem_fun(*this, &DialogWin::on_ok_clicked));
    m_box.pack_start(m_ok);
    show_all_children();
    resize( 200, 100 );  
  };

  void on_ok_clicked(){
    std::cout << "Click OK" << std::endl;
    this->hide();
  };
};
#elif defined(_WIN32)
static char WaveFileName[MAX_PATH*2]; 

int OpenDiaog(HWND hwnd,LPCSTR Filter,char *FileName,DWORD Flags)
{
   OPENFILENAME OFN; 

   ZeroMemory(&OFN,sizeof(OPENFILENAME));
   OFN.lStructSize = sizeof(OPENFILENAME); 
   OFN.hwndOwner = hwnd;
   OFN.lpstrFilter =Filter;
   OFN.lpstrFile =FileName;  
   OFN.nMaxFile = MAX_PATH*2;
   OFN.Flags = Flags;    
   OFN.lpstrTitle = "File Open";
   return (GetOpenFileName(&OFN));
}
#endif


void MyModuleInit(RTC::Manager* manager)
{
  WavPlayerInit(manager, WaveFileName);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("WavPlayer");

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

  if(argc == 1 || argv[1][0] == '-'){
#if defined(__linux)
  Gtk::Main kit(argc, argv);
  DialogWin dialogwin;
  Gtk::Main::run( dialogwin );


#elif defined(_WIN32)
  //HINSTANCE hInst = GetModuleHandle( NULL );
  HWND hwnd = GetWindow( NULL, GW_OWNER );
  OpenDiaog(hwnd,"Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0\0",
					WaveFileName,OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
  //printf("Wave File Name:%s\n", WaveFileName);

#endif
  }else{
      strncpy(WaveFileName, argv[1], strlen(argv[1]));
  }

  if (strlen(WaveFileName) == 0){
   exit(0);
  }

  printf("Wave File Name:%s\n", WaveFileName);

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
  
  manager = RTC::Manager::init(argc, argv);

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
