= The Mouse

This project builds upon last year’s _MinosMouse_ design, incorporating ideas from the _Picomouse_ project and introducing several of our own improvements. Namely, one improvement is the ability to traverse diagonals in a completely straight line, as opposed to a zigzag motion. To accomplish this, we've decided on a combinations of motor encoders, sensors, and a gyroscope to accurately track the position and orientation of the mouse.

*The Competition*\
Even though Ratatoskr will probably not compete in official MicroMouse competitions, we will still adhere to their rules, which are linked in full in @ref.\
We will also measure Ratatoskr's performance according to the official rules, which state:
1. Each mouse is allowed a maximum of 10 minutes to perform.
2. The scoring of a micro mouse shall be obtained by computing a handicapped time for each run as follows:\
  $ t_"total" = t_"run" space + space 1/30 dot t_"search" space + space p_"touch" $
  where $p_"touch"$ is a touch penalty and is calculated as $p_"touch" = 3 space + space 1/10 dot t_"run"$. It is only added if the mouse has been touched at any time between the search and the run.
These rules ensure that mice which use a lot of their maximal time finding the optimal path are penalized accordingly, as the goal of a good micromouse is to be not only fast but also efficient.
  
== Design and Technical Drawing
Below is a technical drawing of the mouse, showing the design from multiple angles as well as some of its dimensions in millimeters. Note that these drawings don't yet include the newly conceptualized sensor layout as described in @tof_sensors.

#figure(
  image("img/ratatoskr_drawing_v1.svg", width:90%),
  caption: [Technical drawing of Ratatoskr (created using Autedesk Fusion360)]
) <RatatoskrDrawing>

== Hardware

#let i2c = [I#super([2])C]

We will walk through the list of major hardware components used in Ratatoskr, explaining the reasoning and purpose of our choices.
=== Motors, Drivers & Encoders
Just like previous proven attempts, Ratatoskr will use two N20 DC gear motors with 750 RPM and built-in Hall encoders. We decided to keep this motor type since they’ve proven reliable and compact. The encoders allow for accurate position and speed control, which is necessary for both precise maze exploration and fast traversal. 

// The motors will be driven by an MX1508 dual H-bridge driver /* THIS CAN VERY MUCH CHANGE. it did lmao*/. We chose this setup due to its simplicity, as it gives us enough control for differential driving using standard PWM and direction signals. The ESP32 will generate PWM signals for each motor and handle encoder interrupts to read wheel rotation. 
The motors will be driven by an Adafruit DRV8871 DC Motor Driver Breakout Board (3.6 A max). 
We chose this driver since it offers a higher current limit and overall better efficiency, while remaining compact and easy to interface with. The DRV8871 provides reliable bidirectional control through standard PWM and direction pins, which makes it perfectly suited for our differential drive setup. // differential drive setup sounds too weird 

The ESP32 will generate PWM signals for each motor and handle encoder interrupts to read wheel rotation. This setup allows us to precisely regulate motor speed and direction while keeping the driver circuit simple and robust. The driver also includes built-in protection features such as current limiting, thermal shutdown, and undervoltage lockout, reducing the risk of hardware failure during testing.

We plan on using the encoders to estimate distance and direction by comparing the ticks of both wheels. This gives us an internal odometry system that can be cross-checked with the gyroscope and wall sensors later on.

For power, the motors run directly off the 7.4V LiPo battery #footnote([Although initially, we will be using dry cell batteries for power]) through the driver, while the rest of the system will be powered through a buck converter that brings the voltage down to 5V for logic and sensors. The encoders connect to interrupt-capable GPIO pins on the ESP32.

Overall, the hardware control of the motors will stay very similar to previous years’ designs, but we will focus more on improving wheel control through better PID tuning and the combined use of encoder and gyro data to reduce slippage and drift. More importantly, we will aim to develop a combination of hardware and software for Ratatoskr to be able to traverse diagonals within the maze structure efficiently.

=== Gyroscope
For measuring orientation and rotational velocity, we are using an MPU-6050 IMU module. It’s a 6-axis sensor that provides both accelerometer and gyroscope data over #i2c, and is commonly used in small robotic systems like ours. 

The main purpose of the gyroscope is to keep track of Ratatoskr’s direction and correct drift that occurs from wheel slip or uneven surfaces. During runs, the gyroscope will be used to ensure straight motion (keeping angular velocity close to zero) and to control turns precisely by integrating angular velocity until the desired rotation angle is reached.\
The IMU connects via #i2c to the ESP32, and readings will be used with the wheel encoder data to improve reliability. At startup, the gyroscope will calibrate itself by measuring bias when the mouse is stationary. This zeroing process is necessary to prevent drift during longer runs.

We considered switching to a higher-end IMU (like the BMI160 used in some previous projects), but decided that the MPU-6050 is sufficient and easier to source locally. If we later notice high drift or inconsistent readings, the module can be replaced without any major hardware or software changes, since the communication protocol remains the same. /*idk i kinda felt i should mention why we didnt use pico mouses gyro stuff*/

=== TOF sensors <tof_sensors>
For wall detection and distance measurement, we are using four Time-of-Flight (ToF) sensors, similar to the setup used previously but slightly adjusted for our own needs. The sensors will help Ratatoskr detect nearby walls and align itself accurately within the maze.

We will use two VL53L4CD sensors facing forward to detect walls ahead of the mouse and to measure break distance at high speed. These long-range sensors allow the mouse to slow down early before hitting a wall and to plan turns smoothly.\
Additionally, we will use two VL6180X sensors placed on the left and right sides of the mouse. These are short-range sensors used for centering between walls and for detecting open paths or intersections. The combination of both types gives us accurate coverage around the front half of the mouse.

All four sensors communicate over #i2c. Since they share the same default address, each one will be assigned a unique address during initialization using the XSHUT pins, or controlled through the #i2c multiplexer (see @bom.)

In operation, the forward sensors will be used mainly for detecting walls and deceleration, while the side sensors will continuously poll and measure distances to maintain alignment. The software will filter these readings to ignore invalid values and handle possible occasional reflections from the maze which may mess with the sensors.
This arrangement gives Ratatoskr a good field of view while avoiding the sensors overlapping and interfering with one another. See also @turn_types for an overview of how these sensors will be used to navigate turns.

== Software
The software for this project will be loosely based on the previous iterations that we have access to and therefore use `C++` as it was used in both projects and is generally the language of choice for such applications. We have decided on using the Google formatting style but using 4 spaces of indent, enforceable using the `.clang-format` file. All code that is not directly interacting with hardware should be testable without access to the mouse using unit tests and the #link("https://github.com/mackorone/mms")[mms] micromouse simulation software.


=== Hardware Interfaces
We will need some software interfaces to communicate with the hardware mentioned earlier, the details of which are listed in the following.

==== Motor Interface Overview

// Source: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-drv8871-brushed-dc-motor-driver-breakout.pdf
// https://cdn-shop.adafruit.com/product-files/3190/drv8871.pdf TEXAS INSTRUMENTS? bro uses a calculator for the mouse

The Adafruit #link("https://cdn-learn.adafruit.com/downloads/pdf/adafruit-drv8871-brushed-dc-motor-driver-breakout.pdf")[DRV8871] DC Motor Driver Breakout Board provides two input pins (IN1 and IN2) that control the H-bridge and can accept PWM signals up to #text("~")200kHz. The board supports 6.5–45V motor supply and up to 3.6A peak current, with built-in protections for overcurrent, thermal shutdown, and undervoltage lockout. // this part looks like im a salesman tryna pitch mfs this motor, its tech details i saw in the first page, dk if i should include

*Attributes*
#table(
  columns: (1fr, 6fr),
  inset: 0pt,
  row-gutter: 5pt,
  stroke: none,
  [`in1_pin`], [first input pin connected to ESP32 PWM/logic GPIO  ],
  [`in2_pin`], [second input pin connected to ESP32 PWM/logic GPIO  ],
  [`pwm_channel`], [ESP32 PWM channel (if hardware PWM is used)  ],
  [`max_pwm`], [upper limit for PWM duty (0–255 or 0–1023 depending on mode)  ],
  [`target_duty`], [last commanded PWM duty cycle  ],
  [`direction`], [current direction ("forward", "backward", "brake", "coast")  ],
)

#pagebreak()
*Methods*
#table(
  columns: (1fr, 4.5fr),
  inset: 0pt,
  row-gutter: 5pt,
  stroke: none,
  [`init()`], [sets up both pins as outputs, attaches PWM channel, and ensures driver is idle.  ],
  [`set_speed(duty)`], [writes PWM value to selected input and LOW to the other pin.  ],
  [`forward(speed)`], [sets IN1 = LOW, PWM on IN2.  ],
  [`backward(speed)`], [sets IN2 = LOW, PWM on IN1.  ],
  [`brake()`], [drives both pins HIGH (active braking).  ],
  [`coast()`], [drives both pins LOW (free spin).  ],
  [`stop()`], [alias for `coast()`, used when idling or shutting down.  ],
  [`test_ramp()`], [optional helper to sweep PWM up/down for diagnostics.  ],
)

*Example usage*
```python
motorL = MotorDRV8871(in1_pin=25, in2_pin=26)
motorL.init()

# run forward half speed
motorL.forward(128)

# brake briefly, then reverse
motorL.brake()
delay(0.2)
motorL.backward(200)

motorL.stop()
```


All motor instances share the same pattern: one DRV8871 per wheel, each controlled through two GPIOs. Speed control is achieved purely through PWM duty cycle, and the higher-level motion control (PID, odometry, etc.) determines the duty target.

To conclude:
#align(center, scale(90%, (
$ ("IN1" eq "LOW") space &and space ("IN2" eq "PWM") space &&arrow space "Forward" \
("IN1" eq "PWM") space &and space ("IN2" eq "LOW") space &&arrow space "Backward" \
("IN1" eq "LOW") space &and space ("IN2" eq "LOW") space &&arrow space "Coast" \
("IN1" eq "HIGH") space &and space ("IN2" eq "HIGH") space &&arrow space "Break" $
// the docs are goated
)))

This layout gives us a clean, hardware-agnostic motor interface that remains valid even if the driver is swapped later (as long as it accepts PWM + DIR logic inputs).

==== ToF Interface Overview
Both ToF sensors speak #i2c and measure distance via laser time-of-flight. The short-range #link("https://cdn-learn.adafruit.com/downloads/pdf/adafruit-vl6180x-time-of-flight-micro-lidar-distance-sensor-breakout.pdf")[VL6180X] reliably covers up to #text("~")200 mm which is ideal for side-wall centering, while the longer-range #link("https://cdn-learn.adafruit.com/downloads/pdf/adafruit-vl53l4cd-time-of-flight-distance-sensor.pdf")[VL53L4CD] covers up to #text("~")1.2 m allowing forward look-ahead for fast runs. We will provide one abstract interface class in software that handles initialization, reading distance, error checking and supports multiple sensors. Sensors will be configured individually (side vs front) but use the same underlying driver logic. This gives us a clean way to plug whichever breakout we use (or future upgrade) and ensures the maze-solving logic can simply ask "what’s my distance to wall X?" without worrying about the specifics. 
/*
A breakout is just the little PCB that hosts the chip (e.g., VL53L4CD) plus regulator, level-shifters, pull-ups, pins, etc. Different vendors (Adafruit, Pololu, Waveshare, generic ST eval boards) sell different breakouts of the same chip. If we write a thin driver that only depends on the chip’s #i2c registers + one XSHUT pin, we can swap any vendor’s breakout in or out without touching the rest of the code. (had to google search breakout)
*/
// @EVERYONE DO I GO IN DEPTH HERE? or just mention docs, cause i can give step by step of what to do but that may be too long, cause it'd just be reiterating the docs
// locked out for lunch
// I feel like this might be a bit much for the proposal, but at that point, what do I know

All sensors share the same basic interface so they can be handled the same way, regardless of model or where they’re mounted (front or side).

*Attributes*
#table(
  columns: (1fr, 6fr),
  inset: 0pt,
  row-gutter: 5pt,
  stroke: none,

  [`addr`], [#i2c address (unique per sensor after initialization)],
  [`xshut_pin`], [GPIO pin used to enable or disable the sensor],
  [`role`], ["front" or "side" (determines config like range and polling rate, can be enum)],
  [`distance`], [last valid distance reading (in mm)],
  [`valid`], [flag showing if the current reading is valid],
  [`timestamp`], [last time a valid reading was taken]
)

*Methods*
#table(
  columns: (1fr, 6fr),
  inset: 0pt,
  row-gutter: 5pt,
  stroke: none,
  [`init()`], [powers up the sensor, assigns #i2c address, sets measurement mode and timing.],
  [`start()`], [begins continuous measurements or enables periodic polling.],
  [`read()`], [reads the latest distance value (returns distance in mm and a validity flag).],
  [`stop()`], [stops ranging and powers down the sensor (used when idling or resetting).],
  [`reset()`], [performs a soft reset if a sensor becomes unresponsive.],
  [`is_valid()`], [returns whether the current measurement is valid or timed out.],
  [`filter()`], [optional smoothing (median or moving average) to remove spurious spikes.] // might be useful if our maze gets fucked in the ass
)

*Example usage*
```python
tof_left = ToF(addr=0x2A, xshut_pin=17, role="side")
tof_left.init()
tof_left.start()

distance, valid = tof_left.read()
if valid:
  print("Left wall:", distance, "mm")
else:
  print("No valid reading")
tof_left.stop()
```

All ToF sensors will be managed through a shared driver that loops over each sensor object, polling and updating readings in the background. This lets higher-level modules (e.g., centering or wall detection) just call something like `get_wall_distance("left")` without worrying about the hardware details.

==== Gyroscope Interface Overview
// man i love adafruit: https://cdn-learn.adafruit.com/downloads/pdf/mpu6050-6-dof-accelerometer-and-gyro.pdf
// https://www.youtube.com/watch?v=XCyRXMvVSCw will be useful

The #link("https://cdn-learn.adafruit.com/downloads/pdf/mpu6050-6-dof-accelerometer-and-gyro.pdf")[MPU-6050] sits on #i2c (default addr `0x68`; `0x69` if `AD0` is high) and provides 3-axis gyro + 3-axis accel. We only require readings from this module so we will aim for the interface to be very simple.

*Attributes*
#table(
  columns: (1fr, 6fr),
  inset: 0pt,
  row-gutter: 5pt,
  stroke: none,
  [`addr`], [#i2c address (`0x68` or `0x69`)],
  [`gyro`], [latest gyro readings (x, y, z)],
  [`accel`], [latest accel readings (x, y, z)],
  [`timestamp`], [last time the sensor was updated],
  [`valid`], [flag indicating whether the last read was successful],
)

*Methods*
#table(
  columns: (1fr, 6fr),
  inset: 0pt,
  row-gutter: 5pt,
  stroke: none,
  [`init()`], [initializes #i2c communication and wakes up the sensor.],
  [`start()`], [begins regular data polling from the IMU.],
  [`read()`], [returns the latest gyro and accel readings.],
  [`stop()`], [stops polling when the mouse is idle or powered down.],
  [`is_valid()`], [checks if the most recent data is valid.],
)
#pagebreak()
*Example Usage*
```python
imu = IMU_MPU6050(addr=0x68)
imu.init()
imu.start()

gyro, accel = imu.read()
if imu.is_valid():
    print("Gyro:", gyro, "Accel:", accel)
imu.stop()
```
This basic interface is enough for our needs: it lets us continuously read motion data and feed it into Ratatoskr’s movement and orientation logic.


//The Adafruit breakout has on-board regulation/level-shifting (power it with 3–5 V; SCL/SDA already pulled up), and exposes an **INT** pin we can use for “data ready” if we want lower-latency reads. // ig for faster reads?

=== Movement Control
We will use a class to streamline the control of the mouse and to create an API which is able to interact with the simulator.
#columns(2, [
- `int mazeWidth()`
- `int mazeHeight()`
- `bool wallFront()`
- `bool wallRight()`
- `bool wallLeft()`
- `void moveForward(int distance = 1)`
- `void turnRight()`
- `void turnLeft()`
- `void setWall(int x, int y, char direction)`
#colbreak()
- `void clearWall(int x, int y, char direction)`
- `void setColor(int x, int y, char color)`
- `void clearColor(int x, int y)`
- `void clearAllColor()`
- `void setText(int x, int y, String text)`
- `void clearText(int x, int y)`
- `void clearAllText()`
- `bool wasReset()`
- `void ackReset()`
])
// nailed it

=== PID Controllers (Proportional–integral–derivative)

/*To ensure precise and stable motion, Ratatoskr  will rely heavily on PID control loops. There will be two main control layers: one for wheel speed (inner loop) and one for heading or distance (outer loop). /* Why is this the case? This sort of makes it seem as if it was declared by divine command :) uhh its what they did i think we can lowkey just merge */*/ // old stuff

To make Ratatoskr’s movement precise and stable, we rely on PID control loops that constantly adjust the motor power based on sensor feedback. The idea is to let the mouse correct itself continuously on the fly.

In our implementation, we combine two kinds of control: one that regulates the wheel speeds through encoder feedback, and another that keeps the overall direction stable using the gyroscope. In practice, this means the encoders ensure both wheels spin at the correct rate, while the gyro makes sure the mouse maintains or changes its orientation exactly as commanded. Together they help Ratatoskr drive straight, turn cleanly, and recover quickly if one wheel slips.

The wheel speed PID adjusts the PWM sent to each motor so that the measured encoder speed matches the target speed. This keeps both wheels synchronized and allows for smooth acceleration. The PID in charge of direction, on the other hand, uses gyro and encoder data to correct deviations from the intended orientation. For instance, if the mouse starts veering slightly left or right while moving straight. 
// Also why do you use "heading" instead of "direction" or "orientation"? lmao no reason, just read some place calling it "heading", i think it has naval origins but idk
// is meer basically the french fucking around in gergamn
// whats the caspian see, isnane, does the meer/see thing come 
// from second hand nature? like "this sounds better" cool
// so even tho its northern german, we still say zurichsee
// 
// whoops i hit ctrl W by accident


We will start with the PID parameters from MinosMouse as a reference, since they were tuned for a similar motor and weight setup. However, we’ll re-tune them for Ratatoskr’s specific mass distribution and the wooden maze surface. Tuning will be done empirically: testing response times, overshoot, and steady-state errors until the mouse can accurately complete physical test routes we will set up. // Changed something here

Each control loop runs at a fixed update rate (likely 500–1000 Hz), which is fast enough for stable control but not too heavy for the ESP32. The final PID equations will be implemented using integer arithmetic where possible to minimize floating-point overhead.

In short, the PID controllers ensure that Ratatoskr moves in a controlled, predictable way not too fast to slip, and not too slow to waste time. They also ensure that Ratatoskr will not crash into walls at very high speeds due to the fact that PID uses real-time data and automatically adjusts the movement. /*sounds like a shit sentence*/ Once properly tuned, they’ll form the backbone of the motion control system.

=== Maze Solver
/*The core piece of the software is the maze solving algorithm; this software solves the maze in two phases: a `search` phase and a `run` phase, which do as their names suggest. */
// I've moved this to the abstract by addind two words

The mouse initially has no information on the maze which motivates the `search` traversal of the maze to discover the layout. As this phase is also timed and a fraction of it added to the final time, speed is not irrelevant. To save time we will employ an algorithm that increases the likelihood that the best path is inside of the explored labyrinth while not discovering every cell. The mouse needs to keep track of the walls present in each cell in order to construct a partial replica of the maze in its memory. We will approach exploration with a bias of moving towards the goal, where upon arriving the robot must make its way back to the start in order to initial the `run` phase. The way back to the start is used to discover more of the maze, once again using the `floodfill` algorithm. Using the micromouse simulator we can ensure that what the mouse wants to do is what it should do and optimize the path taken.

==== Diagonals
An additional issue for the solver will be the detection of diagonals. It should be possible to implement the detection of the type of diagonal and the encoding as post-processing steps that are applied to the path found by the simple solver. If time and resources allow, there is the possibility of refactoring the solver once the basic diagonals and straights work to search for the _fastest_ instead of the _shortest_ path. Further, the PID will need adjustments for the added complexity of sensor readings while traversing a diagonal.

=== LED Matrix & SD Card
As the battery on the micromouse is only sufficient for about half an hour of use, as observed by the MinosMouse team, we must make sure to waste as little battery life as possible. Thus, for debugging and logging we opt for a combined solution of an LED matrix and an SD card. This allows us to not be dependent on power-hungry systems like WiFi or Bluetooth.

The LED matrix will primarily be used during the runs, providing immediate feedback of the sensor data, for example which ToF sensors currently report a wall. A mockup of what this could look like can be seen in @led_matrix_mockup.\
The rest of the LEDs could be used to indicate other useful information, such as states of the state machine and so on.

The SD card will mainly be used after a run. It will collect and store information  about the run at a much more granular level than the LED matrix could provide. Thus in combination, if a run has weird behavior/*are we bri'ish on a cheeky chewsday innit?*/, we can immediately check the LED matrix and then look at the debug data afterward.

The hardware-software interfaces for the LED matrix and SD card are quite simple, as they both use #i2c for communication and there are well documented libraries for both, namely the _Adafruit LED Backpack Library_ and the builtin _SD_ library.

#figure(
  image("img/ratatoskr_LED_matrix.png", width:20%),
  caption: [The LED matrix showing that all four sensors are currently sensing a wall]
) <led_matrix_mockup>

// Cola trinken
// great:)
// und fortnite spielen
// ₍₍⚞(˶˃ ꒳ ˂˶)⚟⁾⁾
// ◝(ᵔᗜᵔ)◜