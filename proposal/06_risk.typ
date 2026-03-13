= Risk Assessment
 Regarding Ratatoskr, we will discuss the 3 elementary sources of issues and the risks they convey, namely hardware, software, and human resources. To measure the likelihood of these risks, we will use a scale of 1, meaning very unlikely, to 5, meaning very likely.

== Hardware Risks
We made a list of a couple risks we will be facing in terms of hardware ordered from most likely to least likely but also with regard to importance:
/*
Color gradient for us to use from least to greatest scale
*/
#let best  =  "#1e3b8a"
#let good  =  "#2e5ba8"
#let mid   =  "#1484bc"
#let bad   =  "#e5582a"
#let worst =  "#963912"

- *Complexity of circuitry*
  - #(text(rgb(bad))[Likelihood: 4 ])
  - #(text(rgb(worst))[Impact: 5 ])
  - Effect: Use of multiple electrical components with all associated risks that come with it. Will lead to burning/ruining components. //Possibility of using LIPO batteries, 
  - Origin: Complexity will cause confusion on a hardware-level. This creates the risk
  - Prevention: Keeping the circuitry as simple and modular as possible. Sanity checking the design with multiple people and staff before implementing.
  
- *Insufficient power from motors (or hardware mismatch)*
  - #(text(rgb(mid))[Likelihood: 3 ])
  - #(text(rgb(bad))[Impact: 4 ])
  - Effect: Slower than expected mouse. One component can also become the weakest link, hindering other component capabilities (if motors are slow, then no use in getting top of the market gyros if our speed isn't even fast enough)
  - Origin: Miscalculation in design. Lack of foresight.
  - Prevention: Go over the design and plan multiple times with different people. Make sure to think until the very end of the assembly process.
  
- *Slippage on the new maze design * 
    - #(text(rgb(good))[Likelihood: 2 ])
    - #(text(rgb(mid))[Impact: 3 ])
  - Effect: Ratatoskr will not be able to make sharp turns. Slippage leads to uncertainty.
  - Origin: Trying out a new design for the maze floor which differs from the previous year's felted design.
  - Prevention: We believe MDF provides sufficient friction for Ratatoskr's tires. This belief/*(bro is getting philosophical, lmao couldnt think of another way to say "it might be fucking wrong tho")*/, however, could be misplaced. In the worst-case scenario, we may use some rubberized coating seal, like Flex Seal or Plasti Dip. 
  // @EVERYONE is this (above) a good idea?/should we add to bom just in case? naaaaah
  // https://www.lowes.com/pl/specialty-commercial-paint/rubberized-coatings/aerosol-spray/3011101225363-4294652196
  // https://www.ebay.de/itm/145287989702?chn=ps&norover=1&itemid=145287989702&targetid=4583039411461818&device=c&mktype=&googleloc=181558&poi=&campaignid=590369269&mkgroupid=1271037108052362&rlsatarget=pla-4583039411461818&abcId=10409296&merchantid=87778


== Software Risks
There is also a list of software concerns we may likely face:
- *I2C/Communication issues with hardware*
  - #(text(rgb(worst))[Likelihood: 5 ])
  - #(text(rgb(mid))[Impact: 3 ])
  - Effect: If software does not correctly communicate with hardware, we may face endless amounts of debugging and researching ways to bypass some restrictions left by I2C and our implementation. 
  - Origin: Lack of understanding or lack of infrastructure to seamlessly create an interface between the hardware we order and the software we make.
  - Prevention: Ensure to have completely read through documentation on the components along with their interfaces, specifically how they connect to software. If a component seems to cause problems, we can either rethink the component choice or think of a workaround.
  
- *Complexity of algorithms (algorithm mismatch)*  
  - #(text(rgb(mid))[Likelihood: 3 ])
  - #(text(rgb(good))[Impact: 2 ])
  - Effect: Nonoptimal attempts to solve/traverse the maze. Failed runs. 
  - Origin: Lack of understanding regarding the complexity of an algorithm or how it will be implemented. Perhaps choice of algorithm may not suit specific types of mazes, issues with scaling to bigger mazes.
  - Prevention: We must keep an eye on the choice/complexity of the algorithm we will use for solving and traversing the maze in the shortest distance and fastest time. We should think of the different scenarios Ratatoskr may be put it (small mazes with an emphasis and speed or large mazes with an emphasis of time)

// yooo software guys, imma write stuff here but try to guide me if im yapping hard

== Human Resource Risks
Finally, we have a list of human-related issues and risk we will likely face.
- *Bugs encountered when developing* 
  - #(text(rgb(bad))[Likelihood: 4 ])
  - #(text(rgb(mid))[Impact: 3 ])
  - Effect: The scale and complexity of the project may cause issues for the developing team. Malfunctioning code could be detrimental to Ratatoskr in some cases like crashing into maze walls.
  - Origin: Very likely but also very dependant on the team working on the project. It can be caused by either one person working on the code in its entirety and getting caught up by the complexity and scale, or it can due to lack of communication between the team members working on development. To add to this, debugging may be difficult with an autonomous piece of hardware. Live checks are not possible, thus hindering the debugging process.
  - Prevention: Through modularly choosing sections and tasks for development, we can spread the work through multiple people who master their own implementation and can effectively communicate their code with others. Having multiple people on the code may also help with frequent sanity checks on code logic. Logging data when completing runs may also help tremendously with the debugging procedure.
  
- *Mishandling of components*  
  - #(text(rgb(good))[Likelihood: 2])
  - #(text(rgb(mid))[Impact: 3 ])
  - Effect: May result in the destruction of the components we work with rendering them useless. That would in turn cause delays in the progress of the project (buying new components, figuring out fixes)
  - Origin: Lack of experience regarding the components we are dealing with. Lack of supervision when working with unfamiliar components. 
  - Prevention: Ensuring to check with more experienced people like the staff when it comes to unfamiliar hardware.