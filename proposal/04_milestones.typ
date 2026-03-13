#import "@preview/gantty:0.4.0": gantt

#pagebreak()
= Tasks and Milestones
We have created a GANTT chart to provide an overview of the tasks to be done as well as their relations to the milestones and to each other.
#scale(90%, gantt(yaml("gantt.yaml")))

== Ratatoskr V1.0
The first version of the micromouse must be able to successfully maneuver and solve the maze in however much time it takes. This requires correct assembly and wiring of the physical components, working drivers for motors, gyroscope and sensors, a PID and an algorithm that can explore and solve the maze. At this points there is no need to worry about fancy turns or diagonals, a simple `stop - turn 90° - go` movement pattern will suffice, see type 0 in @turn_types.

== Ratatoskr V1.1
To reach this milestone it is necessary to improve upon the first version by compressing successive forward movements into a single long straight. Further, the mouse should now be able to complete both type 1 and type 2 turns (@turn_types).

== Ratatoskr V1.2
The next step will be to tackle diagonals. Requiring adjustments in the PID, two (possibly five) new turn types in order to efficiently and precisely handle entering, traversal and exiting. The algorithm composing the path for the final run will have to be adjusted to recognize diagonals and give the correct instructions.

== Ratatoskr Final
The final version of our micromouse should be able to solve the maze by exploring it and then compute and execute a final run in which it never comes to a full stop, accelerates on long straights and speeds along diagonals where applicable.


#figure(
  image("img/turn_types.svg", width: 80%),
  caption: [Turn Types]
) <turn_types>

