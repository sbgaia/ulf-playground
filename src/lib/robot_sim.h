// Tiny simulator for a two-wheeled (differential-drive) robot in one of three worlds.
#ifndef ROBOT_SIM_H
#define ROBOT_SIM_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_SPEED 1.0                           // Speed of a wheel at full power (m/s).
#define WHEEL_BASE 0.09                         // Distance between the wheels (m).
#define SLOPE 15.0                              // Slope of the ramp (deg).
#define PI 3.14159265358979323846
#define TO_RAD(deg) ((deg) * PI / 180.0)
#define TO_DEG(rad) ((rad) * 180.0 / PI)

// Standard deviation of the noise on the sensor readings, at noise = 1.
#define ENCODER_NOISE 0.001                     // Encoders (m).
#define GYRO_NOISE 0.5                          // Gyroscope (deg).
#define ACCEL_NOISE 0.3                         // Accelerometer (deg).

// The three worlds: a flat floor, a ramp, and a track with a tape line.
typedef enum { WORLD_FLAT, WORLD_RAMP, WORLD_TRACK } world_t;

typedef struct {
  world_t world;
  double x, y, theta;                           // Position (m) and rotation (rad).
  double left, right;                           // Motor powers, from -1 to 1.
  double travelled;                             // Total distance travelled (m).
  double turned;                                // Total angle turned (rad).
  double noise;                                 // Scale of the sensor noise (0: none).
  uint64_t rng;                                 // State of the random number generator.
} robot_t;

typedef enum { SENSOR_LEFT, SENSOR_CENTER, SENSOR_RIGHT } line_sensor_t;

static inline void robot_init(robot_t* r, world_t world, double x, double y, double heading,
                              double noise, uint64_t seed) {
  memset(r, 0, sizeof(*r));
  r->world = world;
  r->x = x;
  r->y = y;
  r->theta = TO_RAD(heading);
  r->noise = noise;
  r->rng = seed;
}

// A uniform random number in (0, 1) using the SplitMix64 algorithm.
static inline double splitmix64_uniform(robot_t* r) {
  uint64_t z = (r->rng += 0x9E3779B97F4A7C15ull);
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
  z ^= z >> 31;
  return ((z >> 11) + 0.5) / 9007199254740992.0;  // 2^53
}

// Gaussian noise with the given standard deviation, scaled by the noise of the 
// robot (Box-Muller transform).
static inline double sensor_noise(robot_t* r, double stddev) {
  double u1 = splitmix64_uniform(r), u2 = splitmix64_uniform(r);
  return r->noise * stddev * sqrt(-2 * log(u1)) * cos(2 * PI * u2);
}

// Move the robot for a time step dt (s) based on the current motor powers.
static inline void robot_move(robot_t* r, double dt) {
  double vl = r->left * MAX_SPEED;
  double vr = r->right * MAX_SPEED;
  // Linear velocity (m/s) of the robot.
  double v = (vl + vr) / 2;
  // Angular velocity (rad/s) of the robot.
  double w = (vr - vl) / WHEEL_BASE;
  r->theta += w * dt;
  r->x += v * cos(r->theta) * dt;
  r->y += v * sin(r->theta) * dt;
  r->travelled += v * dt;
  r->turned += w * dt;
}

static inline bool robot_on_ramp(const robot_t* r) {
  return r->x > -0.3 && r->x < 1.4 && fabs(r->y) < 0.3;
}

static inline bool robot_on_slope(const robot_t* r) {
  return r->world == WORLD_RAMP && r->x > 0 && r->x < 1.0;
}

// Encoders: distance travelled (m), averaged over both wheels.
static inline double robot_distance(robot_t* r) {
  return r->travelled + sensor_noise(r, ENCODER_NOISE);
}

// Gyroscope: angle turned since startup (deg), positive to the left.
static inline double robot_angle(robot_t* r) {
  return TO_DEG(r->turned) + sensor_noise(r, GYRO_NOISE);
}

// Simulate the pitch and roll of the robot on a slope.
// The robot is assumed to be a rigid body, so it tilts as it goes up or down the ramp.
static inline double robot_pitch(robot_t* r) {
  return (robot_on_slope(r) ? SLOPE * cos(r->theta) : 0) + sensor_noise(r, ACCEL_NOISE);
}
static inline double robot_roll(robot_t* r) {
  return (robot_on_slope(r) ? SLOPE * sin(r->theta) : 0) + sensor_noise(r, ACCEL_NOISE);
}

// Check if the floor is dark at the given position (m). 
static inline bool floor_is_dark(world_t world, double x, double y) {
  if (world == WORLD_RAMP) {
    return !(x > -0.26 && x < 1.36 && fabs(y) < 0.26);  // 4 cm dark band around the ramp.
  }
  if (world == WORLD_TRACK) {
    // Distance from the oval: points at 0.4 m from the segment (0, 0.4)-(1, 0.4).
    double t = fmin(fmax(x, 0.0), 1.0);
    double d = hypot(x - t, y - 0.4) - 0.4;
    return fabs(d) < 0.0125;  // The tape is 2.5 cm wide.
  }
  return false;
}

// Line sensors placed 4 cm ahead of the wheels, 2 cm apart. 
// No noise is added to the line sensors.
static inline bool robot_line(const robot_t* r, line_sensor_t sensor) {
  double offset = sensor == SENSOR_LEFT ? 0.02 : sensor == SENSOR_RIGHT ? -0.02 : 0;
  double x = r->x + 0.04 * cos(r->theta) - offset * sin(r->theta);
  double y = r->y + 0.04 * sin(r->theta) + offset * cos(r->theta);
  return floor_is_dark(r->world, x, y);
}

static inline void robot_print(const robot_t* r, int64_t elapsed_ns) {
  printf("%7.3f s |   robot at x = %5.2f m, y = %5.2f m, heading %4.0f deg\n", elapsed_ns / 1e9,
         r->x, r->y, fmod(TO_DEG(r->theta) + 360, 360));
  fflush(stdout);
}

#endif
