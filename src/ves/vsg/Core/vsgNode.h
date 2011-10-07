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

#ifndef __vsgNode_h
#define __vsgNode_h

# include "vsgMacro.h"
# include "vsgTypes.h"
# include <vector>

class vesVisitor;

class Painter;

class vsgGroupingNode;

class vsgNode
{
public:
           vsgNode();
  virtual ~vsgNode();

  virtual void accept(vesVisitor &visitor){}

  inline vsgGroupingNode* parent(){ return this->m_parent; }

//  virtual bool read() = 0;
//  virtual void render(Painter * render) =0;

protected:
  friend class vsgGroupingNode;

  bool setParent(vsgGroupingNode *parent);

  vsgGroupingNode *m_parent;
};

typedef vsgNode* SFNode;
typedef std::vector<SFNode> MFNode;

#endif // __vsgNode_h
