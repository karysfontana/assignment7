# assignment8 

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
- KDTree partially works, however internal nodes don't properly render. To use it, switch the Raytracer in View for the KDRayTracer, as that's where it's KDTree is implemented. 

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

**Extra Credit**
Scene: For the extra credit scene I wanted to use the neptune model that's in ObjViewer. So, I planned the entire scene around the single neptune model (I was absolutely deadset on using it as the complex model) 
- I physically drew out a mausoleum/temple to house the neptune object as a statue to start. I prefer to do this because it gives me a base for how everything should look together (it's great for visualization and confirmed that my ideal scene was over the required 15 objects). 
- After, I started to define materials, I wanted to use beiege and grey tones for the temple itself, but keep neptune as a stand-out figure with its color. This took a couple of tries to get the look I was hoping for. 
- I then started with defining a base for the temple, then a pedestal for the neptune statue. 
- For the pedestal I really wanted it to look like an actual pedestal you'd find in a Roman temple, despite using primitive shapes. To do that, I made use a box as a base for the pedestal as a whole and then put 4 spheres ontop of the box base. I chose to use 4 spheres because I thought (and still think) it's reminiscent of the detailing between the base of a pedestal and the platform where a statue stands in a Roman temple. Finally, for the top of a pedestal, I used a flat cylinder to give the pedestal as a whole a round-ish shape (emphasis on ish) to mimic the taper effect actual pedestals have. 
- After I placed the neptune object ontop of the pedestal (which took a lot of fine-tuning in regards to scale and translations), I moved onto modeling the actual temple structure. 
- For the structure of the temple itself I continued to mimic Roman architecture. This meant creating 4 columns for the temple. I made a box base for each column, then placed a cylinder (scaled to be very tall) on top of the box base. I think that this effectively created a column for each corner, despite having primitive shapes. 
- Finally, for the roof of the temple, I used a very stretched out cone. I had initially planned to use a box base for the roof and then a cone on top, but that ended up looking ugly, so I used only the cone instead. I didn't use the box alone because it wouldn't look like a Roman temple without a tapered roof. 
- This scene ended up being made up of 18 objects in total: 6 box objects, 5 cylinder objects, 4 sphere objects, 1 cone object, and 1 neptune object. 


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

- Precision Error in KDTree Traversal for hits. 
When P and Q are close to the split plane, there is an issue with checking the triangles. 
It seems as though the triangle indices for the left and right are not actually in lists, however, 
manual assignment does not seem to fix this. This is causing the ray to not hit all the internal nodes. 
As such, we left the KDTree implementation in, but chose to not use our KD raytracer renderer for our 
submission images so we could turn in on time. 

**SIDE NOTE**
We accidentally put the shadows on our to-do list because we thought they were for this assignment. 
We commented out the implementation. But, if it's possible we would appreciate some feedback on it. 
**Citations**
White.ppm - I opened MSPaint and just downloaded the png then converted to ppm.
Checkerboard.ppm - I took it from the example LightsAndTextures from class.