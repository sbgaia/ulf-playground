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

In addition, there are some examples of modal reactors that control a simulated robot. 
They are the modal exercises of the [embedded-lab](https://github.com/lf-lang/embedded-lab) 
course, on a simulated robot ([src/lib/RobotSim.ulf](src/lib/RobotSim.ulf)), which prints where 
it is once per second. Its encoders, gyroscope and accelerometer have a little Gaussian noise, 
from a fixed seed, so every run is the same. Set `noise = 0` for exact readings, or 
change `seed` for another run:

| Example | What it shows |
| --- | --- |
| [RobotDrive](src/RobotDrive.ulf) | toggle between driving and stopped (Robot lab 7.2) |
| [RobotSquare](src/RobotSquare.ulf) | drive in a square with encoders and gyroscope (Robot lab 7.4) |
| [RobotAvoid](src/RobotAvoid.ulf) | the square, backing off from obstacles (Robot lab 7.5) |
| [HillLineAvoid](src/HillLineAvoid.ulf) | avoid the dark edges of a ramp (Hill Climb lab 8.2) |
| [HillClimb](src/HillClimb.ulf) | climb, turn around, drive down (Hill Climb lab 8.3) |
| [TrackFollow](src/TrackFollow.ulf) | follow a line (Track and Line Following lab 9.2) |

## Notes

- A timer declared inside a mode only fires while that mode is active, but its name belongs to the
  reactor, so two timers in different modes need different names.
- A `reaction(reset)` runs when a mode is entered with a reset transition, but not when the initial
  mode becomes active at startup. The robot examples therefore start in a short `INIT` mode.
- A reaction cannot declare both `reset(X)` and `history(X)` for the same mode. Use one reaction
  per kind of transition, as in MusicBox.
