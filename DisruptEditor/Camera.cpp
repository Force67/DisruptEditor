#include "Camera.h"

#include "Common.h"
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

void Camera::update(float delta) {
	switch (type) {
		case FLYCAM:
		{
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			bool moveForward = state[settings.keyForward];
			bool moveBackward = state[settings.keyBackward];
			bool moveLeft = state[settings.keyLeft];
			bool moveRight = state[settings.keyRight];
			bool moveUp = state[settings.keyAscend];
			bool moveDown = state[settings.keyDescend];

			float actualMoveSpeed = delta * 10.f * settings.flyMultiplier;
			if (SDL_GetModState() & settings.keySlow)
				actualMoveSpeed /= 10.f;
			if (SDL_GetModState() & settings.keyFast)
				actualMoveSpeed *= 20.f;

			float dx = sinf(phi) * cosf(theta) * actualMoveSpeed;
			float dz = cosf(phi) * actualMoveSpeed;
			float dy = sinf(phi) * sinf(theta) * actualMoveSpeed;

			glm::vec3 movement;

			if (moveForward) {
				movement.x += dx;
				movement.y += dy;
				movement.z += dz;
			}
			if (moveBackward) {
				movement.x -= dx;
				movement.y -= dy;
				movement.z -= dz;
			}
			if (moveLeft) {
				movement.x += dy;
				//movement.y += dy;
				movement.y -= dx;
			}
			if (moveRight) {
				movement.x -= dy;
				//movement.y += dy;
				movement.y += dx;
			}

			location += movement;

			if (moveUp) location.z += actualMoveSpeed;
			if (moveDown) location.z -= actualMoveSpeed;

			float actualLookSpeed = delta * 10.f;

			int mouseX, mouseY;
			Uint32 mouseMask = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (!(mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)))
				actualLookSpeed = 0.f;

			lon += mouseX * actualLookSpeed;
			lat -= mouseY * actualLookSpeed;

			lat = max(-85, min(85, lat));
			phi = (90 - lat) * (3.141592f / 180.f);

			theta = (lon) * (3.141592f / 180.f);

			lookingAt.x = sinf(phi) * cosf(theta);
			lookingAt.z = cosf(phi);
			lookingAt.y = sinf(phi) * sinf(theta);

			lookingAt += location;
			break;
		}
		case ORBIT:
			break;
	}
}
