// -*- C++ -*-
/*!
 * @file  WebRTCVAD.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "WebRTCVAD.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "WebRTCVAD",
    "type_name",         "WebRTCVAD",
    "description",       "VAD using WebRTC",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.FileterLength", "5",
    "conf.__constraints__.FileterLength", "x >= 1",
    "conf.__type__.FileterLength", "int",
    "conf.__description__.FileterLength", "Filter length to smooth voice detection result.",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WebRTCVAD::WebRTCVAD(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AuidoDataInIn("AuidoDataIn", m_AuidoDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WebRTCVAD::~WebRTCVAD()
{
}

RTC::ReturnCode_t WebRTCVAD::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("WebRTCVAD : VAD using WebRTC"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AuidoDataIn", m_AuidoDataInIn);
  m_AuidoDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("FileterLength", m_FileterLength, "5");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t WebRTCVAD::onFinalize()
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WebRTCVAD::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WebRTCVAD::onActivated(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

RTC::ReturnCode_t WebRTCVAD::onDeactivated(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WebRTCVAD::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t WebRTCVAD::onExecute(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WebRTCVAD::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WebRTCVAD::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void WebRTCVADInit(RTC::Manager* manager)
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
			     RTC::Create<WebRTCVAD>,
			     RTC::Delete<WebRTCVAD>);
  }
};
