# BlocksDS SDK examples: Inter-processor communications

This folder contains examples of how to communicate between code running in the
ARM9 and the ARM7.

- `fifo_stress_test`: Sets up several FIFO channels and sends loads of messages.
- `pass_buffer_to_arm7`: ARM9 sends a buffer to ARM7. ARM7 replies with a value.
