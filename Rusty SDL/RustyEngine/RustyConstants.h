#ifndef RUSTYENGINE_RUSTYCONSTANTS_H
#define RUSTYENGINE_RUSTYCONSTANTS_H

#pragma once

namespace RustyEngine
{
	// Window and render size limits (currently also settings)
	#define WINDOW_MIN_X 1920
	#define WINDOW_MIN_Y 1080
	#define RENDER_MIN_X 1920
	#define RENDER_MIN_Y 1080

	// Conversion
	#define RAD_TO_DEG 57.29577951f
	#define DEG_TO_RAD 0.017453293f

	// Various
	#define GAMEOBJECTS_START_ACTIVE true	// when true, new gameobjects will be active by default
	#define COMPONENTS_START_ACTIVE true	// when true, new components will be active by default
	#define AUDIO_MIN_DISTANCE 0.01f		// minimum distance before treating distance between source and listener equal to 0
	#define RENDER_SCALE 4.0f
	#define RENDER_PIXEL_PERFECT true		// when true, objects will rendered at pixel perfect position on screen (works best with nearest neighbour rendering)
}

#endif // RUSTYENGINE_RUSTYCONSTANTS_H