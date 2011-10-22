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

#ifndef VESRENDERER_H
#define VESRENDERER_H

// VES includes
#include "vesGL.h"
#include "vesGMTL.h"

// C++ includes
#include <string>

// Forward declarations
class vesActor;
class vesCamera;
class vesGroupNode;
class vesRenderStage;
class vesTexture;

class vesRenderer
{
public:
           vesRenderer();
  virtual ~vesRenderer();

  virtual void render();

  virtual void resetCamera();
  virtual void resetCameraClippingRange();
  virtual void resize(int width,int height, float scale);

  virtual void setBackgroundColor(float r, float g, float b, float a=1.0f);
  virtual void setBackground(vesTexture *background);

  virtual void addActor   (vesActor *actor);
  virtual void removeActor(vesActor *actor);

  const vesGroupNode* sceneRoot() const { return this->m_sceneRoot; }

  inline vesCamera* camera(){ return this->m_camera; }

  inline int width()   { return this->m_width;  }
  inline int height()  { return this->m_height; }

  vesVector3f computeWorldToDisplay(vesVector3f world);
  vesVector3f computeDisplayToWorld(vesVector3f display);

protected:

  virtual void updateTraverseScene();
  virtual void cullTraverseScene();

  void resetCameraClippingRange(float bounds[6]);

private:
  double m_aspect[2];
  int m_width;
  int m_height;
  float m_backgroundColor[4];

  vesCamera *m_camera;
  vesGroupNode *m_sceneRoot;

  vesRenderStage *m_renderStage;
};

#endif
