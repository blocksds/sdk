# BlocksDS SDK examples: DSP

This folder contains examples of how to use Teak DSP of the DSi.

- `cmd_reply`: It uses the CMD/REP registers to check that the DSP is running.
- `dma_arm_to_dsp`: Transfers data from ARM9 to DSP using DMA (synchronously).
- `dma_async`: Asynchronously uses DMA to transfer data from DSP to ARM9.
- `dma_dsp_to_arm`: Transfers data from DSP to ARM9 using DMA (synchronously).
- `fifo_transfers`: Transfers data between DSP and ARM9 using the FIFO.
- `irq_apbp`: Sets up a DSP IRQ when data is received in the REP registers.
- `irq_timer`: Sets up a DSP IRQ when a timer reloads.
- `multiple_binaries`: How to use multiple DSP binaries (from NitroFS or ARM9).
- `semaphores`: How to use the hardware semaphores of the DSP.
- `speakers_and_microphone`: Uses the DSP only to record audio and play it back.
- `speakers_output`: It sends audio to the speakers directly from the DSP.
- `timers`: It shows how to setup the DSP timers in different modes.
