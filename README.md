# Rusty-Engine v0.01
2D Game Engine

Rusty engine is a 2D game engine written in C++. It's a side project in which I'm trying to create my very own game engine. Development is slow however, as I'm only working on it in my spare time.
### Development is currently on-hold.

The engine is still in it's early development stages and is not yet ready for use. Engine is currently using SDL 2 for render, input and audio (SDL_mixer).
In the current state, engine is not even a good example, so please do not use it as such. Think of it more as an experiment.

# What does work?

- sprite rendering on three layers with support for basic transformations (flip, scale, rotate, translate)
- world and levels (with very basic level loader) 
- touch and mouse input handling
- simultaneous playback of 16 positional sound effects and one music track
- basic physics (velocity, torque, moment of inertia, drag, basic and compound colliders)
- collision detection and resolution along with physical response (except for rectangles - that is very buggy)

# Planned features

- Customizable number of layers with support for parallax scrolling
- Render layers
- Editor
- Support for tile-based levels (tilemaps)
- Simple real-time lighting
- Built-in light baking
- Post process effects
- Particle systems
- 2D physics
- Pre-computed and real-time 2D global illumination
- Touch gestures and gamepad support
- Raw input support for mouse

# Current plans

- Fix class hierarchy or rethink the whole structure (which is a giant blob of circular dependency right now)
- Extend input options to full keyboard and mouse support (currently only mouse and touch inputs are supported)
- Fully implement menu and gui elements
- Create editor for the engine
- Replace my poor custom 2D physics with third-party physics engine (current candidates: Box2D, Chipmunk2D, Newton Game Dynamics, Physx)
- Implement other planned features

# Documentation

There is no documentation at this time, but most of the code is well commented (even over commented).

# Binaries
There are no binaries avalible at this time.


## Q&A

### Can I use it in a commercial application? What licence does it use?

Yes you can, Rusty Engine is avalible under the terms of the GNU Affero General Public License v3.0. Refer to LICENCE for more information.

### Is it cross-platform?

Thankfully SDL takes care of the most platform specifics, so the engine is currently not really platform bound. However it is a bit tricky to get it compiled on other platforms in it's current state. It requires C++11 compiler and you'll have to link required SDL libraries. In some cases you may have to add some forward declarations or fix some smaller things that Visual Studio fixes for you. Most of which are a result of my lacking knowledge of C++ or just pure laziness.
There is a known problem when working with iOS. SDL window creation and renderer initialization produce black screen, unless resolution of window is set to (0, 0). Check and edit World::init() function or change resolution constants.

### Do you plan to finish, maintain and upgrade it?

Yes, I do want to finish it, and also make a few games with it. I will keep engine up to date as long as I have the need for it, or as long as anyone finds use for it. Upgrades however will come from community demand or personal ideas for new features.

### Can I help?

I appreciate your enthusiasm, but for now I want to continue development on my own.

### What is all this junk in main.cpp?

That's the game logic from my school assignment. I only had few hours to write it, so I never fully seperated it from the engine, since it was easier to test engine changes this way. I'm keeping it for now, since it is also the only example avalible at the moment, even if it's a bad one.
