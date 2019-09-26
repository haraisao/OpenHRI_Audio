// -*- C++ -*-
/*!
 * @file  WavPlayer.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "WavPlayer.h"

#if defined(__linux)
static char WaveFileName[512*2]; 
#elif defined(_WIN32)
static char WaveFileName[MAX_PATH*2]; 
#endif

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "WavPlayer",
    "type_name",         "WavPlayer",
    "description",       "Play audio data from WAV file",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "component_type",    "STATIC",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    
    "conf.default.OutputSampleRate", "16000",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__type__.OutputSampleRate", "int",
    "conf.__description__.OutputSampleRate", "入力される音声データのサンプリング周波数(Hz)",
    "conf.default.ChannelNumbers", "1",
    "conf.__constraints__.ChannelNumbers", "x >= 1",
    "conf.__type__.ChannelNumbers", "int",
    "conf.__description__.ChannelNumbers", "音声データのチャンネル数",
    "conf.default.FileName", "wavrecord-default.wav",
    "conf.__type__.FileName", "string",
    "conf.default.Delay", "1",
    "conf.__constraints__.Delay", "x >= 1",
    "conf.__type__.Delay", "int",
    "conf.__description__.Delay", "Delay",

    ""
  };
// </rtc-template>

#if defined(__linux)
//nothing
#elif defined(_WIN32)
#ifdef SHARED_LIB
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
   OFN.lpstrTitle = "ファイルを開く";
   return (GetOpenFileName(&OFN));
}
#endif//SHARED_LIB
#endif//defined(_WIN32)


/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WavPlayer::WavPlayer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WavPlayer::~WavPlayer()
{
}

RTC::ReturnCode_t WavPlayer::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("WavPlayer : Play audio data from WAV file"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("OutputSampleRate", m_OutputSampleRate, "16000");
  bindParameter("ChannelNumbers", m_ChannelNumbers, "1");
  bindParameter("FileName", m_FileName, "wavrecord-default.wav");
  bindParameter("Delay", m_Delay, "1");

#if defined(__linux)
#ifdef SHARED_LIB
	Gtk::FileChooserDialog diag( "ファイル選択", Gtk::FILE_CHOOSER_ACTION_OPEN );
	// 開く、キャンセルボタン
	diag.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	diag.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	switch( diag.run() ){
	case Gtk::RESPONSE_OK:
	  strncpy(WaveFileName, (diag.get_filename()).c_str(), (diag.get_filename()).size());
	  break;
        case Gtk::RESPONSE_CANCEL:
	  strncpy(WaveFileName, m_filename.c_str(), m_filename.size());
	  break;
	}
	Gtk::MessageDialog( WaveFileName ).run();
#endif //SHARED_LIB

#elif defined(_WIN32)
#ifdef SHARED_LIB
	HWND hwnd = GetWindow( NULL, GW_OWNER );

	ZeroMemory(WaveFileName,MAX_PATH*2);
	strncpy(WaveFileName, m_filename.c_str(), m_filename.size());
	printf("m_filename.c_str: %s\n", m_filename.c_str());
	printf("m_filename.size: %d\n", m_filename.size());
	printf("Wave File Name: %s\n", WaveFileName);
	if (OpenDiaog(hwnd,"Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0\0",
					WaveFileName,OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY))
	            
	//MessageBox(hwnd,strcat(WaveFileName,"\nを選択しました。"),"情報",MB_OK);
#endif//SHARED_LIB
#endif//defined(_WIN32)

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t WavPlayer::onFinalize()
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WavPlayer::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavPlayer::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WavPlayer::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  sfinfo.samplerate = m_OutputSampleRate;
  sfinfo.channels = m_ChannelNumbers;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  try {
    sfr = sf_open(WaveFileName, SFM_READ, &sfinfo);
    RTC_INFO(("Wave File Name: %s\n", WaveFileName));

	if (sf_format_check(&sfinfo) == 0) {
		RTC_DEBUG(("invalid format"));
		RTC_INFO(("Wave file invalid format"));
		return RTC::RTC_ERROR;
	}
    if (sfr == NULL) {
      RTC_INFO(("unable to open file: %s", WaveFileName));
      return RTC::RTC_ERROR;
    }
    m_timer = coil::gettimeofday() - m_Delay;
  } catch (...) {
    RTC_WARN(("%s", "error onActivated."));
    return RTC::RTC_ERROR;
  }

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavPlayer::onDeactivated(RTC::UniqueId ec_id)
{
	sf_close( sfr );
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WavPlayer::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavPlayer::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavPlayer::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WavPlayer::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));

  coil::TimeValue now = coil::gettimeofday();
  long bufferlen = long((now - m_timer) * m_OutputSampleRate);
  if ( bufferlen <= 0 ) return RTC::RTC_OK;

  coil::sleep(m_Delay);
  m_timer = now;
  short *buffer = new short[bufferlen];
  sf_count_t count = sf_readf_short(sfr, buffer, bufferlen);

  if(count == 0){
    RTC::ExecutionContext_var ec = get_context(ec_id);
    if (!CORBA::is_nil(ec))
    {
      ec->deactivate_component(::RTC::RTObject::_duplicate(getObjRef()));
    }
  } else {
    m_AudioDataOut.data.length((unsigned long)(count * 2 ));  //!< set outport data length
    memcpy((void *)&(m_AudioDataOut.data[0]), (void *)&(buffer[0]), count *  2 );
    setTimestamp(m_AudioDataOut);
    m_AudioDataOutOut.write();
  }

  delete [] buffer;

  RTC_DEBUG(("onExecute finish"));

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WavPlayer::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavPlayer::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavPlayer::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavPlayer::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void WavPlayerInit(RTC::Manager* manager, char * wave_file_name)
  {
    int i, j;
#ifndef SHARED_LIB
    strcpy(WaveFileName, wave_file_name);
#endif
    for (i = 0; strlen(rtcomponent_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl[j] = (char *)rtcomponent_spec[j];
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
			     RTC::Create<WavPlayer>,
			     RTC::Delete<WavPlayer>);
  }
};
