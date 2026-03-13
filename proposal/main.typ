#import "template.typ": *
#show: template.with(title: "Ratatoskr", class: "CS-358a")

#align(center, [
  Leoluca Bernardi, Leonardo Bolognese, Ali Gorgani, Mattia Metzler, Anthony Tamberg
])

#outline(depth: 3)
#pagebreak()
= Abstract
Ratatoskr is a small autonomous wheeled robot, a 'micromouse', that can find the fastest path through a maze. In two separate `search` and `run` phases, the mouse finds the most optimal path and then traverses it, accomplishing both in the shortest possible time. This project's aim is to improve the design and software of the previous year's micromice, as well as the design of the maze that they solve. The final result will be a faster micromouse wandering a smoother maze.

// There's literally nothing i can add that isn't redundant i.e., We will improve it by improving it... (duh)
// hjlhjklhjklhklhjklhjklhjklhlkjhlkjhlkjhlkjhlkjhjklhjklhjkhlhjkl
//【デ═一 guns
// ( ͡° ͜ʖ ͡°)︻̷┻̿═━一- 
// by the way guys leonardo has been up there for soooo many days its worrying
// ╭∩╮( ͡⚆ ͜ʖ ͡⚆)╭∩╮
// i think we cooked chat
// *We have a deadline* hey leo
//------------------------------------------------------------------------------------(The line in question)
/// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH
// bella ciao bella ciao bella ciao ciao ciao
#emoji
// tianemen square lookin ass place
// this will no longer eexist
// oh god vim lords are here
// (づ ◕‿◕ )づ

#include("02_maze.typ")
#include("03_mouse.typ")
#include("04_milestones.typ")
#include("05_userstories.typ")
#include("06_risk.typ")
// #include("07_bom.old.typ")
#include("07_bom.typ")

/* not yet needed for the first handin
#include("08_assembly.typ") */

= Inspiration and References <ref>
Last year's micromice:
- MinosMouse: https://github.com/epfl-cs358/2024fa-minosmouse
- PicoMouse: https://github.com/epfl-cs358/2024fa-picomouse

Official micromouse rules: https://micromouseonline.com/micromouse-book/rules/
