# mruby-raspicam
mruby gem for accessing Raspberry Camera.

##Intro
At the moment, the camera grabs a snapshot and returns the coordinates of the red dot in the frame (if any):

```ruby
r = RaspiCam.new(1024).open
p r.position #=> [123, 117]
r.save_image "test.jpg"
r.close
```

## Building
On Raspberry, there is an issue when compiling gems with C++ code, that breaks the support for Ruby exceptions (any exception crashes the interpreter).
In order to circumvent this issue, the raspicam library is separatedly built as a shared library:

```sh
cd lib; make
sudo make install
make clean
cd ..
./run_test.rb
```

Note that `make install` only installs the shared library `/usr/local/lib/libLaserCam.so`, _and not the header files_, which remain under `lib`.