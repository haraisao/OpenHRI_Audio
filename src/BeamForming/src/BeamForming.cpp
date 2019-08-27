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
    "conf.__type__.ConstAngle", "int",
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

  return RTC::RTC_OK;
}

RTC::ReturnCode_t BeamForming::onDeactivated(RTC::UniqueId ec_id)
{

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
