/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShape.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkShape.h"

// --------------------------------------------------------------------includes
#include <vtkAppearanceNode.h>
#include <vtkGeometryNode.h>
#include "vtkPainter.h"
#include "vtkMapper.h"
// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkShapeInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkShape::vtkShape()
{
  this->Internal = new vtkShapeInternal();
}

// -----------------------------------------------------------------------destr
vtkShape::~vtkShape()
{
  delete this->Internal;
}

bool vtkShape::Read()
{
  std::cout << "Read: Shape" <<std::endl;
  GetAppearance() -> Read();
  if (GetGeometry()) 
  {
    GetGeometry() -> Read();
  }
  return true;
}

void vtkShape::Render(vtkPainter* render)
{
  render->Shape(this);
}

void vtkShape::ComputeBounds()
{
  vtkMapper* mapper = (vtkMapper*) GetGeometry();
  if(mapper)
  {
  mapper->ComputeBounds();
  SetBBoxCenter(mapper->GetMin(),mapper->GetMax());
  SetBBoxSize(mapper->GetMin(),mapper->GetMax());
  }
}



