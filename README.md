# assignment8 

**Features**
- Upon starting, you show the model passed in as a .txt file and the text hierarchy in terminal.
- pressing S will cause a raytracer to run on the scene. The calculations take into account shadows, reflections, and shading. 
- KDTree's allow for efficient raytracing.

**Implementation Details.**
We started off with Assignment 7. 
- The first thing we did was fix the KDTree to render the tree properly. This allowed us to move past the brute force
method that was implemented in the RaytraceMesh.
- We then uncommented the shadow function. We accidentally implemented this during our first Raytracer implementation so
it was already complete.
- Finally, we used recursion as recommended to calculate the reflection of the objects by creating new rays as recommended
on the assignment page.

- Karys did the new model which is described in the Extra Credit Section.
- Andres did the reflections. 
- Shadows were already complete and were a combined effort.

**Extra Credit**
Texture Mapping: We have the math for texture mapping inside of Hit Record and the RaytraceMesh. These were not fully showcased in the KD Tree and we have no png's to showcase it.
- The math was entirely based off of the image given in the assignment description (in the helper method to compute it). 

Refraction: The refraction uses Snell's law as recommended along with taking in transparency and the refraction-index. This can be
seen in the raytracerRefraction.png in the images.

Scene: For the extra credit scene I wanted to use the neptune model that's in ObjViewer. So, I planned the entire scene around the single neptune model (I was absolutely deadset on using it as the complex model). However, the neptune object doesn't work well with the neptune model, so I made a temple dedicated to aeroplanes. 
- I physically drew out a mausoleum/temple to house the neptune object as a statue to start. I prefer to do this because it gives me a base for how everything should look together (it's great for visualization and confirmed that my ideal scene was over the required 15 objects). 
- After, I started to define materials, I wanted to use beiege and grey tones for the temple itself, but keep neptune as a stand-out figure with its color. This took a couple of tries to get the look I was hoping for. 
- I then started with defining a base for the temple, then a pedestal for the neptune statue. 
- For the pedestal I really wanted it to look like an actual pedestal you'd find in a Roman temple, despite using primitive shapes. To do that, I made use a box as a base for the pedestal as a whole and then put 4 spheres ontop of the box base. I chose to use 4 spheres because I thought (and still think) it's reminiscent of the detailing between the base of a pedestal and the platform where a statue stands in a Roman temple. Finally, for the top of a pedestal, I used a flat cylinder to give the pedestal as a whole a round-ish shape (emphasis on ish) to mimic the taper effect actual pedestals have. 
- After I placed the neptune object ontop of the pedestal (which took a lot of fine-tuning in regards to scale and translations), I moved onto modeling the actual temple structure. 
- For the structure of the temple itself I continued to mimic Roman architecture. This meant creating 4 columns for the temple. I made a box base for each column, then placed a cylinder (scaled to be very tall) on top of the box base. I think that this effectively created a column for each corner, despite having primitive shapes. 
- Finally, for the roof of the temple, I used a very stretched out cone. I had initially planned to use a box base for the roof and then a cone on top, but that ended up looking ugly, so I used only the cone instead. I didn't use the box alone because it wouldn't look like a Roman temple without a tapered roof. 
- This scene ended up being made up of 18 objects in total: 6 box objects, 5 cylinder objects, 4 sphere objects, 1 cone object, and 1 neptune object. 
- I avoided giving the model a back object because it would ruin the temple look of the model. 
- The KD tree doesn't seem to be able to handle the neptune object. So, ultimately, the statue of neptune was replaced with the plane object. This does ruin my initial scene concept, however, now the temple is more of a worship of planes than neptune. 

**Citations**
White.ppm - I opened MSPaint and just downloaded the png then converted to ppm.
Checkerboard.ppm - I took it from the example LightsAndTextures from class.