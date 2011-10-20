
// Uniforms.
uniform bool    hasVertexColors;
uniform mediump vec3 lightDirection;
uniform highp mat4   modelViewMatrix;
uniform mediump mat3 normalMatrix;
uniform lowp int     primitiveType;
uniform highp mat4   projectionMatrix;

// Vertex attributes.
attribute highp vec3   vertexPosition;
attribute mediump vec3 vertexNormal;
attribute lowp vec4    vertexColor;

// Varying attributes.
varying mediump vec4 varPosition;
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

varying lowp vec4 varColor;

void main()
{
  varPosition = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // 3 is line
  if (primitiveType != 3) {
    // Transform vertex normal into eye space.
    varNormal = normalize(normalMatrix * vertexNormal);

    // Save light direction (direction light for now)
    varLightDirection = normalize(vec3(0.0, 0.0, 0.650));
  }

  varColor = vertexColor;

  // GLSL still requires this.
  gl_Position = varPosition;
}
