/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkRenderer.mm
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vtkRenderer.h"
#include "vtkFileReader.h"
#include "vtkCamera.h"
#include "vtkShaderProgram.h"

#include <iostream>
#include <string>

//vtkRenderer::vtkRenderer(vtkShaderProgram* program, vktCamera* camera, vtkActor *actor)
//{
//  mProgram = program;
//  mCamera = camera;
//  mActor = actor;
//  once = true;
//}

vtkRenderer::vtkRenderer(vtkCamera* camera)
{
  this->mCamera = camera;
}
vtkRenderer::~vtkRenderer()
{
  if(mActor)
  {
    delete mActor;
  }
}
void vtkRenderer::SetProgram(vtkShaderProgram* program)
{
  this->mProgram = program;
}

void vtkRenderer::SetActor(vtkActor* actor)
{
  this->mActor = actor;
  once = true;
}

void vtkRenderer::Read()
{
  CopyCamera2Model();
  this->mActor->Read();
  _view = makeTranslationMatrix4x4(vtkVector3f(0,0,2))* makeScaleMatrix4x4(.1,.1,.1);
  resize(_width,_height,1);
}

void vtkRenderer::resize(int width, int height, float scale)
{
  if(width > 0 && height >0){
    _width = width;
    _height = height;
  }
  const GLfloat nearp = .1, farp = 10, fov = 90*M_PI/360.0*scale*.5;
  float aspect,left,right,bottom,top;
  if(_width > _height) {
    aspect = _width/_height;
    top = tan(fov) * nearp;
    bottom = -top;
    left = aspect * bottom;
    right = aspect * top;
  }else{
    aspect = _height/width;
    right = tan(fov) * nearp;
    left = -right;
    bottom = aspect * left;
    top = aspect * right;
  }
  _proj= makeOrthoMatrix4x4(left, right, bottom, top, nearp, farp);
  //_proj= makePerspectiveMatrix4x4(left, right, bottom, top, nearp, farp);
  
  glViewport(0, 0, width, height);

  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);
}

void vtkRenderer::resetView()
{
  _model = vtkMatrix4x4f();
  mCamera->Reset();	
}

void vtkRenderer::CopyCamera2Model()
{
#if GMTL_CAMERA
  _model = mCamera->GetMatrixGMTL();
#else
  _model.mData[0] = (GLfloat)mCamera->GetMatrix().m11;
	_model.mData[1] = (GLfloat)mCamera->GetMatrix().m12;
	_model.mData[2] = (GLfloat)mCamera->GetMatrix().m13;
	_model.mData[3] = (GLfloat)mCamera->GetMatrix().m14;
	_model.mData[4] = (GLfloat)mCamera->GetMatrix().m21;
	_model.mData[5] = (GLfloat)mCamera->GetMatrix().m22;
	_model.mData[6] = (GLfloat)mCamera->GetMatrix().m23;
	_model.mData[7] = (GLfloat)mCamera->GetMatrix().m24;
	_model.mData[8] = (GLfloat)mCamera->GetMatrix().m31;
	_model.mData[9] = (GLfloat)mCamera->GetMatrix().m32;
	_model.mData[10] = (GLfloat)mCamera->GetMatrix().m33;
	_model.mData[11] = (GLfloat)mCamera->GetMatrix().m34;
	_model.mData[12] = (GLfloat)mCamera->GetMatrix().m41;
	_model.mData[13] = (GLfloat)mCamera->GetMatrix().m42;
	_model.mData[14] = (GLfloat)mCamera->GetMatrix().m43;
	_model.mData[15] = (GLfloat)mCamera->GetMatrix().m44;
#endif  
}
void vtkRenderer::Render()
{
  this->Read();
  
  // Work out the appropriate matrices
  vtkMatrix4x4f mvp;
  //mvp = _proj * _view * _model * (*mActor)();
  vtkMatrix4x4f mv;
  mv = _view * _model * mActor->Eval();
  mvp = _proj * mv;
	
  vtkMatrix3x3f normal_matrix = makeNormalMatrix3x3f(makeTransposeMatrix4x4(makeInverseMatrix4x4 (mv)));
  //vtkMatrix4x4f temp = makeNormalizedMatrix4x4(makeTransposeMatrix4x4(_vie);
  vtkPoint3f lightDir = vtkPoint3f(0.0,0.0,.650);
	
  vtkVector3f light(lightDir.mData[0],lightDir.mData[1],lightDir.mData[2]);
  this->mProgram->SetUniformMatrix4x4f("u_mvpMatrix",mvp);
  this->mProgram->SetUniformMatrix3x3f("u_normalMatrix",normal_matrix);
  this->mProgram->SetUniformVector3f("u_ecLightDir",light);

  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // Enable our attribute arrays
  this->mProgram->EnableVertexArray("a_vertex");
  this->mProgram->EnableVertexArray("a_normal");

  if (mActor) {
    mActor->Render(this->mProgram);
  }

  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  this->mProgram->DisableVertexArray("a_vertex");
  this->mProgram->DisableVertexArray("a_normal");
}
