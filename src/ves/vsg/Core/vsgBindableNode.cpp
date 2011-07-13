// ============================================================================
/**
 * @file   vsgBindableNode.cpp
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#include "vsg/Core/vsgBindableNode.h"
// --------------------------------------------------------------------includes

// -------------------------------------------------------------------macro

// -------------------------------------------------------------------cnstr
vsgBindableNode::vsgBindableNode()
{
}

// -------------------------------------------------------------------destr
vsgBindableNode::~vsgBindableNode()
{
}
// ------------------------------------------------------------------public

SFBool vsgBindableNode::isBound()
{
  return true;
}
// -----------------------------------------------------------------private

