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

#ifndef __vesKiwiDataLoader_h
#define __vesKiwiDataLoader_h

#include <string>

class vesTriangleData;
class vtkAlgorithm;

class vesKiwiDataLoader
{
public:

  vesKiwiDataLoader();
  ~vesKiwiDataLoader();

  vesTriangleData* loadDataset(const std::string& filename);
  std::string errorTitle() const;
  std::string errorMessage() const;

protected:

  vesTriangleData* dataFromPolyDataAlgorithm(vtkAlgorithm* algorithm);
  bool updateAlgorithmOrSetErrorString(vtkAlgorithm* algorithm);
  bool hasEnding(const std::string& fullString, const std::string& ending) const;
  void setMaximumNumberOfPointsErrorMessage();

private:

  vesKiwiDataLoader(const vesKiwiDataLoader&); // Not implemented
  void operator=(const vesKiwiDataLoader&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif
