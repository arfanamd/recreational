#include <stdio.h>
#include "raylib-6.0_linux_amd64/include/raylib.h"

#define COLOR1     CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR2     CLITERAL(Color){ 221, 221, 221, 255 }
#define COLOR3     CLITERAL(Color){ 203,  41,  87, 255 }
#define COLOR4     CLITERAL(Color){   0,   0,   0, 255 }

int main(void) {
	const size_t screenWidth  = 1080;
	const size_t screenHeight = 720;
	
	InitWindow(
		screenWidth,
		screenHeight,
		"Basic Window"
	);
	SetTargetFPS(30);
	
	fprintf(stderr, "[DEBUG]:screenWidth: %d\n[DEBUG]:screenHeight: %d\n",
		GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			ClearBackground(RAYWHITE);
			DrawRectangleLines(
				0,
				0,
				screenWidth*30/100,
				screenHeight,
				COLOR2
			);
			DrawRectangleLines(
				0,
				screenHeight*30/100,
				screenWidth*30/100,
				screenHeight-(screenHeight*30/100),
				COLOR3
			);
			DrawText(
				"This is window GUI",
				screenWidth*50/100,
				screenHeight*50/100,
				20,
				COLOR1
			);
		}
		EndDrawing();
		if (IsKeyPressed(KEY_Q)) CloseWindow();
	}
	
	CloseWindow();
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
