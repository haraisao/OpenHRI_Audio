// -*- C++ -*-
/*!
 * @file  BeamForming.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "BeamForming.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "BeamForming",
    "type_name",         "BeamForming",
    "description",       "Beam Forming",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.SampleRate", "16000",
    "conf.__constraints__.SampleRate", "x >=1",
    "conf.__type__.SampleRate", "int",
    "conf.__description__.SampleRate", "入力音声データのサンプリング周波数(Hz)",
    "conf.default.ConstAngle", "0",
    "conf.__constraints__.ConstAngle", "0 <= x <360",
    "conf.__type__.ConstAngle", "double",
    "conf.__description__.ConstAngle", "固定角度モードにおけるビームを向ける角度の設定 (deg)",
    "conf.default.Mode", "Const",
    "conf.__constraints__.Mode", "Const, Variable",
    "conf.__type__.Mode", "string",
    "conf.__description__.Mode", "ビームを固定角度に向けるか可変角度に向けるかの設定",
    "conf.default.ChannelNumbers", "8",
    "conf.__constraints__.ChannelNumbers", "x >= 2",
    "conf.__type__.ChannelNumbers", "int",
    "conf.__description__.ChannelNumbers", "入力する音声データのチャンネル数",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
BeamForming::BeamForming(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_micIn("mic", m_mic),
    m_angleIn("angle", m_angle),
    m_resultOut("result", m_result)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
BeamForming::~BeamForming()
{
}

RTC::ReturnCode_t BeamForming::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("BeamForming : Beam Forming"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("mic", m_micIn);
  m_micIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new MicDataListener("ON_BUFFER_WRITE", this), false); 

  addInPort("angle", m_angleIn);
  addOutPort("result", m_resultOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("SampleRate", m_SampleRate, "16000");
  bindParameter("ConstAngle", m_ConstAngle, "0");
  bindParameter("Mode", m_Mode, "Const");
  bindParameter("ChannelNumbers", m_ChannelNumbers, "8");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t BeamForming::onFinalize()
{
  is_active = false;
  BufferClr();
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t BeamForming::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BeamForming::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t BeamForming::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  int i = 0;
  is_active = true;
  m_horizon = true;

  m_micinfo = new mic_info[m_ChannelNumbers];
  char fname[] = "micset.csv";
  std::ifstream stream(fname);
  if ( stream.is_open() )
  {
    float wk_x,wk_y,wk_z;
//    double wk_rad;
    char str[256];
    while ( stream.getline( str, 256 ) ) {
      if ( str[0] == '#' ) continue;
      if ( sscanf( str, "%f,%f,%f", &wk_x, &wk_y, &wk_z) == 3 ) {
        m_micinfo[i].used = true;
        m_micinfo[i].dt = 0;
        m_micinfo[i].x = (double)wk_x;
        m_micinfo[i].y = (double)wk_y;
        m_micinfo[i].z = (double)wk_z;
        if (( wk_y != 0.0 ) || ( wk_z != 0.0 )) {
          m_horizon = false;
        }
        if ( m_micinfo[i].x < 0 ) {
          if ( m_micinfo[i].y < 0 ) {
            m_micinfo[i].xy_rad = acos((double)wk_y) * -1;
          } else {
            m_micinfo[i].xy_rad = asin((double)wk_x);
          }
        } else {
          m_micinfo[i].xy_rad = acos((double)wk_y);
        }
        if ( m_micinfo[i].y < 0 ) {
          if ( m_micinfo[i].z < 0 ) {
            m_micinfo[i].yz_rad = acos((double)wk_z) * -1;
          } else {
            m_micinfo[i].yz_rad = asin((double)wk_y);
          }
        } else {
          m_micinfo[i].yz_rad = acos((double)wk_z);
        }
      }
      i++;
      if ( i >= m_ChannelNumbers ) break;
    }
    stream.close();
  }
  if ( m_horizon == true ) {
    for ( i = 0; i < m_ChannelNumbers; i++ ) {
      m_micinfo[i].xy_rad = 0;
      m_micinfo[i].yz_rad = 0;
    }
  }
  DelayFunc();
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}


void BeamForming::DelayFunc(void)
{
  int i = 0;
  double wk_rad;
  for ( i = 0; i < m_ChannelNumbers; i++ ) {
    if ( m_horizon == true ) {
      wk_rad = m_ConstAngle * M_PI / 180;
      double dt = sin(wk_rad) * m_micinfo[i].x;
      dt = dt / SONIC;
      dt = dt * m_SampleRate;
      m_micinfo[i].dt = (int)dt;
    } else {
      wk_rad = abs(m_micinfo[i].xy_rad - (m_ConstAngle * M_PI / 180));
      if ( wk_rad > M_PI ) {
        wk_rad = (2 * M_PI) - wk_rad;
      }

      if ( wk_rad < M_PI/2 ) {
        double brank = sqrt(pow(m_micinfo[i].x, 2.0) + pow(m_micinfo[i].y, 2.0));
        double dt = (brank / cos(wk_rad) - brank) * m_SampleRate / SONIC;
        m_micinfo[i].dt = (int)dt;
      } else {
        m_micinfo[i].used = false;
      }
    }
  }
  return;
}

void BeamForming::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
//  delete [] m_micinfo;
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void BeamForming::RcvBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    int i,j = 0;
    int length = data.data.length();
    int size = length / ( 2 * m_ChannelNumbers );
    if ( size < 1 ) return;

    short **buffer = new short*[m_ChannelNumbers];
    for ( i = 0; i < m_ChannelNumbers; i++ ) {
      buffer[i] = new short[size];
    }

    short wk;
    unsigned char wk0, wk1;
    for ( i = 0; i < size; i++) {
      for ( j = 0; j < m_ChannelNumbers; j++) {
        wk0 = (unsigned char)data.data[i*2];
        wk1 = (unsigned char)data.data[i*2+1];
        wk = (short)(wk1 << 8) + (short)wk0;
        buffer[j][i] = wk;
      }
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));

    int cnt = 0;
    short *result = new short[size];
    for ( i = 0; i < size; i++) {
      double dwk = 0.0;
      for ( j = 0; j < m_ChannelNumbers; j++) {
        if ( m_micinfo[j].used == false ) continue;
        cnt = m_micinfo[j].dt + i;
        if ( ( cnt < 0 ) || ( cnt >= size ) ) continue;
        dwk += buffer[j][cnt];
      }
//      result[i] = (short)( dwk / ( m_ChannelNumbers / 2 ) );
      result[i] = (short)( dwk );
    }
    for ( i = 0; i < m_ChannelNumbers; i++ ) {
      delete [] buffer[i];
    }
    delete [] buffer;
    m_result.data.length(size * 2); //!< set outport data length

    for (i = 0; i < size; i++) {
      m_result.data[i*2] = (unsigned char)(result[i] & 0x00ff);
      m_result.data[i*2+1] = (unsigned char)((result[i] & 0xff00) >> 8);
    }
    setTimestamp( m_result );
    m_resultOut.write();
    delete [] result;
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t BeamForming::onDeactivated(RTC::UniqueId ec_id)
{
  try {
    is_active = false;
    delete [] m_micinfo;
    BufferClr();
  } catch (...) {
    RTC_WARN(("%s", "onDeactivated error"));
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t BeamForming::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BeamForming::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BeamForming::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t BeamForming::onExecute(RTC::UniqueId ec_id)
{
  if( m_Mode == "Variable" ) {
    m_angleIn.read();
    m_ConstAngle = m_angle.data;
    DelayFunc();
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t BeamForming::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BeamForming::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BeamForming::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t BeamForming::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void BeamFormingInit(RTC::Manager* manager)
  {
    int i, j;

    for (i = 0; strlen(rtcomponent_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl[j] = (char *)rtcomponent_spec[j];
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
			     RTC::Create<BeamForming>,
			     RTC::Delete<BeamForming>);
  }
};
