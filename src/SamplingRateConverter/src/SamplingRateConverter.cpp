// -*- C++ -*-
/*!
 * @file  SamplingRateConverter.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "SamplingRateConverter.h"

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "SamplingRateConverter",
    "type_name",         "SamplingRateConverter",
    "description",       "Sampling Rate Converter",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "OpenHRI",
    "activity_type",     "PERIODIC",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    
    "conf.default.InputSampleRate", "16000",
    "conf.__constraints__.InputSampleRate", "x >= 1",
    "conf.__type__.InputSampleRate", "int",
    "conf.__description__.InputSampleRate", "入力する音声のサンプリング周波数(Hz)",
    "conf.default.InputBitDepth", "16",
    "conf.__constraints__.InputBitDepth", "8,16,24,33, float32",
    "conf.__type__.InputBitDepth", "string",
    "conf.__description__.InputBitDepth", "入力する音声のビット幅",
    "conf.default.OutputSampleRate", "16000",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__type__.OutputSampleRate", "int",
    "conf.__description__.OutputSampleRate", "変換後の音声データのサンプリング周波数(Hz)",
    "conf.default.OutputBitDepth", "16",
    "conf.__constraints__.OutputBitDepth", "8,16,24,32, float32",
    "conf.__type__.OutputBitDepth", "string",
    "conf.__description__.OutputBitDepth", "変換後の音声データのビット幅",

    ""
  };
// </rtc-template>
enum {DEPTH8, DEPTH16, DEPTH24, DEPTH32, DEPTHFLOAT32};

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
SamplingRateConverter::SamplingRateConverter(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_AudioDataInIn("AudioDataIn", m_AudioDataIn),
    m_AudioDataOutOut("AudioDataOut", m_AudioDataOut),
    m_handle(NULL)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
SamplingRateConverter::~SamplingRateConverter()
{
}

RTC::ReturnCode_t SamplingRateConverter::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("SamplingRateConverter : Sampling Rate Converter"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("AudioDataIn", m_AudioDataInIn);
  m_AudioDataInIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new AudioInDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("AudioDataOut", m_AudioDataOutOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("InputSampleRate", m_InputSampleRate, "16000");
  bindParameter("InputBitDepth", m_InputBitDepth, "16");
  bindParameter("OutputSampleRate", m_OutputSampleRate, "16000");
  bindParameter("OutputBitDepth", m_OutputBitDepth, "16");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t SamplingRateConverter::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  if ( m_handle ) {
    resample_close( m_handle );
    m_handle = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SamplingRateConverter::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SamplingRateConverter::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t SamplingRateConverter::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  m_in_format = DEPTH8;
  if ( m_InputBitDepth == "8" ) {
    m_in_format = DEPTH8;
  } else if ( m_InputBitDepth == "16" ) {
    m_in_format = DEPTH16;
  } else if ( m_InputBitDepth == "24" ) {
    m_in_format = DEPTH24;
  } else if ( m_InputBitDepth == "32" ) {
    m_in_format = DEPTH32;
  } else if ( m_InputBitDepth == "float32" ) {
    m_in_format = DEPTHFLOAT32;
  } else {
    RTC_INFO(("Unsupported input format %s", m_InputBitDepth.c_str()));
  }

  m_out_format = DEPTH8;
  if ( m_OutputBitDepth == "8" ) {
    m_out_format = DEPTH8;
  } else if ( m_OutputBitDepth == "16" ) {
    m_out_format = DEPTH16;
  } else if ( m_OutputBitDepth == "24" ) {
    m_out_format = DEPTH24;
  } else if ( m_OutputBitDepth == "32" ) {
    m_out_format = DEPTH32;
  } else if ( m_OutputBitDepth == "float32" ) {
    m_out_format = DEPTHFLOAT32;
  } else {
    RTC_INFO(("Unsupported output format %s", m_OutputBitDepth.c_str()));
  }

  m_ratio = (double)m_OutputSampleRate / (double)m_InputSampleRate;
  if ( !m_handle ) {
    m_handle = resample_open(1, m_ratio, m_ratio);
  }
  is_active = true;

  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void SamplingRateConverter::RcvInBuffer(RTC::TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( is_active == true ) {
      m_mutex.lock();
    //! The queuing does input data.
    for ( int i = 0; i < (int)data.data.length(); i++ ) {
      m_queue.push_back((unsigned char)data.data[i]);  //!< enqueue
    }
      m_mutex.unlock();
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}


long *SamplingRateConverter::InputDataSet(int *length)
{
  RTC_DEBUG(("Output data start"));
  unsigned char val[4];
  int i;
  int size = (int)m_queue.size();
  long *data;

  switch ( m_in_format ) {
  case DEPTH8:
    *length = size;
    data = new long[size];
    for ( i = 0; i < size; i++ ) {
      data[i] = m_queue.front();
      m_queue.pop_front();
    }
    break;
  case DEPTH16:
    *length = size/2;
    data = new long[size/2];
    for ( i = 0; i < size/2; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      short wk = (short)val[0] + (short)(val[1] << 8);
      data[i] = wk;
    }
    break;
  case DEPTH24:
    *length = size/3;
    data = new long[size/3];
    for ( i = 0; i < size/3; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16);
      if ( wk > 8388607 ) {
        wk = wk - 16777215;
      }
      data[i] = wk;
    }
    break;
  case DEPTH32:
    *length = size/4;
    data = new long[size/4];
    for ( i = 0; i < size/4; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      val[3] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16) + (long)(val[3] << 24);

      data[i] = wk;

    }
    break;
  case DEPTHFLOAT32:
    data = NULL;
    break;
  default:
    data = NULL;
    break;
  }
  return data;
}

float *SamplingRateConverter::InputDataSetFloat(int *length)
{
  RTC_DEBUG(("Output data start"));
  unsigned char val[4];
  int i;
  int size = (int)m_queue.size();
  float *data;

  switch ( m_in_format ) {
  case DEPTH8:
    *length = size;
    data = new float[size];
    for ( i = 0; i < size; i++ ) {
      data[i] = (float)m_queue.front();
      m_queue.pop_front();
    }
    break;
  case DEPTH16:
    *length = size/2;
    data = new float[size/2];
    for ( i = 0; i < size/2; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      short wk = (short)val[0] + (short)(val[1] << 8);
      data[i] = (float)wk;
    }
    break;
  case DEPTH24:
    *length = size/3;
    data = new float[size/3];
    for ( i = 0; i < size/3; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16);
      if ( ( wk & 0x00800000 ) != 0 ) {
        wk = ~(wk & 0x007fffff);
      }
      data[i] = (float)wk;
    }
    break;
  case DEPTH32:
    *length = size/4;
    data = new float[size/4];
    for ( i = 0; i < size/4; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      val[3] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16) + (long)(val[3] << 24);
      if ( ( wk & 0x80000000 ) != 0 ) {
        wk = ~(wk & 0x7fffffff);
      }
      data[i] = (float)wk;
    }
    break;
  case DEPTHFLOAT32:
    data = NULL;
    break;
  default:
    data = NULL;
    break;
  }
  return data;
}

void SamplingRateConverter::OutputDataWriteFloat(float *data, int length)
{
  RTC_DEBUG(("Output data start"));
  int i;
  long val;

  switch ( m_out_format ) {
  case DEPTH8:
    m_AudioDataOut.data.length(length * 1);
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      if ( val < 0 ) {
        val *= -1;
      }
      if ( val > 255 ) {
        val = 255;
      }

      m_AudioDataOut.data[i] = (unsigned char)val;
    }
    break;
  case DEPTH16:
    m_AudioDataOut.data.length(length * 2);
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      if ( val > 32767 ) {
        val = 32767;
      } else if ( val < -32767 ) {
        val = -32767;
      }
      m_AudioDataOut.data[i*2]   = (unsigned char)(val & 0x000000ff);
      m_AudioDataOut.data[i*2+1] = (unsigned char)((val & 0x0000ff00) >> 8);
    }

    break;
  case DEPTH24:
    m_AudioDataOut.data.length(length * 3);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      if ( val > 8388607 ) {
        val = 8388607;
      } else if ( val < -8388607 ) {
        val = -8388607;
      }
      m_AudioDataOut.data[i*3]   = (unsigned char)(val & 0x000000ff);
      m_AudioDataOut.data[i*3+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_AudioDataOut.data[i*3+2] = (unsigned char)((val & 0x00ff0000) >> 16);
    }

    break;
  case DEPTH32:
    m_AudioDataOut.data.length(length * 4);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      m_AudioDataOut.data[i*4]   = (unsigned char)(val & 0x000000ff);
      m_AudioDataOut.data[i*4+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_AudioDataOut.data[i*4+2] = (unsigned char)((val & 0x00ff0000) >> 16);
      m_AudioDataOut.data[i*4+3] = (unsigned char)((val & 0xff000000) >> 24);
    }
    break;
  case DEPTHFLOAT32:
    break;
  }
  setTimestamp( m_AudioDataOut );
  m_AudioDataOutOut.write();
  return;
}

double SamplingRateConverter::DeprhGain(void)
{
  double wk0,wk1;
  switch ( m_in_format ) {
  case DEPTH8:
    wk0 = 255.0;
    break;
  case DEPTH16:
    wk0 = 32767.0;
    break;
  case DEPTH24:
    wk0 = 8388607.0;
    break;
  case DEPTH32:
    wk0 = 2147483520.0;
    break;
  default:
    wk0 = 1.0;
    break;
  }
  switch ( m_out_format ) {
  case DEPTH8:
    wk1 = 255.0;
    break;
  case DEPTH16:
    wk1 = 32767.0;
    break;
  case DEPTH24:
    wk1 = 8388607.0;
    break;
  case DEPTH32:
    wk1 = 2147483520.0;
    break;
  default:
    wk1 = 1.0;
    break;
  }
  wk0 = wk1/wk0;
//  if ( wk0 < 0.00001 ) wk0 = 0.004;
  return wk0;
}
void SamplingRateConverter::OutputDataWrite(long *data, int length)
{
  RTC_DEBUG(("Output data start"));
  double gain = DeprhGain();
  int i;
  long val;

  switch ( m_out_format ) {
  case DEPTH8:
    m_AudioDataOut.data.length(length * 1);
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      if ( val < 0 ) {
        val *= -1;
      }
      val = (long)(val * gain);
      if ( val > 255 ) {
        val = 255;
      }
      m_AudioDataOut.data[i] = (unsigned char)val;
    }
    break;
  case DEPTH16:
    m_AudioDataOut.data.length(length * 2);
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      val = (long)(val * gain);
      if ( val > 32767 ) {
        val = 32767;
      } else if ( val < -32767 ) {
        val = -32767;
      }
      m_AudioDataOut.data[i*2]   = (unsigned char)(val & 0x000000ff);
      m_AudioDataOut.data[i*2+1] = (unsigned char)((val & 0x0000ff00) >> 8);
    }
    break;
  case DEPTH24:
    m_AudioDataOut.data.length(length * 3);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      val = (long)(val * gain);
      val = (long)data[i];
      if ( val > 8388607 ) {
        val = 8388607;
      } else if ( val < -8388607 ) {
        val = -8388607;
      }
      m_AudioDataOut.data[i*3]   = (unsigned char)(val & 0x000000ff);
      m_AudioDataOut.data[i*3+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_AudioDataOut.data[i*3+2] = (unsigned char)((val & 0x00ff0000) >> 16);
    }
    break;
  case DEPTH32:
    m_AudioDataOut.data.length(length * 4);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      val = (long)(val * gain);
      m_AudioDataOut.data[i*4]   = (unsigned char)(val & 0x000000ff);
      m_AudioDataOut.data[i*4+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_AudioDataOut.data[i*4+2] = (unsigned char)((val & 0x00ff0000) >> 16);
      m_AudioDataOut.data[i*4+3] = (unsigned char)((val & 0xff000000) >> 24);
    }
    break;
  case DEPTHFLOAT32:
    break;
  }
  setTimestamp( m_AudioDataOut );
  m_AudioDataOutOut.write();
  return;
}

RTC::ReturnCode_t SamplingRateConverter::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  if ( m_handle ) {
    resample_close( m_handle );
    m_handle = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SamplingRateConverter::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SamplingRateConverter::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SamplingRateConverter::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t SamplingRateConverter::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if ( !m_queue.empty() ) {
    int len;
    if ( m_ratio == 1 ) {
      m_mutex.lock();
      long *data = InputDataSet(&len);
      m_mutex.unlock();
      if ( data == NULL ) return RTC::RTC_ERROR;

      OutputDataWrite(data, len);
      delete [] data;
      
    } else {

      m_mutex.lock();
      float *src = InputDataSetFloat(&len);
      m_mutex.unlock();
      if ( src == NULL )  return RTC::RTC_ERROR;
 
      int dnum = (int)(len * m_ratio);
      float *dst = new float[dnum];

      int o = 0, srcused = 0, srcpos = 0, out = 0;
      for(;;) {
        int srcBlock = len - srcpos;
        o = resample_process(  m_handle,
                               m_ratio,
                               &src[srcpos],
                               srcBlock,
                                   0,
                               &srcused,
                               &dst[out],
                               dnum - out);
        srcpos += srcused;
        if ( o >= 0 ) {
          out += o;
        }
        if ( o < 0 || (o == 0 && srcpos == len) ) {
          break;
        }
      }
      delete [] src;
      OutputDataWriteFloat(dst, dnum);
      delete [] dst;
    }

  }

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SamplingRateConverter::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SamplingRateConverter::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SamplingRateConverter::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t SamplingRateConverter::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void SamplingRateConverterInit(RTC::Manager* manager)
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
			     RTC::Create<SamplingRateConverter>,
			     RTC::Delete<SamplingRateConverter>);
  }
};
