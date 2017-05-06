# Rusty-Engine v0.01
2D Game Engine

This is the main repository of Rusty game engine.

The engine is still in it's early development phase and is not yet ready for use. Engine is currently using SDL 2 for render, input and audio (SDL_mixer).
In the current state, engine is not even a good example, so please do not use it as such.

# What does even work right now?

- sprite rendering with support for basic transformations (flip, scale, rotation)
- world and levels (with basic level loader) 
- touch and mouse input handling
- audio playback and real-time positional audio
- basic physics (velocity, torque, moment of inertia, drag, colliders)
- collision detection and resolution along with physical response (except for rectangles)

# Planned features

- Editor
- Support for tile-based worlds
- Simple real-time lighting
- Built-in light baking
- Post process effects
- Particle systems
- 2D physics
- Pre-computed and real-time 2D positional audio
- Touch and gamepad support

# Roadmap

- Fix class hiearchy (currently this is a giant blob of circular dependency)
- Extend input options to full keyboard and mouse support (currently only mouse and touch inputs are supported)
- Fully implement menu and gui elements
- Create editor
- Replace custom 2D physics with third-party physics engine (current candidates: Box2D, Chipmunk2D, Newton Game Dynamics, Physx)
- Implement other planned features (starting with visuals)

# Documentation
There is no documentation at this time, but the code is well commented.

# Binaries
There are no binaries avalible at this time.
