#ifndef RUSTYENGINE_RUSTYCONSTANTS_H
#define RUSTYENGINE_RUSTYCONSTANTS_H

#pragma once

namespace RustyEngine
{
	// Window and render size limits (currently also settings)
	#define WINDOW_MIN_X 800
	#define WINDOW_MIN_Y 600
	#define RENDER_MIN_X 800
	#define RENDER_MIN_Y 600

	// Various
	#define GAMEOBJECTS_START_ACTIVE true	// when true, new gameobjects will be active by default
	#define AUDIO_MIN_DISTANCE 0.01			// minimum distance before treating distance between source and listener equal to 0
}

#endif // RUSTYENGINE_RUSTYCONSTANTS_H