# ulf-playground

Small **modal reactors** for [micro-LF](https://micro-lf.org)
([reactor-uc](https://github.com/lf-lang/reactor-uc/tree/micromode)). 
Modal modes are implemented by the micromode runtime extension of reactor-uc.
They run on Linux or macOS.

Each example is one file in [src/](src). There is no keyboard: the buttons each example presses
are scheduled in its `reaction(startup)`, and `@timeout` stops the program. Change them to try
other cases. [src/lib/Display.ulf](src/lib/Display.ulf) prints every line it receives, prefixed
by its logical time.

## Build and run

```bash
git clone --recurse-submodules <this repository>
cd ulf-playground
```

Then build and run one example, here `InterruptModal`:

```bash
cmake -B build/InterruptModal -DLF_MAIN=InterruptModal
cmake --build build/InterruptModal
./build/InterruptModal/InterruptModal
```

The first build also builds the micro-LF compiler, which takes a few minutes. To build with
another checkout of reactor-uc, set `REACTOR_UC_PATH` to it.

## Examples

Each example shows one feature of modes. Read them in this order:

| Example | What it shows |
| --- | --- |
| [Turnstile](src/Turnstile.ulf) | modes, transitions, one mode active at a time |
| [InterruptModal](src/InterruptModal.ulf) | a timer in each mode (Interrupts lab 6.4) |
| [TrafficLight](src/TrafficLight.ulf) | timers inside modes restart when the mode is entered |
| [Thermostat](src/Thermostat.ulf) | hysteresis, from Lee & Seshia |
| [Toaster](src/Toaster.ulf) | pending actions of a mode that is left |
| [MusicBox](src/MusicBox.ulf) | `reset` versus `history` transitions |
| [WashingMachine](src/WashingMachine.ulf) | modes inside modes |
| [CruiseControl](src/CruiseControl.ulf) | connections that change with the mode |
| [Irrigation](src/Irrigation.ulf) | `startup` and `shutdown` reactions in a mode |


## Notes

- A timer declared inside a mode only fires while that mode is active, but its name belongs to the
  reactor, so two timers in different modes need different names.
- A `reaction(reset)` runs when a mode is entered with a reset transition, but not when the initial
  mode becomes active at startup.
- A reaction cannot declare both `reset(X)` and `history(X)` for the same mode. Use one reaction
  per kind of transition, as in MusicBox.
