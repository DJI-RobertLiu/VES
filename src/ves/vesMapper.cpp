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

#include "vesMapper.h"

// VES includes
#include "vesMaterial.h"
#include "vesGeometryData.h"
#include "vesGLTypes.h"
#include "vesRenderData.h"
#include "vesRenderStage.h"
#include "vesShaderProgram.h"
#include "vesVertexAttributeKeys.h"

#include "vesGL.h"

// C++ includes
#include <map>
#include <vector>
#include <cstdio>

class vesMapper::vesInternal
{
public:
  vesInternal()
  {
    this->m_color.resize(4);
  }

  ~vesInternal()
  {
    this->cleanUpDrawObjects();
  }

  void setColor(const float &r, const float &g, const float &b, const float &a)
  {
    m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;
  }

  const float* color() const
  {
    return &(this->m_color.front());
  }

  void cleanUpDrawObjects()
  {
    this->m_bufferVertexAttributeMap.clear();
    this->m_buffers.clear();
  }

  std::vector< float >                       m_color;
  std::vector< unsigned int >                m_buffers;
  std::map< unsigned int, std::vector<int> > m_bufferVertexAttributeMap;
};


vesMapper::vesMapper() : vesBoundingObject(),
  m_drawPoints(false),
  m_initialized(false),
  m_maximumTriangleIndicesPerDraw
               (65535),
  m_internal   (0x0)
{
  this->m_internal = new vesInternal();

  // Default is almost white.
  this->setColor(0.9, 0.9, 0.9, 1.0);
}


vesMapper::~vesMapper()
{
  if (this->m_initialized) {
    // For now we are not checking if the context is valid.
    this->deleteVertexBufferObjects();
  }

  delete this->m_internal; this->m_internal = 0x0;
}


void vesMapper::computeBounds()
{
  vesVector3f min = this->m_geometryData->boundsMin();
  vesVector3f max = this->m_geometryData->boundsMax();

  this->setBounds(min, max);

  this->setBoundsDirty(false);
}


void vesMapper::normalize()
{
  float r = this->boundsRadius();

  this->m_normalizedMatrix =
      makeScaleMatrix4x4(1/r,1/r,1/r)*
      makeTranslationMatrix4x4(-this->boundsCenter());

  this->setBoundsCenter(transformPoint3f(this->m_normalizedMatrix, this->boundsCenter()));
  this->setBoundsSize(transformPoint3f(this->m_normalizedMatrix, this->boundsSize()));
}


bool vesMapper::setGeometryData(vesSharedPtr<vesGeometryData> geometryData)
{
  bool success = true;

  if (geometryData && this->m_geometryData != geometryData)
  {
    this->m_geometryData = geometryData;
    this->m_initialized = false;
  }
  else
  {
    success = false;
  }

  return success;
}


vesSharedPtr<vesGeometryData> vesMapper::geometryData()
{
  return this->m_geometryData;
}


const vesSharedPtr<vesGeometryData> vesMapper::geometryData() const
{
  return this->m_geometryData;
}


void vesMapper::setColor(float r, float g, float b, float a)
{
  this->m_internal->setColor(r, g, b, a);
}


float* vesMapper::color()
{
  return const_cast<float*>(this->m_internal->color());
}


const float* vesMapper::color() const
{
  return this->m_internal->color();
}


void vesMapper::render(const vesRenderState &renderState)
{
  if (!this->m_initialized) {
    this->setupDrawObjects(renderState);
  }

  if (renderState.m_material->binNumber() == vesMaterial::Overlay) {
    glDisable(GL_DEPTH_TEST);
  }

  // Fixed vertex color.
  glVertexAttrib4fv(vesVertexAttributeKeys::Color, this->color());

  std::map<unsigned int, std::vector<int> >::const_iterator constItr
    = this->m_internal->m_bufferVertexAttributeMap.begin();

  int bufferIndex = 0;
  for (; constItr != this->m_internal->m_bufferVertexAttributeMap.end();
       ++constItr) {
    glBindBuffer(GL_ARRAY_BUFFER, constItr->first);
    for (size_t i = 0; i < constItr->second.size(); ++i) {
      renderState.m_material->bindVertexData(renderState, constItr->second[i]);
    }
    ++bufferIndex;
  }

  // Try rendering our point cloud here
  if (m_drawPoints)
    drawPoints(renderState);

  unsigned int numberOfPrimitiveTypes = this->m_geometryData->numberOfPrimitiveTypes();
  for(unsigned int i = 0; i < numberOfPrimitiveTypes; ++i)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_internal->m_buffers[bufferIndex++]);

    if (this->m_geometryData->primitive(i)->primitiveType() == GL_TRIANGLES) {
      // Draw triangles
      this->drawTriangles(renderState, this->m_geometryData->primitive(i));
    }
    else {
      // Draw rest of the primitives

      // Send the primitive type information out
      renderState.m_material->bindRenderData(
        renderState, vesRenderData(this->m_geometryData->primitive(i)->primitiveType()));

      glDrawElements(this->m_geometryData->primitive(i)->primitiveType(),
                     this->m_geometryData->primitive(i)->numberOfIndices(),
                     GL_UNSIGNED_SHORT,
                     (void*)0);
    }
  }

  // Unbind.
  bufferIndex = 0;
  constItr = this->m_internal->m_bufferVertexAttributeMap.begin();
  for (; constItr != this->m_internal->m_bufferVertexAttributeMap.end();
       ++constItr) {
    for (size_t i = 0; i < constItr->second.size(); ++i) {
      renderState.m_material->unbindVertexData(renderState, constItr->second[i]);
    }
    ++bufferIndex;
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if (renderState.m_material->binNumber() == vesMaterial::Overlay) {
    glEnable(GL_DEPTH_TEST);
  }
}


void vesMapper::setupDrawObjects(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  // Delete buffer objects from past if any.
  this->deleteVertexBufferObjects();

  // Now clean up any cache related to draw objects.
  this->m_internal->cleanUpDrawObjects();

  // Now construct the new ones.
  this->createVertexBufferObjects();

  this->m_initialized = true;
}

struct vtkVertex3f
{
  vesVector3f point;
  vesVector3f normal;
  vesVector3f color;
};

void vesMapper::createVertexBufferObjects()
{
  unsigned int bufferId;

  unsigned int numberOfSources = this->m_geometryData->numberOfSources();
  for(unsigned int i = 0; i < numberOfSources; ++i)
  {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ARRAY_BUFFER, this->m_geometryData->source(i)->sizeInBytes(),
      this->m_geometryData->source(i)->data(), GL_STATIC_DRAW);

    std::vector<int> keys = this->m_geometryData->source(i)->keys();
    for(size_t j = 0; j < keys.size(); ++j) {
      this->m_internal->m_bufferVertexAttributeMap[
      this->m_internal->m_buffers.back()].push_back(keys[j]);
    }
  }

  size_t numberOfPrimitiveTypes = this->m_geometryData->numberOfPrimitiveTypes();
  for(size_t i = 0; i < numberOfPrimitiveTypes; ++i)
  {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
      this->m_geometryData->primitive(i)->sizeInBytes(),
      this->m_geometryData->primitive(i)->data(),
      GL_STATIC_DRAW);
  }

  this->m_initialized = true;
}


void vesMapper::deleteVertexBufferObjects()
{
  if (!this->m_internal->m_buffers.empty()) {
    glDeleteBuffers(this->m_internal->m_buffers.size(),
                    &this->m_internal->m_buffers.front());
  }
}


void vesMapper::drawTriangles(const vesRenderState &renderState,
                              vesSharedPtr<vesPrimitive> triangles)
{
  const unsigned int numberOfIndices
    = triangles->numberOfIndices();

  unsigned int drawnIndices = 0;

  while (drawnIndices < numberOfIndices) {
    int numberOfIndicesToDraw = numberOfIndices - drawnIndices;

    if (numberOfIndicesToDraw > this->m_maximumTriangleIndicesPerDraw) {
      numberOfIndicesToDraw = this->m_maximumTriangleIndicesPerDraw;
    }

    unsigned int offset
      = triangles->sizeOfDataType()
        * drawnIndices;

    // Send the primitive type information out
    renderState.m_material->bindRenderData(
      renderState, vesRenderData(triangles->primitiveType()));

    // Now draw the elements
    glDrawElements(triangles->primitiveType(),
                   numberOfIndicesToDraw,
                   GL_UNSIGNED_SHORT,
                   (void*)offset);


    drawnIndices += numberOfIndicesToDraw;
  }
}


void vesMapper::drawPoints(const vesRenderState &)
{
  vesSharedPtr<vesSourceData> data =
      m_geometryData->sourceData(vesVertexAttributeKeys::Position);
  glDrawArrays(GL_POINTS, 0, data->sizeOfArray());
}
