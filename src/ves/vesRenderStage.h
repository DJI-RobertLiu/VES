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

#ifndef VESRENDERSTAGE_H
#define VESRENDERSTAGE_H

// VES includes
#include "vesGMTL.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesRenderLeaf.h"

// C++ includes
#include <map>
#include <vector>

class vesRenderStage
{
public:
  typedef std::vector< vesRenderLeaf> RenderLeaves;
  typedef std::map<int, RenderLeaves> BinRenderLeavesMap;

  // \todo: Use it later.
  enum SortMode
  {
    BackToFront = 0x0,
    FrontToBack,
    SortByState
  };


  vesRenderStage()
  {
    // Do nothing as of now.
  }


 ~vesRenderStage()
  {
    this->m_binRenderLeavesMap.clear();
  }


  void addRenderLeaf(const vesRenderLeaf &renderLeaf)
  {
    this->m_binRenderLeavesMap[renderLeaf.m_bin].push_back(renderLeaf);
  }


  void sort(SortMode mode)
  {
    // \todo: Implement this.
  }


  void render(vesRenderState &renderState, vesRenderLeaf *previous)
  {
    BinRenderLeavesMap::iterator itr = this->m_binRenderLeavesMap.begin();
    RenderLeaves::iterator rlsItr;

    for (itr; itr != this->m_binRenderLeavesMap.end(); ++itr) {
      for (rlsItr = itr->second.begin(); rlsItr != itr->second.end(); ++rlsItr) {
        (*rlsItr).render(renderState, previous);

        previous = &(*rlsItr);
      }
    }
  }


  void clearAll()
  {
    this->m_binRenderLeavesMap.clear();
  }


private:

  BinRenderLeavesMap  m_binRenderLeavesMap;

  // Not implemented.
  vesRenderStage   (const vesRenderStage&);
  void operator=   (const vesRenderStage&);
};

#endif // VESRENDERSTAGE_H
