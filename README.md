# Plugins

## Building

Install the prerequisites:

```sh
# MacOS
brew install cairo
# Debian/Ubuntu
apt-get install libcairo2-dev
# Fedora
dnf install cairo-devel
```

Then from the root folder:

```sh
scripts/build_pugl.sh
make
make run
```

## References

 - https://github.com/brummer10/libxputty (GUI)
 - https://www.math.wustl.edu/~victor/mfmm/fourier/fft.c (FFT)
