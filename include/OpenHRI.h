// -*- C++ -*-
/*!
 * @file  OpenHRI.h
 * @author Isao Hara
 *
 * Copyright (C) 2019
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * @date  $Date$
 *
 * $Id$
 */

#ifndef __OPENHRI_H__
#define __OPENHRI_H__

#include "DescriptablePort.h"

/*!
 * for  DataListener
 */
#if RTM_MAJOR_VERSION >= 1 && RTM_MINOR_VERSION >= 2
#define _CONST
#else
#define ReturnCode void
#define _CONST const
#define NO_CHANGE
#endif

#endif // __OPENHRI_H__
