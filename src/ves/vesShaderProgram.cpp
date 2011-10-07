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

#include "vesShaderProgram.h"

// VES includes
#include "vesShader.h"
#include "vesUniform.h"
#include "vesVertexAttribute.h"

// C++ includes
#include <vector>


vesShaderProgram::vesShaderProgram() : vesMaterialAttribute()
{
  this->m_type = Shader;
  this->m_programHandle = 0;
}


vesShaderProgram::~vesShaderProgram()
{
  this->cleanUp();
}


bool vesShaderProgram::addShader(vesShader *shader)
{
  if (!shader)
    return false;

  // \todo: Memory management.
  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {

    if (shader == *it)
      return false;

    if ((*it)->shaderType() == shader->shaderType()) {
      this->m_shaders.erase(it);
      break;
    }
  }

  this->m_shaders.push_back(shader);

  // \todo: Implement this and make its state dirty.
  // shader->addProgramReference(this);

  return true;
}


bool vesShaderProgram::addUniform(vesUniform *uniform)
{
  if (!uniform)
  {
    return false;
  }

  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (constItr; constItr != this->m_uniforms.end(); ++constItr)
  {
    if (uniform == *constItr)
      return false;
  }

  this->m_uniforms.push_back(uniform);

  // \todo: Make it modified or dirty.
}


bool vesShaderProgram::addVertexAttribute(vesVertexAttribute *attribute, int key)
{
  if (!attribute)
  {
    return false;
  }

  this->m_vertexAttributes[key] = attribute;

  // \todo: Make it modified or dirty.
}


bool vesShaderProgram::addBindAttributeLocation(const std::string &name,
                                                int location)
{
  this->m_vertexAttributeNameToLocation[name] = location;

  // \todo: Make it modified or dirty.
}


int vesShaderProgram::uniformLocation(const std::string &name) const
{
  UniformNameToLocation::const_iterator constItr =
    this->m_uniformNameToLocation.find(name);

  if (constItr != this->m_uniformNameToLocation.end()) {
    return constItr->second;
  }
  else {
    return -1;
  }
}


int vesShaderProgram::attributeLocation(const std::string &name) const
{
  VertexAttributeNameToLocation::const_iterator constItr =
    this->m_vertexAttributeNameToLocation.find(name);

  if (constItr != this->m_vertexAttributeNameToLocation.end()) {
    return constItr->second;
  }
  else {
    return -1;
  }
}


void vesShaderProgram::use()
{
  glUseProgram(this->m_programHandle);
}


int vesShaderProgram::queryUniformLocation(const std::string &value)
{
  return glGetUniformLocation(this->m_programHandle, value.c_str());
}


int vesShaderProgram::queryAttributeLocation(const std::string &value)
{
  return glGetAttribLocation(this->m_programHandle, value.c_str());
}


void vesShaderProgram::deleteProgram()
{
  if (this->m_programHandle) {
    glDeleteProgram(this->m_programHandle);
    this->m_programHandle = 0;
  }
}


bool vesShaderProgram::link()
{
  GLint status;

  glLinkProgram(this->m_programHandle);

  GLint logLength;
  glGetProgramiv(this->m_programHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->m_programHandle, logLength, &logLength, log);
    std::cerr  << "Program link log:" << std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->m_programHandle, GL_LINK_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


bool vesShaderProgram::validate()
{
  GLint logLength, status;

  glValidateProgram(this->m_programHandle);
  glGetProgramiv(this->m_programHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->m_programHandle, logLength, &logLength, log);
    std::cerr << "Program validate log:" <<std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->m_programHandle, GL_VALIDATE_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


void vesShaderProgram::bindAttributes()
{
  std::map<int, vesVertexAttribute*>::const_iterator constItr =
    this->m_vertexAttributes.begin();

  int i=0;
  for (;constItr != this->m_vertexAttributes.end(); ++constItr) {
    glBindAttribLocation(this->m_programHandle, i,
                         constItr->second->name().c_str());
    this->m_vertexAttributeNameToLocation[constItr->second->name()] = i;
    ++i;
  }
}

void vesShaderProgram::bindUniforms()
{
  std::list<vesUniform*>::const_iterator constItr = this->m_uniforms.begin();
  for (;constItr != this->m_uniforms.end(); ++constItr)
  {
    this->m_uniformNameToLocation[(*constItr)->name()] =
      queryUniformLocation((*constItr)->name());
  }
}


void vesShaderProgram::cleanUp()
{
  this->deleteVertexAndFragment();

  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {
    delete (*it);
  }

  this->deleteProgram();
}


void vesShaderProgram::deleteVertexAndFragment()
{
  // Delete a shader object.
  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {
    glDeleteShader((*it)->shaderHandle());
  }
}


vesUniform* vesShaderProgram::uniform(const std::string &name)
{
  std::list<vesUniform*>::iterator itr =
    this->m_uniforms.begin();

  for (; itr != this->m_uniforms.end(); ++itr)
  {
    if (((*itr)->name().compare(name)) == 0)
    {
      return *itr;
    }
  }

  return 0;
}


bool vesShaderProgram::uniformExist(const std::string &name)
{
  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (; constItr != this->m_uniforms.end(); ++constItr)
  {
    if (((*constItr)->name().compare(name)) == 0)
    {
      return true;
    }
  }

  return false;
}


void vesShaderProgram::updateUniforms()
{
  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (constItr; constItr != this->m_uniforms.end(); ++constItr)
  {
    (*constItr)->callGL(this->m_uniformNameToLocation[(*constItr)->name()]);
  }
}


void vesShaderProgram::setup(const vesRenderState &renderState)
{
}


void vesShaderProgram::bind(const vesRenderState &renderState)
{
  if (!this->m_programHandle) {
    this->m_programHandle = glCreateProgram();

    if (this->m_programHandle == 0)
    {
      std::cerr << "ERROR: Cannot create Program Object" <<std::endl;
      return;
    }

    // Compile shaders.
    for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
         it!=this->m_shaders.end(); ++it) {
      std::cerr << "INFO: Compiling shaders: " << std::endl;

      (*it)->compileShader();

      (*it)->attachShader(this->m_programHandle);
    }

    this->bindAttributes();

    // link program
    if (!this->link()) {
      std::cerr << "ERROR: Failed to link Program" << std::endl;
      this->cleanUp();
    }

    this->use();

    this->bindUniforms();
  }
  else
  {
    this->use();
  }

  // Call update callback.
  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (constItr; constItr != this->m_uniforms.end(); ++constItr) {
    (*constItr)->update(renderState, *this);
  }

  // Now update values to GL.
  this->updateUniforms();
}



void vesShaderProgram::unbind(const vesRenderState &renderState)
{
  // \todo: Implement this.
}


void vesShaderProgram::setupVertexData(const vesRenderState &renderState, int key)
{
  std::map<int, vesVertexAttribute*>::const_iterator constItr =
    this->m_vertexAttributes.find(key);

  if (constItr != this->m_vertexAttributes.end()) {
    this->m_vertexAttributes[key]->setupVertexData(renderState, key);
  }
}


void vesShaderProgram::bindVertexData(const vesRenderState &renderState, int key)
{
  std::map<int, vesVertexAttribute*>::const_iterator constItr =
    this->m_vertexAttributes.find(key);

  if (constItr != this->m_vertexAttributes.end()) {
    this->m_vertexAttributes[key]->bindVertexData(renderState, key);
  }
}


void vesShaderProgram::unbindVertexData(const vesRenderState &renderState, int key)
{
  std::map<int, vesVertexAttribute*>::const_iterator constItr =
    this->m_vertexAttributes.find(key);

  if (constItr != this->m_vertexAttributes.end()) {
    this->m_vertexAttributes[key]->bindVertexData(renderState, key);
  }
}

