// -*- C++ -*-
/*!
 * @file  Gate.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "Gate.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "Gate",
    "type_name",         "Gate",
    "description",       "Gate",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.None", "None",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Gate::Gate(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn),
    m_GateInIn("GateIn", m_GateIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Gate::~Gate()
{
}

RTC::ReturnCode_t Gate::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("Gate : Gate"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  addInPort("GateIn", m_GateInIn);
  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Gate::onFinalize()
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Gate::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Gate::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t Gate::onActivated(RTC::UniqueId ec_id)
{
  is_gated = false;
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Gate::onDeactivated(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Gate::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Gate::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Gate::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t Gate::onExecute(RTC::UniqueId ec_id)
{
  int i;
  if (m_GateInIn.isNew()) {
    m_GateInIn.read();
    is_gated = m_GateIn.data;
  }
  if (m_AudioDataInIn.isNew()) {
    m_AudioDataInIn.read();
    m_AudioDataOut.data.length(m_AudioDataIn.data.length());
    if (is_gated == false) {
      for (i = 0; i < m_AudioDataIn.data.length(); i++) {
        m_AudioDataOut.data[i] = m_AudioDataIn.data[i];
      }
    } else {
      for (i = 0; i < m_AudioDataIn.data.length() / 2; i++) {
        m_AudioDataOut.data[2*i] = i % 2; // avoid julius zero stripping problem
        m_AudioDataOut.data[2*i+1] = 0;
      }
    }
    setTimestamp(m_AudioDataOut);
    m_AudioDataOutOut.write();
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Gate::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Gate::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Gate::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Gate::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void GateInit(RTC::Manager* manager)
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
			     RTC::Create<Gate>,
			     RTC::Delete<Gate>);
  }
};
