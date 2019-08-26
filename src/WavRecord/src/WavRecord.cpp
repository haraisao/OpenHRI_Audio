// -*- C++ -*-
/*!
 * @file  WavRecord.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "WavRecord.h"

#if defined(__linux)
static char WaveFileName[512*2]; 
#elif defined(_WIN32)
static char WaveFileName[MAX_PATH*2]; 
#endif

#define SAMPLE_RATE 44100
#define BIT_DEPTH 16

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "WavRecord",
    "type_name",         "WavRecord",
    "description",       "Record audio data to WAV file",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.SampleRate", "16000",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__type__.SampleRate", "int",
    "conf.__description__.SampleRate", "入力される音声データのサンプリング周波数(Hz)",
    "conf.default.FileName", "wavrecord-default.wav",
    "conf.__type__.FileName", "string",
    "conf.__description__.FileName", "録音したデータを保存するファイル名",
    "conf.default.ChannelNumbers", "1",
    "conf.__constraints__.ChannelNumbers", "x >= 1",
    "conf.__type__.ChannelNumbers", "int",
    "conf.__description__.ChannelNumbers", "入力される音声データのチャンネル数",

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
WavRecord::WavRecord(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WavRecord::~WavRecord()
{
}

RTC::ReturnCode_t WavRecord::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("WavRecord : Record audio data to WAV file"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new WabRecordDataListener("ON_BUFFER_WRITE", this), false); 


  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("SampleRate", m_SampleRate, "16000");
  bindParameter("FileName", m_FileName, "wavrecord-default.wav");
  bindParameter("ChannelNumbers", m_ChannelNumbers, "1");

#if defined(__linux)
#ifdef SHARED_LIB
	Gtk::FileChooserDialog diag( "ファイル選択", Gtk::FILE_CHOOSER_ACTION_SAVE );
	// 開く、キャンセルボタン
	diag.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	diag.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	switch( diag.run() ){
	case Gtk::RESPONSE_OK:
	  strncpy(WaveFileName, (diag.get_filename()).c_str(), (diag.get_filename()).size());
	  break;
        case Gtk::RESPONSE_CANCEL:
	  strncpy(WaveFileName, m_FileName.c_str(), m_FileName.size());
	  break;
	}
	Gtk::MessageDialog( WaveFileName ).run();
#endif //SHARED_LIB
#elif defined(_WIN32)
#ifdef SHARED_LIB
	HWND hwnd = GetWindow( NULL, GW_OWNER );

	ZeroMemory(WaveFileName,MAX_PATH*2);
	strncpy(WaveFileName, m_FileName.c_str(), m_FileName.size());
	OpenDiaog(hwnd,"Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0\0",
					WaveFileName,OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);
#endif//SHARED_LIB
#endif//defined(_WIN32)

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t WavRecord::onFinalize()
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WavRecord::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavRecord::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WavRecord::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  RTC_INFO(("Wave File Name: %s\n", WaveFileName));
  sfinfo.samplerate = m_SampleRate;
  sfinfo.channels = m_ChannelNumbers;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  if (sf_format_check(&sfinfo) == 0) {
    RTC_DEBUG(("invalid format"));
    return RTC::RTC_ERROR;
  }
  sfw = sf_open(WaveFileName, SFM_WRITE, &sfinfo);
  if (sfw == NULL) {
    RTC_DEBUG(("unable to open file: %s", WaveFileName));
    return RTC::RTC_ERROR;
  }
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void WavRecord::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if (is_active == true) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    sf_count_t frames = data.data.length();
    sf_write_raw(sfw, &data.data[0], frames);
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t WavRecord::onDeactivated(RTC::UniqueId ec_id)
{
  is_active = false;
  sf_close(sfw);
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WavRecord::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavRecord::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavRecord::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WavRecord::onExecute(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WavRecord::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavRecord::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavRecord::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WavRecord::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void WavRecordInit(RTC::Manager* manager, char * wave_file_name)
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
			     RTC::Create<WavRecord>,
			     RTC::Delete<WavRecord>);
  }
};
