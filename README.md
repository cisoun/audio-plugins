# Plugins

This project aims to develop a series of simple audio plugins with minimal dependencies.

> [!NOTE]
> This project is at its proof of concept stage. No usable plugin is available at this moment.

## Building

Install the prerequisites:

```sh
# MacOS
brew install cairo libsndfile lv2
# Debian/Ubuntu
apt-get install libcairo2-dev libsndfile1-dev lv2-dev
# Fedora
dnf install cairo-devel libsndfile-devel lv2-devel
```

Then from the root folder:

```sh
# Compile Pugl and project.
make
# Run test window.
make run
```

## Roadmap

Please refer the the [roadmap](ROADMAP.md).

## License

GNU General Public License version 3.

## References

 - https://github.com/brummer10/libxputty (GUI)
 - https://www.math.wustl.edu/~victor/mfmm/fourier/fft.c (FFT)
