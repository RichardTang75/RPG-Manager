# RPG-Manager

I wrote this to start developing a UI and handling basic game logic while avoiding the computational and code baggage of my other project. In addition, I wanted to work on increasing my understanding and my familiarity with certain aspects of the language (smart pointers, references, classes, polymorphism) while building something visually interesting.

I wanted to test out my idea for automatically generating provinces using perturbed voronoi noise. Each voronoi polygon would be a province of sorts, but the difficulty lay in how to take an array of unique values (each value corresponding to a polygon) generated from a third party library and put it into useable form with boundaries. I wantd to link events to provinces, choosing a random location then finding the province in which the event lay in and associating the two. Each province would store its own values and be modified by an event. Furthermore, it would be nice to have a province overlay option, so each province would preferably store its image and location for drawing. 

I decided to go through each member of the array, store a map of the unique value, and the column and row indices in which the unique values existed, then building the image and allowing for a way to determine if a coordinate exists in a certain province.

Since I had written the province code early on, everytime I wanted to run the program when I wanted to debug a certain feature, I would have to wait for the province code to run through. I had previously mass-commented things out, but I had used conditional preprocessor definitions before and wondered if there was a better way to turn code on or off. I found out about feature flagging and currently use it to skip this code when focusing on UI development.

The UI class is somewhat inspired by HTML's z-indexing and was written so that a single loop would deal with all the windows at once, going in order of z-index, then having a unique identifier for each window allowing me to dynamic cast or use virtual functions to do each window's specialized drawing or handle each window's clicks.
