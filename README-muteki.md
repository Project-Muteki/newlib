# Muteki-specific instructions

## Using bear and clangd

Create a compiler semantic config file that excludes system GCC. For example:

```json
{
    "compilation": {
        "compilers_to_exclude": ["/usr/bin/gcc"]
    }
}
```

To configure a build directory for use with the default clangd configuration (the configure parameters given here are just examples):

```sh
mkdir builddir
cd builddir

bear --config /path/to/compiler-semantic-config.json -- ../configure \
  --prefix=/usr/local \
  --build=x86_64-pc-linux-gnu \
  --host=x86_64-pc-linux-gnu \
  --target=arm-none-bestaeabi \
  --disable-newlib-supplied-syscalls \
  --disable-nls \
  --disable-libgloss \
  --enable-newlib-mb \
  --enable-newlib-iconv \
  --enable-newlib-register-fini \
  --enable-newlib-reent-thread-local
```

To build (for the first time or incrementally):

```sh
bear --config /path/to/compiler-semantic-config.json --append -- make all-target-newlib -j$(nproc)
```

[!NOTE]
Bear is not shipped with Muteki Flatpak SDK. To use it with a Flatpak IDE, you either need to [build it from source](https://github.com/rizsotto/Bear/blob/master/INSTALL.md), or run the above build commands outside of your Flatpak IDE (e.g. in a distrobox container) and patch the resulting compiler paths to `/usr/lib/sdk/muteki/bin/arm-none-bestaeabi-gcc`.
