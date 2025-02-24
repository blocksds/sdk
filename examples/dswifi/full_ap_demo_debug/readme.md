# Full AP demo with debug logs

You can build the release version of this example with:

```
make
```

or or the debug version with debug logs from the ARM7 with:

```
make DEBUG=1
```

In order to enable debug logs from the ARM9, open `main.c` and change the define
to enable the redirection of logs from sgIP to `printf()`.
