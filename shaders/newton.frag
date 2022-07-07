#version 400 core

out vec4 frag_color;

uniform int window_width;
uniform int window_height;
uniform dvec2 fractal_center;
uniform double fractal_width;
uniform double fractal_height;
uniform sampler1D pal0;
uniform sampler1D pal1;
uniform int max_iter;


dvec2 mult(dvec2 z, dvec2 w) {
  return dvec2(z.x*w.x - z.y*w.y, z.x*w.y + z.y*w.x);
}


dvec2 conj(dvec2 z) {
  return dvec2(z.x, -z.y);
}


double lensq(dvec2 z) {
  return z.x*z.x + z.y*z.y;
}


dvec2 div(dvec2 z, dvec2 w) {
  return mult(z, conj(w))/lensq(w);
}


dvec2 f(dvec2 z) {
  return mult(mult(z,z), z) - dvec2(1, 0);
}


dvec2 fprime(dvec2 z) {
  return 3*mult(z,z);
}


dvec2 newton_step(dvec2 z) {
  return z - div(f(z), fprime(z));
}


void main() {
  double w = window_width;
  double h = window_height;
  double r = window_height/window_width;
  double cx = fractal_center.x + (gl_FragCoord.x / w - 0.5)*fractal_width;
  double cy = fractal_center.y + (gl_FragCoord.y / h - 0.5)*fractal_height;
  dvec2 c = dvec2(cx, cy);

  dvec2 z0 = dvec2(1, 0);
  dvec2 z1 = dvec2(-0.5, sqrt(3)/2);
  dvec2 z2 = dvec2(-0.5, -sqrt(3)/2);

  int iter = 0;

  while (iter < max_iter) {
    c = newton_step(c);
    iter++;
  }


  double eps = 1e-2;
  if (length(c-z0) < eps) {
    frag_color = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else if (length(c-z1) < eps) {
    frag_color = vec4(0.0, 1.0, 0.0, 1.0);
  }
  else if (length(c-z2) < eps) {
    frag_color = vec4(0.0, 0.0, 1.0, 1.0);
  }
  else {
    frag_color = vec4(1.0, 1.0, 1.0, 1.0);
  }
}
