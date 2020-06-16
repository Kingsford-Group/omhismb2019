# OMH code

This is a proof of concept implementation of the OMH method.
The code is release under the BSD license.

# Compilation

## From release tarball

It is easiest to compile from the [release](https://github.com/Kingsford-Group/omhismb2019/releases).
After downloading and untarring the source release package, compile with (replace `/path/to/installation` with the correct path):

```Shell
./configure --prefix=/path/to/installation
make
make install
```

The library [xxhash](https://github.com/Cyan4973/xxHash) is required.
To run the unit tests with `make check`, [gtest](https://github.com/google/googletest) is required.

## From git tree

When compiling from the git tree, [yaggo](https://github.com/gmarcais/yaggo/releases) is an additional dependency (download the `yaggo` script and copy it in your path).
Then compile with:

```Shell
autoreconf -fi
./configure --prefix=/path/to/installation
make
make install
```
