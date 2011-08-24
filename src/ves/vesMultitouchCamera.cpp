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

#include "vesMultitouchCamera.h"

// --------------------------------------------------------------------includes
#include "Painter.h"

// -----------------------------------------------------------------------macro

// -----------------------------------------------------------------------cnstr
vesMultitouchCamera::vesMultitouchCamera()
{
#ifndef ANDROID
  currentCalculatedMatrix.m11 = (CGFloat)_matrix.mData[0];
  currentCalculatedMatrix.m12 = (CGFloat)_matrix.mData[1];
  currentCalculatedMatrix.m13 = (CGFloat)_matrix.mData[2];
  currentCalculatedMatrix.m14 = (CGFloat)_matrix.mData[3];
  currentCalculatedMatrix.m21 = (CGFloat)_matrix.mData[4];
  currentCalculatedMatrix.m22 = (CGFloat)_matrix.mData[5];
  currentCalculatedMatrix.m23 = (CGFloat)_matrix.mData[6];
  currentCalculatedMatrix.m24 = (CGFloat)_matrix.mData[7];
  currentCalculatedMatrix.m31 = (CGFloat)_matrix.mData[8];
  currentCalculatedMatrix.m32 = (CGFloat)_matrix.mData[9];
  currentCalculatedMatrix.m33 = (CGFloat)_matrix.mData[10];
  currentCalculatedMatrix.m34 = (CGFloat)_matrix.mData[11];
  currentCalculatedMatrix.m41 = (CGFloat)_matrix.mData[12];
  currentCalculatedMatrix.m42 = (CGFloat)_matrix.mData[13];
  currentCalculatedMatrix.m43 = (CGFloat)_matrix.mData[14];
  currentCalculatedMatrix.m44 = (CGFloat)_matrix.mData[15];
#endif
}

// -----------------------------------------------------------------------destr
vesMultitouchCamera::~vesMultitouchCamera()
{
}

bool vesMultitouchCamera::Read()
{
  return true;
}

void vesMultitouchCamera::Render(Painter* render)
{

}

void vesMultitouchCamera::SetWidthHeight(const unsigned int width,
                                         const unsigned int height)
{

}

vesMatrix4x4f vesMultitouchCamera::GetMatrix()
{
  vesMatrix4x4f _model;
#ifndef ANDROID
  _model.mData[0] = (float)currentCalculatedMatrix.m11;
  _model.mData[1] = (float)currentCalculatedMatrix.m12;
  _model.mData[2] = (float)currentCalculatedMatrix.m13;
  _model.mData[3] = (float)currentCalculatedMatrix.m14;
  _model.mData[4] = (float)currentCalculatedMatrix.m21;
  _model.mData[5] = (float)currentCalculatedMatrix.m22;
  _model.mData[6] = (float)currentCalculatedMatrix.m23;
  _model.mData[7] = (float)currentCalculatedMatrix.m24;
  _model.mData[8] = (float)currentCalculatedMatrix.m31;
  _model.mData[9] = (float)currentCalculatedMatrix.m32;
  _model.mData[10] = (float)currentCalculatedMatrix.m33;
  _model.mData[11] = (float)currentCalculatedMatrix.m34;
  _model.mData[12] = (float)currentCalculatedMatrix.m41;
  _model.mData[13] = (float)currentCalculatedMatrix.m42;
  _model.mData[14] = (float)currentCalculatedMatrix.m43;
  _model.mData[15] = (float)currentCalculatedMatrix.m44;
#endif
  return _model;
}

vesMatrix4x4f vesMultitouchCamera::GetMatrixGMTL()
{
  return _matrix;
}

void vesMultitouchCamera::Reset()
{
#ifndef ANDROID
  float matrix[16]  = {1.0, 0.0, 0.0, 0.0,
                       0.0, 1.0, 0.0, 0.0,
                       0.0, 0.0, 1.0, 0.0,
                       0.0, 0.0, 0.0, 1.0};
  currentCalculatedMatrix.m11 = (CGFloat)matrix[0];
  currentCalculatedMatrix.m12 = (CGFloat)matrix[1];
  currentCalculatedMatrix.m13 = (CGFloat)matrix[2];
  currentCalculatedMatrix.m14 = (CGFloat)matrix[3];
  currentCalculatedMatrix.m21 = (CGFloat)matrix[4];
  currentCalculatedMatrix.m22 = (CGFloat)matrix[5];
  currentCalculatedMatrix.m23 = (CGFloat)matrix[6];
  currentCalculatedMatrix.m24 = (CGFloat)matrix[7];
  currentCalculatedMatrix.m31 = (CGFloat)matrix[8];
  currentCalculatedMatrix.m32 = (CGFloat)matrix[9];
  currentCalculatedMatrix.m33 = (CGFloat)matrix[10];
  currentCalculatedMatrix.m34 = (CGFloat)matrix[11];
  currentCalculatedMatrix.m41 = (CGFloat)matrix[12];
  currentCalculatedMatrix.m42 = (CGFloat)matrix[13];
  currentCalculatedMatrix.m43 = (CGFloat)matrix[14];
  currentCalculatedMatrix.m44 = (CGFloat)matrix[15];
#endif
  gmtl::identity(_matrix);
}

void vesMultitouchCamera::RotateAngleAxis(double angle, double x, double y, double z)
{
#ifndef ANDROID
  currentCalculatedMatrix = CATransform3DRotate(currentCalculatedMatrix, angle, x, y, z);
#endif
}

void vesMultitouchCamera::UpdateMatrix(const float xRotation,
                                       const float yRotation,
                                       float scaleFactor,
                                       float xTranslation,
                                       float yTranslation)
{
#ifndef ANDROID
  // Scale the view to fit current multitouch scaling
  currentCalculatedMatrix = CATransform3DScale(currentCalculatedMatrix, scaleFactor, scaleFactor, scaleFactor);
  //std::cout<<"========================"<<std::endl;
  //printCurrentCalculatedMatrix();

  // Perform incremental rotation based on current angles in X and Y
  float totalRotation = sqrt(xRotation*xRotation + yRotation*yRotation);

  CATransform3D temporaryMatrix = CATransform3DRotate(currentCalculatedMatrix, totalRotation * (22/7)* 1 / 180.0,
                                                      ((xRotation/totalRotation) * currentCalculatedMatrix.m12 + (yRotation/totalRotation) * currentCalculatedMatrix.m11),
                                                      ((xRotation/totalRotation) * currentCalculatedMatrix.m22 + (yRotation/totalRotation) * currentCalculatedMatrix.m21),
                                                      ((xRotation/totalRotation) * currentCalculatedMatrix.m32 + (yRotation/totalRotation) * currentCalculatedMatrix.m31));

  if ((temporaryMatrix.m11 >= -100.0) && (temporaryMatrix.m11 <= 100.0))
  {
    currentCalculatedMatrix = temporaryMatrix;
  }

  // Translate the model by the accumulated amount
  float currentScaleFactor = sqrt(pow(currentCalculatedMatrix.m11, 2.0f) + pow(currentCalculatedMatrix.m12, 2.0f) + pow(currentCalculatedMatrix.m13, 2.0f));

  float xTranslat = xTranslation / (currentScaleFactor * currentScaleFactor);
  float yTranslat = yTranslation / (currentScaleFactor * currentScaleFactor);

  // Use the (0,4,8) components to figure the eye's X axis in the model coordinate system, translate along that
  temporaryMatrix = CATransform3DTranslate(currentCalculatedMatrix,
                                           xTranslat * currentCalculatedMatrix.m11,
                                           xTranslat * currentCalculatedMatrix.m21,
                                           xTranslat * currentCalculatedMatrix.m31);
  // Use the (1,5,9) components to figure the eye's Y axis in the model coordinate system, translate along that
  temporaryMatrix = CATransform3DTranslate(temporaryMatrix,
                                           yTranslat * currentCalculatedMatrix.m12,
                                           yTranslat * currentCalculatedMatrix.m22,
                                           yTranslat * currentCalculatedMatrix.m32);


  if ((temporaryMatrix.m11 >= -100.0) && (temporaryMatrix.m11 <= 100.0))
    currentCalculatedMatrix = temporaryMatrix;

  // UpdateMatrixGMTL(xRotation, yRotation, scaleFactor, xTranslation, yTranslation);
#endif
}

void vesMultitouchCamera::UpdateMatrixGMTL(const float xRotation,
                                           const float yRotation,
                                           float scaleFactor,
                                           float xTranslation,
                                           float yTranslation)
{
  //    std::cout<<"------------------------"<<std::endl;
  printGMTLMatrix();

  vesMatrix4x4f matrix = makeTransposeMatrix4x4(_matrix);

  // Scale the view to fit current multitouch scaling
  matrix = makeScaleMatrix4x4(scaleFactor,scaleFactor,scaleFactor)*matrix;


  // Perform incremental rotation based on current angles in X and Y
  //  float totalRotation = sqrt(xRotation*xRotation + yRotation*yRotation);
  //  float x = xRotation/totalRotation;
  //  float y = yRotation/totalRotation;
  vesMatrix4x4f tempMat = makeRotationMatrix4x4(xRotation,
                                                matrix.mData[1],
                                                matrix.mData[5],
                                                matrix.mData[9])*matrix;
  tempMat = makeRotationMatrix4x4(yRotation,
                                  tempMat.mData[0],
                                  tempMat.mData[4],
                                  tempMat.mData[8])*matrix;
  //  vesMatrix4x4f tempMat = matrix*makeRotationMatrix4x4(totalRotation * (22/7) * 1/180.0,
  //                                               x*matrix[0][1]+y*matrix[0][0],
  //                                               x*matrix[1][1]+y*matrix[1][0],
  //                                               x*matrix[2][1]+y*matrix[2][0]);

  if (tempMat[0][0] >= -100 && tempMat[0][0] <=100)
  {
    matrix = tempMat;
  }

  // Translate the model by the accumulated amount
  float curScaleFactor = sqrt(pow(matrix[0][0],2.0f) +
                              pow(matrix[0][1],2.0f) +
                              pow(matrix[0][2],2.0f));

  float xTrans = xTranslation / (curScaleFactor * curScaleFactor);
  float yTrans = yTranslation / (curScaleFactor * curScaleFactor);

  tempMat = makeTranslationMatrix4x4(xTrans * vesVector3f(matrix.mData[0],
                                                          matrix.mData[4],
                                                          matrix.mData[8]))*matrix;
  tempMat = makeTranslationMatrix4x4(yTrans * vesVector3f(matrix.mData[1],
                                                          matrix.mData[5],
                                                          matrix.mData[9]))*matrix;

  if (tempMat[0][0] >= -100.0 && tempMat[0][0] <=100.0)
  {
    matrix = tempMat;
  }

  //_matrix = matrix;
  _matrix = makeTransposeMatrix4x4(matrix);

}

void vesMultitouchCamera::printCurrentCalculatedMatrix()
{
#ifndef ANDROID
  CATransform3D m = currentCalculatedMatrix;
  std::cout << m.m11 << " " << m.m12 << " " << m.m13 << " " << m.m14 << std::endl
            << m.m21 << " " << m.m22 << " " << m.m23 << " " << m.m24 << std::endl
            << m.m31 << " " << m.m32 << " " << m.m33 << " " << m.m34 << std::endl
            << m.m41 << " " << m.m42 << " " << m.m43 << " " << m.m44 << std::endl;
#endif
}

void vesMultitouchCamera::printGMTLMatrix()
{
  vesMatrix4x4f matrix = makeTransposeMatrix4x4(_matrix);
  for (int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      std::cout << _matrix[i][j] << " ";
    }
    std::cout<<std::endl;
  }

}
