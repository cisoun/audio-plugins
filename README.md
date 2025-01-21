# Plugins

## Building

Install the prerequisites:

```sh
# MacOS
brew install cairo libsndfile lv2
# Debian/Ubuntu
apt-get install libcairo2-dev
# Fedora
dnf install cairo-devel
```

Then from the root folder:

```sh
make
make run
```

## Roadmap

 - [ ] UI backend
   - [X] Windowing backend (pugl)
   - [X] Drawing backend (cairo)
   - [X] Widgets
     - [X] Buttons
     - [X] File dialog
     - [X] Knobs
     - [X] Text
   - [ ] UI optimizations
 - [ ] DSP backend
 - [ ] Plugin integration
   - [ ] AudioUnit
   - [ ] LV2
   - [ ] VST

## References

 - https://github.com/brummer10/libxputty (GUI)
 - https://www.math.wustl.edu/~victor/mfmm/fourier/fft.c (FFT)
