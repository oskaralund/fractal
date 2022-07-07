# Fractal
An OpenGL based fractal viewer. Uses double precision to allow for extensive zooming. Requires OpenGL 4.0 or greater.

![](demo.gif)

## Building the project
```
git clone https://github.com/oskaralund/fractal
cd fractal
cmake .
make
```

## Usage
Hold left mouse button to drag the camera. Scroll to zoom.
* `J`: Increase number of iterations (allows further zoom at performance cost).
* `K`: Decrease number of iterations.
* `Z`: Constant zoom.
* `A`: Toggle automatic adjustment of iterations.
* `1`: Mandelbrot fractal.
* `2`: Newton fractal.
* `Esc`: Exit.
