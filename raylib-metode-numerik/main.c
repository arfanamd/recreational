#include <stdio.h>
#include "raylib.h"

#define COLOR1  CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR7  CLITERAL(Color){   0,   0,   0, 255 }
#define BLACK   CLITERAL(Color){ 027,  38, 044, 255 }
#define NAVY    CLITERAL(Color){ 015, 076, 117, 255 }
#define BLUE    CLITERAL(Color){ 050, 130, 184, 255 }
#define WINTER  CLITERAL(Color){ 187, 225, 250, 255 }
#define BG CLITERAL(Color){ 39, 55, 77, 255 }
#define BT CLITERAL(Color){ 82, 109, 130, 77 }
#define ST CLITERAL(Color){ 221, 230, 237, 77 }
#define FG CLITERAL(Color){ 221, 230, 237, 255 }

#define FONT_SIZE 16
#define MAX_INPUT 16

const size_t screenWidth  = 1080;
const size_t screenHeight = 720;

const Rectangle winAboutRect = {
	.x = 1,
	.y = 1,
	.width = screenWidth * 30 / 100,
	.height = screenHeight * 30 / 100,
};
const Rectangle winMenuRect = {
	.x = 1,
	.y = winAboutRect.height,
	.width = screenWidth * 30 / 100,
	.height = screenHeight - winAboutRect.height,
};
const Rectangle winEquationRect = {
	.x = winAboutRect.width,
	.y = 1,
	.width = screenWidth - (screenWidth * 30 / 100),
	.height = screenHeight,
};

void RectwStroke(const Rectangle rec, Color fcolor, Color scolor) {
	DrawRectangleRec(rec, fcolor);
	DrawRectangleLinesEx(rec, 1.0f, scolor);
}

void RoundedRectwStroke(const Rectangle rec, Color fcolor, Color scolor) {
	const float roundess = 0.5f;
	DrawRectangleRounded(rec, roundess, 0, fcolor);
	DrawRectangleRoundedLinesEx(rec, roundess, 0, 0.01f, scolor);
}

typedef enum {
	GAUSS,
	SECANT,
	LAGRANGE,
	EULER,
	NOOP,
} Mode;

int main(void) {
	InitWindow(screenWidth, screenHeight, "Metode Numerik");
	SetTargetFPS(30);
	
	const Image iconBanner = LoadImage("assets/iconBanner.png");
	const Texture2D iconBannerTex = LoadTextureFromImage(iconBanner);
	const Vector2 iconBannerPos = {
		.x = (winAboutRect.width * 50 / 100) - (iconBanner.width * 50 / 100),
		.y = winAboutRect.height * 5 / 100,
	};
	
	const Font font = LoadFont("assets/fonts/AzeretMono-Bold.ttf");
	const char *tboxMessage = "Nama: Ahmad Arfan Maulana\nNPM: 202443500619\nMetode Numerik";
	const Vector2 tboxAboutPos = {
		.x = (winAboutRect.width * 50 / 100) - (MeasureText(tboxMessage, FONT_SIZE) * 50 / 100),
		.y = iconBannerPos.y + iconBanner.height + 10,
	};
	const Vector2 menuPos = {
		.x = 30,
		.y = winAboutRect.height + 20,
	};
	
	const Vector2 tboxMainMenu = {
		.x = winMenuRect.x + (winMenuRect.width * 10 / 100),
		.y = winMenuRect.y + 25,
	};
	
	const Vector2 tboxMenuGauss = {
		.x = winMenuRect.x + (winMenuRect.width * 10 / 100),
		.y = tboxMainMenu.y + 45,
	};
	const Rectangle rboxMenuGauss = {
		.x = tboxMenuGauss.x - 10,
		.y = tboxMenuGauss.y - 5,
		.width = MeasureTextEx(font, "Metode Eliminasi Gauss", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "Metode Eliminasi Gauss", FONT_SIZE, 0).y + 10,
	};
	
	const Vector2 tboxMenuSecant = {
		.x = winMenuRect.x + (winMenuRect.width * 10 / 100),
		.y = tboxMenuGauss.y + 35,
	};
	const Rectangle rboxMenuSecant = {
		.x = tboxMenuSecant.x - 10,
		.y = tboxMenuSecant.y - 5,
		.width = MeasureTextEx(font, "Metode Secant", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "Metode Secant", FONT_SIZE, 0).y + 10,
	};
	
	const Vector2 tboxMenuLagrange = {
		.x = winMenuRect.x + (winMenuRect.width * 10 / 100),
		.y = tboxMenuSecant.y + 35,
	};
	const Rectangle rboxMenuLagrange = {
		.x = tboxMenuLagrange.x - 10,
		.y = tboxMenuLagrange.y - 5,
		.width = MeasureTextEx(font, "Metode Lagrange", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "Metode Lagrange", FONT_SIZE, 0).y + 10,
	};
	
	const Vector2 tboxMenuEuler = {
		.x = winMenuRect.x + (winMenuRect.width * 10 / 100),
		.y = tboxMenuLagrange.y + 35,
	};
	const Rectangle rboxMenuEuler = {
		.x = tboxMenuEuler.x - 10,
		.y = tboxMenuEuler.y - 5,
		.width = MeasureTextEx(font, "Metode Euler", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "Metode Euler", FONT_SIZE, 0).y + 10,
	};
	
	const Vector2 tboxMenuExit = {
		.x = winMenuRect.x + (winMenuRect.width * 10 / 100),
		.y = tboxMenuEuler.y + 35,
	};
	const Rectangle rboxMenuExit = {
		.x = tboxMenuExit.x - 10,
		.y = tboxMenuExit.y - 5,
		.width = MeasureTextEx(font, "Exit", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "Exit", FONT_SIZE, 0).y + 10,
	};
	
	const Vector2 tboxGaussInput1 = {
		.x = winEquationRect.x + 35,
		.y = winEquationRect.y + 55,
	};
	const Rectangle rboxGaussInput1 = {
		.x = tboxGaussInput1.x - 10,
		.y = tboxGaussInput1.y - 5,
		.width = MeasureTextEx(font, "1010101010101010", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "1010101010101010", FONT_SIZE, 0).y + 10,
	};
	
	Vector2 mouseP = {0};
	char input[MAX_INPUT + 1] = "\0";
	size_t inputCounter = 0;
	Mode mode = NOOP;
	bool input1focus = false;
	
	while (!WindowShouldClose()) {
		mouseP = GetMousePosition();
		{
			ClearBackground(COLOR7);
			RectwStroke(winAboutRect, BG, FG);
			RectwStroke(winMenuRect, BG, FG);
			RectwStroke(winEquationRect, BG, FG);
			
			DrawTextureV(iconBannerTex, iconBannerPos, COLOR1);
			DrawTextEx(font, tboxMessage, tboxAboutPos, FONT_SIZE, 0, FG);
			
			DrawTextEx(font, "Main Menu", tboxMainMenu, FONT_SIZE, 0, FG);
			
			DrawTextEx(font, "Metode Eliminasi Gauss", tboxMenuGauss, FONT_SIZE, 0, FG);
			if (CheckCollisionPointRec(mouseP, rboxMenuGauss)) {
				RoundedRectwStroke(rboxMenuGauss, BT, FG);
				DrawTextEx(font, "Metode Eliminasi Gauss", tboxMenuGauss, FONT_SIZE, 0, FG);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					mode = GAUSS;
				}
			}
			
			DrawTextEx(font, "Metode Secant", tboxMenuSecant, FONT_SIZE, 0, FG);
			if (CheckCollisionPointRec(mouseP, rboxMenuSecant)) {
				RoundedRectwStroke(rboxMenuSecant, BT, FG);
				DrawTextEx(font, "Metode Secant", tboxMenuSecant, FONT_SIZE, 0, FG);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					fprintf(stderr, "[DEBUG]: Gauss Secant\n");
				}
			}
			
			DrawTextEx(font, "Metode Lagrange", tboxMenuLagrange, FONT_SIZE, 0, FG);
			if (CheckCollisionPointRec(mouseP, rboxMenuLagrange)) {
				RoundedRectwStroke(rboxMenuLagrange, BT, FG);
				DrawTextEx(font, "Metode Lagrange", tboxMenuLagrange, FONT_SIZE, 0, FG);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					fprintf(stderr, "[DEBUG]: Gauss Lagrange\n");
				}
			}
			
			DrawTextEx(font, "Metode Euler", tboxMenuEuler, FONT_SIZE, 0, FG);
			if (CheckCollisionPointRec(mouseP, rboxMenuEuler)) {
				RoundedRectwStroke(rboxMenuEuler, BT, FG);
				DrawTextEx(font, "Metode Euler", tboxMenuEuler, FONT_SIZE, 0, FG);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					fprintf(stderr, "[DEBUG]: Gauss Euler\n");
				}
			}
			
			DrawTextEx(font, "Exit", tboxMenuExit, FONT_SIZE, 0, FG);
			if (CheckCollisionPointRec(mouseP, rboxMenuExit)) {
				RoundedRectwStroke(rboxMenuExit, BT, FG);
				DrawTextEx(font, "Exit", tboxMenuExit, FONT_SIZE, 0, FG);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					goto quit;
				}
			}
		}
		switch (mode) {
			case GAUSS:
				RoundedRectwStroke(rboxGaussInput1, BT, FG);
				DrawTextEx(font, "Input...", tboxGaussInput1, FONT_SIZE, 0, ST);
				if (CheckCollisionPointRec(mouseP, rboxGaussInput1)) {
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) input1focus = true;
				} else {
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) input1focus = false;
				}
				if (input1focus) {
					RoundedRectwStroke(rboxGaussInput1, BT, FG);
					DrawTextEx(font, "Clicked...", tboxGaussInput1, FONT_SIZE, 0, ST);
				}
				break;
			default:
				break;
		}
		BeginDrawing();
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
