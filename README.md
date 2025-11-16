# Turbo-V12

An SDL2-powered homage to old-school sprite-scaling racers. Pilot a blaze-yellow Peugeot 306 Turbo Diesel across five themed stages, dodge traffic, blare the horn to scatter rivals, and beat the two-minute-per-stage timer.

## Requirements
- SDL2 development libraries (`libsdl2-dev`).

If the container image does not already have the SDL headers installed you can add them with:

```bash
sudo apt-get update && sudo apt-get install -y libsdl2-dev
```

## Building
```bash
make
```
This produces the `turbo_v12` executable in the repository root.

## Running
```bash
./turbo_v12
```

## Controls
- **Left / Right arrows (or A / D):** Steer the Peugeot 306.
- **Space bar:** Honk the grumpy diesel horn. Traffic cars will panic and swerve.
- **Enter:** Advance from the welcome screen, re-try after a failure, or move to the next stage.
- **Esc:** Quit.

Each level lasts two minutes; the timer is shown at the top of the screen. Stay on the road, avoid the cones at the edges, and complete all five levels to become an OutRun-style legend.

