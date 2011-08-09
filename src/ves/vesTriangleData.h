/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vesTriangleData.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef vesTriangleData_H
#define vesTriangleData_H

#include "vesGMTL.h"
#include <vector>

// Small struct packing a point and normal together in a vertex
// Memory layout is 3 floats for a point followed by 3 floats for a normal
struct vtkVertex3f
{
  vesVector3f point;
  vesVector3f normal;
};

class vesTriangleData
{
public:
  vesTriangleData();
  std::vector<vtkVertex3f>& GetPoints() { return this->Points; }
  std::vector<vesVector3us>& GetTriangles() { return this->Triangles; }
  std::vector<vesVector2us>& GetLines() { return this->Lines; }
  vesVector3f GetMin();
  vesVector3f GetMax();
  void SetHasNormals(bool b) { this->HasNormals = b; }
  void ComputeNormals();
  void SetHasBounds(bool b) { this->HasBounds = b; }
  void ComputeBounds();
  std::vector<float>& GetPointScalars() { return this->PointScalars; }
  vesVector2f GetPointScalarRange();
protected:
  std::vector<vtkVertex3f> Points;
  std::vector<vesVector3us> Triangles;
  std::vector<vesVector2us> Lines;
  std::vector<float> PointScalars;
  vesVector3f Min;
  vesVector3f Max;
  bool HasBounds;
  bool HasNormals;
};

#endif
