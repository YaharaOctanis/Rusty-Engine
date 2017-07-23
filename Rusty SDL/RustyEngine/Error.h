#ifndef RUSTYENGINE_ERROR_H
#define RUSTYENGINE_ERROR_H

#pragma once

namespace RustyEngine
{
	// System error codes
	#define OUT_OF_MEMORY 10

	// Error codes for file access
	#define FILE_ACCESS_ERROR 100

	// Error codes for input handler
	#define RAWINPUT_REGISTER_FAILED 120
	#define RAWINPUT_GETINPUTBUFFER_FAILED 121

	// Error codes for SDL Renderer
	#define SDL_VERSION_MISMATCH 200
	#define SDL_INIT_ERROR 201
	#define SDL_SURFACE_ERROR 202
	#define SDL_THREAD_ERROR 203
	#define SDL_TEXTURE_ERROR 204
}

#endif // RUSTYENGINE_ERROR_H