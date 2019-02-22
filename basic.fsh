varying mediump vec4 texc;
uniform sampler2D texture;

void main() {
   gl_FragColor = texture2D(texture, texc.st);
}
