// -*- C++ -*-
/*!
 * @file  ChannelSelector.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "ChannelSelector.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "ChannelSelector",
    "type_name",         "ChannelSelector",
    "description",       "Channel Selector",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.InputChannelNumbbers", "4",
    "conf.__constraints__.InputChannelNumbbers", "x >= 1",
    "conf.__type__.InputChannelNumbbers", "int",
    "conf.__description__.InputChannelNumbbers", "Number of input audio channels",
    "conf.default.SelectOutputChannel", "1",
    "conf.__constraints__.SelectOutputChannel", "x >= 1",
    "conf.__type__.SelectOutputChannel", "int",
    "conf.__description__.SelectOutputChannel", "Select output audio channel(ch1～)",
    "conf.default.InputSampleByte", "int32",
    "conf.__constraints__.InputSampleByte", "int8,int16,int24,int32",
    "conf.__type__.InputSampleByte", "string",
    "conf.__description__.InputSampleByte", "Sample byte of auduio capture",
    "conf.default.ReverseEndian", "OFF",
    "conf.__constraints__.ReverseEndian", "ON,OFF",
    "conf.__type__.ReverseEndian", "string",
    "conf.__description__.ReverseEndian", "Reverse endian",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ChannelSelector::ChannelSelector(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ChannelSelector::~ChannelSelector()
{
}

RTC::ReturnCode_t ChannelSelector::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("ChannelSelector : Channel Selector"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioDataDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("InputChannelNumbbers", m_InputChannelNumbbers, "4");
  bindParameter("SelectOutputChannel", m_SelectOutputChannel, "1");
  bindParameter("InputSampleByte", m_InputSampleByte, "int32");
  bindParameter("ReverseEndian", m_ReverseEndian, "OFF");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ChannelSelector::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ChannelSelector::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChannelSelector::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t ChannelSelector::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  if ( m_InputSampleByte == "int8" ) {
    m_bytesize = 1;
  } else if ( m_InputSampleByte == "int16" ) {
    m_bytesize = 2;
  } else if ( m_InputSampleByte == "int24" ) {
    m_bytesize = 3;
  } else if ( m_InputSampleByte == "int32" ) {
    m_bytesize = 4;
  } else {
    m_bytesize = 0;
  }

  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void ChannelSelector::RcvInBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( ( is_active == true ) && ( m_SelectOutputChannel <= m_InputChannelNumbbers ) && (m_bytesize != 0) ) {
    //! The queuing does input data.
    int skip = m_bytesize * m_InputChannelNumbbers;
    int cnt = 0;
    int length = (int)data.data.length();
//    std::cout << "input byte size = " << length << std::endl;
    m_AudioDataOut.data.length(length / m_InputChannelNumbbers);  //!< set outport data length
    for ( int i = m_bytesize * ( m_SelectOutputChannel - 1); i < length; i = i + skip ) {
      if ( m_ReverseEndian == "ON" ) {
        for ( int j = 0; j < m_bytesize; j++ ) {
          m_AudioDataOut.data[cnt + j] = data.data[i + (m_bytesize - 1) - j];
        }
      } else {
        memcpy((void *)&(m_AudioDataOut.data[cnt]), (void *)&(data.data[i]), m_bytesize);
      }
      cnt += m_bytesize;
    }
    setTimestamp( m_AudioDataOut );
    m_AudioDataOutOut.write();
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}


RTC::ReturnCode_t ChannelSelector::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ChannelSelector::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChannelSelector::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChannelSelector::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t ChannelSelector::onExecute(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ChannelSelector::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChannelSelector::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChannelSelector::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChannelSelector::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void ChannelSelectorInit(RTC::Manager* manager)
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
			     RTC::Create<ChannelSelector>,
			     RTC::Delete<ChannelSelector>);
  }
};
