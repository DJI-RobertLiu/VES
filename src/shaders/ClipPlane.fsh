/*=========================================================================

 Program:   Visualization Toolkit
 Module:    Shader.fsh

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

varying lowp vec4  varColor;

varying float clipDistance;

void main()
{
  if(clipDistance < 0.0)
    discard;

  gl_FragColor = varColor;
}
