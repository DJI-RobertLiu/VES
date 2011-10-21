/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include  "vesNode.h"
#include  "vesGroupNode.h"

vesNode::vesNode() : vesObject(),
  m_visible (true),
  m_material(0x0),
  m_isOverlayNode(false)
{
  this->m_parent = 0x0;
}


vesNode::~vesNode()
{
}


bool vesNode::setParent(vesGroupNode *parent)
{
  if (this->m_parent) {
    this->m_parent->removeChild(this);
  }

  this->m_parent = parent;

  return true;
}

