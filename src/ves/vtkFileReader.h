/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkFileReader.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef vtkFileReader_H
#define vtkFileReader_H

#include "vtkMatrix.h"
#include "vtkGMTL.h"
#include <vector>

// Small struct packing a point and normal together in a vertex
// Memory layout is 3 floats for a point followed by 3 floats for a normal
struct Vertex3f
{
  vtkVector3f point;
  vtkVector3f normal;
};

#include <string>
#include <fstream>

class vtkFileReader
{
public:
  vtkFileReader(std::string fileName);
  ~vtkFileReader();

  bool Read();

  unsigned int m_numPoints;
  Vertex3f *m_points;
  vtkPoint3f min,max;
  std::vector<vtkVector3us> m_triangles;
  
private:
  std::string mFileName;
  void ComputeCenterAndRadius();
  void readPoints(std::ifstream &file, Vertex3f *v, int n);
  void readNormals(std::ifstream &file, Vertex3f *v, int n);
  void readPolygons(std::ifstream &file, std::vector<vtkVector3us>& triangleCells, int numPolygons);
};

#endif
