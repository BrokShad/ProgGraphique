attribute highp vec4 posAttr;
attribute mediump vec4 texAttr;
attribute lowp  vec4 colAttr;
uniform highp mat4 matrix;

varying mediump vec4 texc;
varying lowp  vec4 col;

void main() {
   texc = texAttr;
   col = colAttr;
   gl_Position = matrix * posAttr;
}

