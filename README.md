# assignment7

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

KDTree: I primarily used the assignment page, the raytracer module slides, and the textbook 
sections 12.3.1 & 12.3.2 (I know it's technically for BSP trees, however, it does give examples 
(visuals & math) for the spatial dimensional stuff). Thought process is below: 
- I first started with reading through and then modifying the given files header to support
    triangle storage and implementation 
- Once the header files were set up to add triangles, I implemented assigning triangles after 
    reading primitives (I used a helper method to assign triangles)
- I read through Andres' RaytraceMesh class again to understand their methods better and made my
    implementation plan based on that as well as the explained math in the assignment page and textbook. 
- I created three public getters so that I could easily access the left and right nodes and the plane 
- I decided to make a secondary method (intersectNode) to handle the leaf nodes and the four cases 
    for the internal nodes so I could focus on the Moller-Trumbore in the intersectTriangles method. 
For optimization: Andres and I spoke about how to use the bounding box method for optimization and from there: 
- I set both tmin & tmax to infinity in intersect_bounding_box to infinity so that the values could be 
    found to traverse the tree. 
- I changed the given intersect method (again) so that it checks if the ray misses the box BEFORE traversing. 
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