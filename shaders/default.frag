#version 400 core

out vec4 frag_color;

uniform int window_width;
uniform int window_height;
uniform dvec2 fractal_center;
uniform double fractal_width;
uniform double fractal_height;
uniform sampler1D texture;
uniform int max_iter;


dvec2 square(dvec2 z) {
  return dvec2(z.x*z.x - z.y*z.y, 2*z.x*z.y);
}


void main() {
  double w = window_width;
  double h = window_height;
  double r = window_height/window_width;
  double cx = fractal_center.x + (gl_FragCoord.x / w - 0.5)*fractal_width;
  double cy = fractal_center.y + (0.5 - gl_FragCoord.y / h)*fractal_height;
  dvec2 c = dvec2(cx, cy);

  dvec2 z = dvec2(0.0, 0.0);

  int iter = 0;
  //float max_iter = 10;

  while (iter < max_iter && length(z) < 2) {
    z = square(z) + c;
    iter++;
  }

  frag_color = texture1D(texture, iter/float(max_iter));
}
