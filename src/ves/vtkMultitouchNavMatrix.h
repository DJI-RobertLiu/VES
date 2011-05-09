//
//  vtkMultitouchNavMatrix.h
//  kiwi
//
//  Created by kitware on 5/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vtkMultitouchNavMatrix_h
#define __vtkMultitouchNavMatrix_h

#import <QuartzCore/QuartzCore.h>
#include "vtkGMTL.h"

class vtkMultitouchNavMatrix
{
public:
  vtkMultitouchNavMatrix();
  void SetWidthHeight(const unsigned int width, 
                      const unsigned int height);
  CATransform3D GetMatrix(){return currentCalculatedMatrix;}
  vtkMatrix4x4f GetMatrixGMTL(){return _matrix;}
  void Reset();

//private:
  void printCurrentCalculatedMatrix();
  void printGMTLMatrix();
  void UpdateMatrix(const float xRotation, 
                    const float yRotation, 
                    float scaleFactor, 
                    float xTranslation, 
                    float yTranslation);
  void UpdateMatrixGMTL(const float xRotation, 
                    const float yRotation, 
                    float scaleFactor, 
                    float xTranslation, 
                    float yTranslation);
  
  unsigned int _width,_height;
  vtkMatrix4x4f _matrix;
  CATransform3D currentCalculatedMatrix, testMatrix;
};
#endif //__vtkMultitouchNavMatrix_h