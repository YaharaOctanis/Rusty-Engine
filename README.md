# Rusty-Engine v0.01
2D Game Engine

This is the main repository of Rusty game engine. Development is currently on-hold.

The engine is still in it's early development stages and is not yet ready for use. Engine is currently using SDL 2 for render, input and audio (SDL_mixer).
In the current state, engine is not even a good example, so please do not use it as such.

# What does work?

- sprite rendering on three layers with support for basic transformations (flip, scale, rotation)
- world and levels (with basic level loader) 
- touch and mouse input handling
- simultaneous playback of 16 positional sound effects and one music track
- basic physics (velocity, torque, moment of inertia, drag, colliders)
- collision detection and resolution along with physical response (except for rectangles, that is a bit buggy)

# Planned features

- Unlimited number of layers
- Render layers
- Editor
- Parallax scrolling
- Support for tile-based worlds (tilemaps)
- Simple real-time lighting
- Built-in light baking
- Post process effects
- Particle systems
- 2D physics
- Pre-computed and real-time 2D positional audio
- Touch and gamepad support

# Current roadmap

- Fix class hierarchy (currently this is a giant blob of circular dependency)
- Extend input options to full keyboard and mouse support (currently only mouse and touch inputs are supported)
- Fully implement menu and gui elements
- Create editor
- Replace my poor custom 2D physics with third-party physics engine (current candidates: Box2D, Chipmunk2D, Newton Game Dynamics, Physx)
- Implement other planned features

# Documentation

There is no documentation at this time, but most of the code is well commented (even over commented).

# Binaries
There are no binaries avalible at this time.


## Q&A

### Can I use it in a commercial application? What licence does it use?

Yes, Rusty Engine is avalible under the terms of the GNU Affero General Public License v3.0. Refer to LICENCE for more information.

### Is it cross-platform?

Thankfully SDL takes care of the most platform specifics, so the engine is currently not really platform bound. However it is a bit tricky to get it compiled on other platforms in the current state. It requires C++11 compiler and you'll have to link required SDL libraries. In some cases you may have to add some forward declarations or fix some smaller things that Visual Studio fixes for you. As my C++ knowledge is lacking.
There is a known problem when working with iOS. SDL window creation and renderer initialization produce black screen, unless resolution of window is set to (0, 0). Check World::init() function or change resolution constants.

### Do you plan to finish, maintain and upgrade it?

Yes, I do want to finish it, while also make a few games with it. I will keep engine up to date as long as I have the need for it, or as long as the community finds use for it. Upgrades however will come from community demand or personal ideas for new features.

### How can I help?

My attempt at creating collisions with physical response was rather horrible. To be honest I only bothered with it since assignement didn't allow the use of existing libraries. So... If you have the skills or if you like the challenge, you can fork repository, create fully working physics for rigidbodies and create a pull request. I'll check it out and if I'm satisfied with it, I'll merge it with master. Otherwise I'll just implement third-party physics engine of my choice.

But for other features I want to continue development on my own. However you can still help, by using the engine to help uncover bugs. Fixing my grammar also helps. Also by telling others about the engine helps by motivating me to work on it.

### What is all this junk in main.cpp?

That's the game logic from my school assignment. I only had few hours to write it, so I never fully seperated it from the engine, since it was easier to test engine changes this way. I'm keeping it for now, since it is also the only example avalible at the moment, even if it's a bad one.
