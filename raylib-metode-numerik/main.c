#include <stdio.h>
#include "raylib-6.0_linux_amd64/include/raylib.h"

#define COLOR1     CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR2     CLITERAL(Color){ 128, 128, 128, 255 }
#define COLOR3     CLITERAL(Color){ 221, 221, 221, 255 }
#define COLOR4     CLITERAL(Color){ 175, 221, 233, 255 }
#define COLOR5     CLITERAL(Color){ 203,  41,  87, 255 }
#define COLOR7     CLITERAL(Color){   0,   0,   0, 255 }

#define BLACK   CLITERAL(Color){ 027,  38, 044, 255 }
#define NAVY    CLITERAL(Color){ 015, 076, 117, 255 }
#define BLUE    CLITERAL(Color){ 050, 130, 184, 255 }
#define WINTER  CLITERAL(Color){ 187, 225, 250, 255 }

void RectwStroke(Rectangle rec, Color color) {
	DrawRectangleRec(rec, color);
	DrawRectangleLinesEx(rec, 1.0f, NAVY);
}

void RectwStrokeGrad(Rectangle rec, Color color1, Color color2) {
	DrawRectangleGradientEx(rec, color1, color2, color2, color1);
	DrawRectangleLinesEx(rec, 2, NAVY);
}

int main(void) {
	const size_t screenWidth  = 1080;
	const size_t screenHeight = 720;
	
	const Rectangle winAboutRec = {
		.x = 1,
		.y = 1,
		.width = screenWidth * 30 / 100,
		.height = screenHeight * 30 / 100,
	};
	const Rectangle winMenuRec = {
		.x = 1,
		.y = winAboutRec.height,
		.width = screenWidth * 30 / 100,
		.height = screenHeight - winAboutRec.height,
	};
	const Rectangle winTitleRec = {
		.x = winAboutRec.width,
		.y = 1,
		.width = screenWidth - (screenWidth * 30 / 100),
		.height = screenHeight * 10 /100,
	};
	const Rectangle winEquationRec = {
		.x = winAboutRec.width,
		.y = winTitleRec.height,
		.width = screenWidth - (screenWidth * 30 / 100),
		.height = screenHeight - winTitleRec.height,
	};
	
	InitWindow(
		screenWidth,
		screenHeight,
		"Metode Numerik"
	);
	SetTargetFPS(30);
	
	const Image iconBanner = LoadImage("assets/iconBanner.png");
	const Texture2D iconBannerTex = LoadTextureFromImage(iconBanner);
	const Vector2 iconBannerPos = {
		.x = 100,
		.y = winAboutRec.height * 5 / 100,
	};
	
	const Font font = LoadFont("assets/fonts/LoveYaLikeASister-Regular.ttf");
	const Vector2 textPos = {
		.x = 80,
		.y = iconBannerPos.y + iconBanner.height + 10,
	};
	const char *textStr = "Nama: Ahmad Arfan Maulana\nNPM: 202443500619\nMetode Numerik";
	const Vector2 menuPos = {
		.x = 30,
		.y = winAboutRec.height + 20,
	};
	const char *menuStr =
		"Menu\n\nMetode Eliminasi Gauss Jordan\n\nMetode Secant\n\nMetode Lagrange\n\nMetode Euler";
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			ClearBackground(COLOR7);
			RectwStroke(winAboutRec, BLUE);
			RectwStroke(winMenuRec, WINTER);
			RectwStroke(winTitleRec, BLUE);
			RectwStroke(winEquationRec, WINTER);
			
			DrawTextureV(iconBannerTex, iconBannerPos, COLOR1);
			DrawTextEx(font, textStr, textPos, 16.0f, 1.0f, WINTER);
			DrawTextEx(font, menuStr, menuPos, 20.0f, 2.0f, NAVY);
		}
		EndDrawing();
		if (IsKeyPressed(KEY_Q)) goto quit;
	}
	
quit:
	UnloadImage(iconBanner);
	UnloadTexture(iconBannerTex);
	UnloadFont(font);
	CloseWindow();
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
