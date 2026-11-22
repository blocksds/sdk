# BlocksDS SDK examples: Multithreading

This folder contains examples of how to use threads with libnds.

- `comutex`: Usage of mutexes with threads (preemptive).
- `detached_threads`: Creates detached threads that don't require manual cleanup when they end.
- `stack_in_tcm`: Creates threads with their context and stack in ITCM/DTCM.
- `many_threads`: How to create many threads with preemptive multithreading.
- `sync_to_vbl`: How to wait for interrupts with preemptive multithreading.
- `thread_priority`: Creates multiple threads with different priorities.
- `wait_for_signal`: How to wait for user-defined signals with preemptive threads.
