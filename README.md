# linux-conio
A compatibility library so Linux users can work with Conio projects. Implemented on ncurses. Currently very incomplete.

# Building

```shell
$ make
```
or
```shell
$ make all
```

# Installing

```shell
$ make install
```
The default installation dir is `~/.local`. To change this to my_special_dir use:
```shell
$ make install INSTALL_DIR=my_special_dir
```

The folders `lib` and `include` are installed in those directories.
