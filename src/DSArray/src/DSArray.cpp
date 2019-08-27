// -*- C++ -*-
/*!
 * @file  DSArray.cpp
 * @author Isao Hara(isao-hara@aist.go.jp)
 *
 * Copyright (C) 
 *     All rights reserved.
 *
 */

#include "DSArray.h"

// use speex's internal fft functions
extern "C" {
  void *spx_fft_init(int size);
  void spx_fft_destroy(void *table);
  void spx_fft_float(void *table, float *in, float *out);
  void spx_ifft_float(void *table, float *in, float *out);
}

// Module specification
// <rtc-template block="module_spec">
const char* rtcomponent_spec[] =
  {
    "implementation_id", "DSArray",
    "type_name",         "DSArray",
    "description",       "DS Array",
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
    "conf.__description__.SampleRate", "入力音声データのサンプリング周波数(Hz)",
    "conf.default.ChannelNumbers", "4",
    "conf.__constraints__.ChannelNumbers", "x >= 2",
    "conf.__type__.ChannelNumbers", "int",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
DSArray::DSArray(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_micIn("mic", m_mic),
    m_resultOut("result", m_result)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
DSArray::~DSArray()
{
}

int DSArray::ccf(short *base, short *data)
{
  int i;
  int cnt = 0;
  float max = 0;
  float *out0 = new float[WINLEN];
  float *out1 = new float[WINLEN];
  float *fft0 = new float[WINLEN*2];
  float *fft1 = new float[WINLEN*2];
  for ( i = 0; i < WINLEN; i++ ) {
    out0[i] = (float)base[i];
    out1[i] = (float)data[i];
  }
  ApplyWindowFloat(WINLEN, window, out0);
  ApplyWindowFloat(WINLEN, window, out1);
  spx_fft_float(fft, out0, fft0);
  spx_fft_float(fft, out1, fft1);
  for ( i = 0; i < WINLEN*2; i++ ) {
    fft0[i] = fft0[i] * fft1[i];
  }
  spx_ifft_float(fft, fft0, out0);

  for ( i = 0; i < SEARCHMAX; i++ ) {
    if ( max < out0[i] ) {
      max = out0[i];
      cnt = i;
    }
  }
  delete[] out0;
  delete[] out1;
  delete[] fft0;
  delete[] fft1;
  return cnt;
}

int DSArray::CrossCorrelation(short *base, short *data)
{
  int i,j;
  int cnt = SEARCHMAX;
  long max = WINLEN*2*700;
  long sum[SEARCHMAX];
  for ( i = 0; i < SEARCHMAX; i++ ) {
    sum[i] = 0;
    for ( j = 0; j+SEARCHMAX < WINLEN; j++ ) {
      sum[i] += abs(base[j] + data[i+j]);
    }
	if ( max < sum[i] ) {
      max = sum[i];
      cnt = i;
    }
  }
//  std::cout << "max = " << max <<  std::endl;
  return cnt;
}

RTC::ReturnCode_t DSArray::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("DSArray : DS Array"));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set DataPort buffers
  
  addInPort("mic", m_micIn);
  m_micIn.addConnectorDataListener(ON_BUFFER_WRITE, 
      new MicDataListener("ON_BUFFER_WRITE", this), false); 

  addOutPort("result", m_resultOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  
  bindParameter("SampleRate", m_SampleRate, "16000");
  bindParameter("ChannelNumbers", m_ChannelNumbers, "4");


  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}


RTC::ReturnCode_t DSArray::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DSArray::onStartup(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DSArray::onShutdown(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t DSArray::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  m_horizon = true;
  int i = 0;
  BufferClr();

  m_micinfo = new mic_info[m_ChannelNumbers];
  char fname[] = "micset.csv";
  std::ifstream stream(fname);
  if ( stream.is_open() ) {
    float wk_x,wk_y,wk_z;
    double dwk;
    char str[256];
    while ( stream.getline( str, 256 ) ) {
      if ( str[0] == '#' ) continue;
      if ( sscanf( str, "%f,%f,%f", &wk_x, &wk_y, &wk_z ) == 3 ) {
        m_micinfo[i].x = (double)wk_x;
        m_micinfo[i].y = (double)wk_y;
        m_micinfo[i].z = (double)wk_z;
	if (( wk_y != 0.0 ) || ( wk_z != 0.0 )) {
          m_horizon = false;
        }
        dwk = sqrt( wk_x * wk_x + wk_y * wk_y );
        if ( m_micinfo[i].x < 0 ) {
          if ( m_micinfo[i].y < 0) {
            m_micinfo[i].xy_rad = acos( (double)wk_y/dwk ) * -1;
          } else {
            m_micinfo[i].xy_rad = asin( (double)wk_x/dwk );
           }
        } else {
          m_micinfo[i].xy_rad = acos( (double)wk_y/dwk );
        }
        dwk = sqrt( wk_y * wk_y + wk_z * wk_z );
        if ( m_micinfo[i].y < 0 ) {
          if ( m_micinfo[i].z < 0 ) {
            m_micinfo[i].yz_rad = acos( (double)wk_z/dwk ) * -1;
          } else {
            m_micinfo[i].yz_rad = asin( (double)wk_y/dwk );
          }
        } else {
          m_micinfo[i].yz_rad = acos( (double)wk_z/dwk );
        }
      }
      RTC_INFO(("mic %i angle = %f [deg]", i, m_micinfo[i].xy_rad * 180 / M_PI));
      std::cout << "mic " << i << " : (" << m_micinfo[i].x << "," << m_micinfo[i].y << "," << m_micinfo[i].z << ") angle " << m_micinfo[i].xy_rad * 180 / M_PI <<  "[deg]" << std::endl;
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
  window = CreateWindowFloat(WINLEN, Hamming);
//  windowd = CreateWindowDouble(WINLEN, Hamming);
  fft = spx_fft_init(WINLEN);
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}


void DSArray::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
  if ( fft != NULL ) {
    spx_fft_destroy(fft);
    fft = NULL;
  }
  if ( !m_data.empty() ) {
    m_data.clear(); //!< queue buffer clear
  }
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void DSArray::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    int length = data.data.length();
    short wk;
    unsigned char wk0, wk1;
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_data.push_back(wk);
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t DSArray::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  try {
    is_active = false;
    BufferClr();
    delete[] m_micinfo;
  } catch (...) {
    RTC_WARN(("%s", "onDeactivated error"));
  }
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DSArray::onAborting(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DSArray::onError(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DSArray::onReset(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t DSArray::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if ( (int)m_data.size() > ( m_ChannelNumbers * WINLEN ) ) {
//  if ( ( !m_data.empty() ) && ( m_data.size() > ( m_ChannelNumbers * WINLEN ) ) ) {
    m_mutex.lock();
    double dwk;
    double angle,angle0,angle1,deg;
    int cnt = 0;
    int i,j = 0;
    int dt,dtmin,micnum;
    short **buffer = new short*[m_ChannelNumbers];

    for ( i = 0; i < m_ChannelNumbers; i++ ) {
      buffer[i] = new short[WINLEN];
    }

    for ( i = 0; i < WINLEN; i++ ) {
      for ( j = 0; j < m_ChannelNumbers; j++ ) {
        buffer[j][i] = m_data.front();
        m_data.pop_front();
      }
    }
    m_mutex.unlock();

    deg = 0;
    cnt = 0;
    dtmin = SEARCHMAX;
    for ( i = 0; i < m_ChannelNumbers; i++ ) {
      angle0 = m_micinfo[i].xy_rad;

//      for ( j = i + 1; j < m_ChannelNumbers; j++ ) {
      for ( j = 0; j < m_ChannelNumbers; j++ ) {
        if ( i == j ) continue;
        angle1 = m_micinfo[j].xy_rad;
        if ( fabs( angle0 ) > fabs( angle1 ) ) {
          angle = angle0;
        } else {
          angle = angle1;
        }
/*        dt = ccf(buffer[i], buffer[j]);*/
        dt = CrossCorrelation(buffer[i], buffer[j]);
//        if ( ( dt == 0 ) || ( dt == SEARCHMAX ) ) continue;
        if ( dt == SEARCHMAX ) continue;
        if ( dtmin > dt ) {
          dtmin = dt;
          micnum = j;
        }
//        std::cout << "mic = " << i << " - " << j << " dt = " << dt << std::endl;
        /* 理論値：ch0=47,ch1=13,ch2=0,ch3=13,ch4=47 */
        dwk = sqrt(pow((m_micinfo[i].x - m_micinfo[j].x), 2.0) + pow((m_micinfo[i].y - m_micinfo[j].y), 2.0));
        dwk = dt * SONIC / dwk / m_SampleRate;
        if ( (dwk > 1 ) || ( dwk < -1 ) ) continue;
        angle = asin(dwk);

        if ( ( angle0 == 0 ) && ( angle1 == 0 ) ) {
          if ( m_micinfo[i].x < m_micinfo[j].x ) angle *= -1;
        } else {
          if ( ( abs(angle0) > M_PI/2 ) || ( abs(angle1) > M_PI/2 ) ) {
            if ( ( angle0 > 0 ) && ( angle1 < 0 ) ) {
              angle1 = angle1 + 2*M_PI;
            } else if ( ( angle0 < 0 ) && ( angle1 > 0 ) ) {
              angle1 = angle1 - 2*M_PI;
            }
          }
          if ( angle0 < angle1 ) {
            angle = angle0 + (angle1 - angle0)/2 - angle;
          } else {
            angle = angle1 + (angle0 - angle1)/2 + angle;
          }
        }
//		std::cout << "mic = " << i << " - " << j << ": dt = " << dt << ", angle = " << angle * 180 / M_PI << "[°]" << std::endl;
        deg += angle;
        cnt++;
      }
    }
//    if ( ( micnum <  m_ChannelNumbers - 1 ) && ( m_horizon == true ) ) {
//      deg *= -1;
//    }
//    if ( cnt > 2 ) {
    if ( cnt > 0 ) {
      deg = deg / cnt;
      deg = deg * 180 / M_PI;
      std::cout << " angle = " << deg << "[°]" << std::endl;
      m_result.data = deg;
      setTimestamp( m_result );
      m_resultOut.write();
    }

    for ( i = 0; i < m_ChannelNumbers; i++ ) {
      delete[] buffer[i];
    }
    delete[] buffer;
  }

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DSArray::onStateUpdate(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DSArray::onRateChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DSArray::onAction(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DSArray::onModeChanged(RTC::UniqueId ec_id)
{

  return RTC::RTC_OK;
}
*/



extern "C"
{
  void DSArrayInit(RTC::Manager* manager)
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
			     RTC::Create<DSArray>,
			     RTC::Delete<DSArray>);
  }
};
