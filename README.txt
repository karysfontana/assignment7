**Features**
- Upon starting, you show the model passed in as a .txt file and the text hierarchy in terminal.
- You start in stationary mode, You can press 2,3, and 4 to enter other modes
    - In Controlled mode, entered by pressing 2, you can use the arrow keys + F and B to 
    navigate around the screen. When  you press shift, you can rotate your camera using the 
    arrow keys.
    - In Chopper Mode, entered by pressing 3, the camera rotates around the model
    - Finally in Plane Mode, entered by pressing 4, makes the camera view a POV of the flying
    plane around the file.
- pressing S will enter you into Toon mode, where it uses cel-rendering
- The lighting system works as expected with two different spotlights and two positioned lights
- The Textures work work as expected also

**Implementation Details.**
I used Assignment 5 as a starting point since it had the scenegraph implementation
Below is the thought process behind the design of my program:
- First, I made GLLightRenderer to get all the lights and set up the scenegraph.
- Then, I set up the Toon Mode.
- Finally, I set up mipmapping to get Textures prepared

**Issues**
The technical difficulties I gathered from the Toon Mode and Mipmapping are really strange.
I do apologize for their faulty implementation, but they are working to an extent:

- Toon Mode has some NaN issues that spawn from the normalization in the vert. After hours,
I was still confused as to why this happens. The switching of the shaders works but all 
are on the lowest intensity. Even a TA was confused by this occurance.

- The Texture Mapping didn't pronounce the textures. The checkerboard was just gray and dull
when I rendered with just the texture. The reason this happened is also unknown. A good way
to see this one is by looking at object 1. It's slightly darker. That is the checkerboard
texture. So while mipmapping works, the textures themself are off.

**Citations**
White.ppm - I opened MSPaint and just downloaded the png then converted to ppm.
Checkerboard.ppm - I took it from the example LightsAndTextures from class.