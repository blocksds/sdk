---
title: Using the ARM7
weight: 10
---

## 1. Introduction

All Nintendo DS applications need to run on both the ARM9 and ARM7 CPUs. Each
CPU has access to different hardware:

ARM9:

- 2D and 3D graphics of both screens.
- Hardware divider and square root calculators.
- DSi DSP configuration.
- VRAM.
- CPU cache.
- DTCM and ITCM.

ARM7:

- ARM7 WRAM.
- Firmware user settings.
- Audio.
- Touch screen.
- RTC (Real Time Clock).
- Buttons X and Y.
- VRAM banks C and D (if the ARM9 maps them to the ARM7).
- Microphone.
- Wi-Fi.
- DSi camera.
- DSi SD card.
- DSi NAND storage.
- DSi AES engine.

Both:

- Main RAM (there are access penalties if both CPUs access it at the same time).
- Shared WRAM (but only one CPU at a time, BlocksDS maps all of it to the ARM7).
- Timers (each CPU has 4 timers, they aren't shared).
- DMA (each CPU has 4 DMA channels, they aren't shared).
- All buttons (except for X and Y).
- Slot-1 and Slot-2 cartridges (but only one CPU at a time).
- Inter-processor communication hardware (FIFO).
- DSi NDMA (each CPU has 4 channels, they aren't shared).
- DSi NWRAM memory and settings.

In practice, the ARM7 acts as an I/O coprocessor that handles peripherals while
the ARM9 controls all the logic of the application. BlocksDS allows you to
customize the code that runs on the ARM7 in your application, but it comes with
a few [pre-built ARM7 cores](https://github.com/blocksds/sdk/tree/master/sys/arm7/main_core)
that are good enough for most applications:

- `minimal`: Minimal core with only basic libnds services.
- `dswifi`: Libnds and DSWiFi.
- `maxmod`: Libnds and Maxmod. This is the default core used in the default
  makefiles and templates.
- `libxm7`: Libnds and LibXM7.
- `dswifi_maxmod`: Libnds, DSWiFi and Maxmod.
- `dswifi_libxm7`: Libnds, DSWiFi and LibXM7.

Some reasons to write a custom ARM7 core are:

- Libnds doesn't have a function to do what you need to do. For example, libnds
  doesn't have any helper to set the time of the RTC chip.

- You don't want to use Maxmod or LibXM7 and you're writing your own music
  engine.

- You want to offload some calculations to the ARM7 so that the ARM9 has more
  time to do other things.

Some reasons to NOT write a custom ARM7 core are:

- The more you use the ARM7 for custom code, the less time there is for regular
  ARM7 services to run. If you try to use Wi-Fi, play music, read from the SD
  card of the DSi, and you run custom code on top of that, it's possible that
  there won't be enough time for the ARM7 to do it.

- Communications between the CPUs are tricky. It's possible to send commands and
  data safely using the FIFO engine of libnds, but that only works for small
  chunks of data. Transferring big amounts of data between the CPUs requires
  shared buffers in main RAM. The ARM7 can't see the cache of the ARM9, so you
  need to be careful about managing the cache from the ARM9 so that reading the
  buffer doesn't return outdated values.

- The ARM7 doesn't have that much RAM assigned to it. All the ARM7 code and data
  is stored in WRAM, which is quite small on the DS (96 KB). It's bigger on a
  DSi, but that doesn't help you with applications that need to work on both
  models. In practice, it's very easy to run out of memory on the ARM7 if you
  use Wi-Fi, some music player, and other custom code.

- Offloading processing code to the ARM7 isn't as fast as you may think. WRAM is
  quite small, so any shared data between ARM7 and ARM9 is generally stored in
  main RAM. It isn't possible for both CPUs to access it at the same time, it
  results in delays that can be quite noticeable for the ARM9. This is why the
  ARM7 code and data is stored in WRAM.

If you want to use a custom ARM7 core in your application, check
[this template](https://github.com/blocksds/sdk/tree/master/templates/rom_combined).

## 2. FIFO system of libnds

Libnds comes with a FIFO system that can be used to send messages between the
CPUs. This system defines 16 channels:

- `FIFO_PM`: Used for power management.
- `FIFO_SOUND`: Used for the sound helpers of libnds.
- `FIFO_SYSTEM`: Used for system functions.
- `FIFO_MAXMOD`: Used for the Maxmod library (and also for LibXM7).
- `FIFO_DSWIFI`: Used for the DSWiFi library.
- `FIFO_STORAGE`: Used for DS cart, DLDI, DSi SD and NAND access.
- `FIFO_FIRMWARE`: Used for firmware settings access.
- `FIFO_CAMERA`: Used for camera access.
- `FIFO_USER_01` to `FIFO_USER_08`: Channels available for users of libnds.

Users of libnds shouldn't use any of the reserved channels, they should only use
the user channels.

The FIFO system has a global buffer of received data on each CPU. This buffer is
divided as required to store messages from all channels. This way you don't need
to assign different sizes for each channel depending on how much you use the
channels. The global buffer will simply hold all data from all channels.

Each channel can send and receive 3 types of messages:

- `Value32`: 32-bit values. The messages are optimized for small values.
- `Address`: The only allowed addresses are main RAM addresses. This is useful
  to send the address of a buffer so that the other CPU receives a big amount of
  information.
- `DataMsg`: Messages of an arbitrary size. They must be reasonably small so
  that they don't take too much space in the global FIFO buffer.

Each channel defines one message queue for each of the message type. For
example, you may do manual reads of the Value32 queue of one channel and that
won't affect the address queue of that channel. In general, you want to use
Value32 and address messages as much as you can because they are faster to send
and easier to handle.

Sending messages is as easy as using functions `fifoSendAddress()`,
`fifoSendValue32()` and `fifoSendDatamsg()`. You can check the returned value to
verify if the message has been sent or if it has failed because the FIFO system
buffers are full.

You can receive messages in two ways:

- You can setup a callback that gets called whenever a message is received. Use
  functions `fifoSetAddressHandler()`, `fifoSetValue32Handler()` and
  `fifoSetDatamsgHandler()` to setup the handler.

- Check manually if there are available messages to be read. Use
  `fifoCheckAddress()`, `fifoCheckValue32()` or `fifoCheckDatamsg()` to check if
  there are messages. Once there are messages available, use `fifoGetAddress()`,
  `fifoGetValue32()` or `fifoGetDatamsg()` (and `fifoCheckDatamsgLength()`) to
  get the message.

  You can also wait for messages with `fifoWaitValue32()`, `fifoWaitAddress()`
  and `fifoWaitDatamsg()`.

This example shows how to send and receive messages using the FIFO system:
[`examples/ipc/fifo_stress_test`](https://github.com/blocksds/sdk/tree/master/examples/ipc/fifo_stress_test)

## 3. Sharing buffers between CPUs

The best way to share big amounts of data between the CPUs is to share the
address of a buffer in main RAM using a FIFO message. Then, both CPUs can read
and write data that the other CPU can see.

However, there is a big problem with this system: the ARM9 has a data cache that
the ARM7 can't see. Read the [chapter about caches](../tcm_and_cache) if you
haven't done it already.

If the ARM9 writes data to the buffer and sends a FIFO message to the ARM7 right
away it's very likely that there will be some data that hasn't been flushed to
main RAM. Similarly, if the ARM9 tries to wait for a variable to be updated in
the buffer it won't be able to do it because the first read caches the initial
values and all the following reads will simply read the cached value.

There are two different ways to handle this issue:

- The ARM9 can use a pointer to the buffer that is uncached. Main RAM is
  cached, but there are mirrors that can access main RAM bypassing the cache.
  You can usse the `memUncached()` helper of libnds to get the uncached mirror
  of an address inside main RAM, and `memCached()` to get the cached address of
  an uncached mirror.

  This is a very easy way to solve the problem because you can just read and
  write from the uncached mirror without worrying about anything. However, this
  system is slow. Accessing main RAM from the ARM9 without the cache is a lot
  slower than using the cache, and this system forces the ARM9 to do uncached
  accesses to RAM for every read and write.

- Manage the data cache yourself.

  - If the ARM9 wants to send data to the ARM7 it should call
    `DC_FlushRange(base, size)` to make sure that the buffer at address `base`
    with size `size` is updated in main RAM.

    You can check the following example to see how to do this correctly:
    [`examples/ipc/pass_buffer_to_arm7`](https://github.com/blocksds/sdk/tree/master/examples/ipc/pass_buffer_to_arm7)

  - If the ARM7 wants to send data to the ARM9, the ARM9 should flush the buffer
    **before** the ARM7 writes to it. For example, the ARM9 can allocate a
    buffer, flush it, and then send a command to the ARM7 to write data to the
    buffer. The ARM9 must wait for the ARM7 to send a "ready" message before
    reading the data.

    It's also possible to drop the current values in the cache and force the CPU
    to read from main RAM again with `DC_InvalidateRange(base, size)`. This
    function is **dangerous**. If you really want to use it, you need to make
    sure that your buffer is aligned to a data cache line (`CACHE_LINE_SIZE`,
    and that the size of the buffer is a multiple of the size of a data cache
    line. Cache handling functions don't operate with a granularity of a byte,
    they affect the full line. If the data you want to invalidate is in the same
    line as data you don't want to invalidate, you will lose that data too.
