# BlocksDS SDK examples: co-operative multithreading

This folder contains examples of how to use co-operative threads with libnds.

- `comutex_coop`: Usage of mutexes with threads (cooperative).
- `comutex_preempt`: Usage of mutexes with threads (preemptive).
- `stack_in_tcm`: Creates threads with their context and stack in ITCM/DTCM.
- `many_threads_coop`: How to create many threads with cooperative multithreading.
- `many_threads_preemp`: How to create many threads with preemptive multithreading.
- `sync_to_vbl_cothread`: How to wait for interrupts in multithreaded programs.
- `wait_for_signal`: How to make threads wait for user-defined signals.
