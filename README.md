# Fractal
An OpenGL based fractal viewer.

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
* `1`: Mandelbrot fractal.
* `2`: Newton fractal.
* `Esc`: Exit.
