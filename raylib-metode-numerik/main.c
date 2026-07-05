#include <stdio.h>
#include "raylib.h"

#define COLOR1  CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR7  CLITERAL(Color){   0,   0,   0, 255 }
#define BG CLITERAL(Color){ 39, 55, 77, 255 }
#define BT CLITERAL(Color){ 82, 109, 130, 77 }
#define ST CLITERAL(Color){ 221, 230, 237, 77 }
#define FG CLITERAL(Color){ 221, 230, 237, 255 }

#define FONT_SIZE 16
#define MAX_BUF 16

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

bool MenuButton(const char *str, Rectangle rec, Vector2 pos, Font font) {
	bool hovered = CheckCollisionPointRec(GetMousePosition(), rec);
	if (hovered)
		RoundedRectwStroke(rec, BT, FG);
	DrawTextEx(font, str, pos, FONT_SIZE, 0, FG);
	return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

typedef enum {
	GAUSS,
	SECANT,
	LAGRANGE,
	EULER,
	NOOP,
} Mode;

typedef struct {
	Rectangle rec;
	Vector2 pos;
	char buffer[MAX_BUF];
	size_t buflen;
	bool focus;
	const char *label;
} InputField;

typedef struct {
	InputField inputs[3];
	size_t count;
} ModeInput;

void UpdateInput(InputField *m, Font font) {
	Vector2 mouseP = GetMousePosition();
	const char *str = "Input...";
	
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		m->focus = CheckCollisionPointRec(mouseP, m->rec);
	
	if (m->focus) {
		int key = GetCharPressed();
		if (m->buflen < MAX_BUF - 1) {
			switch (key) {
				case 43: case 45: case 48: case 49:
				case 50: case 51: case 52: case 53:
				case 54: case 55: case 56: case 57:
				case 61: case 120:
					m->buffer[m->buflen++] = (char)key;
					m->buffer[m->buflen] = '\0';
					break;
			}
		}
		if (IsKeyPressed(KEY_BACKSPACE) && m->buflen > 0) m->buffer[m->buflen--] = '\0';
	}
	
	RoundedRectwStroke(m->rec, m->focus ? BT : FG, FG);
	DrawTextEx(font, m->buflen == 0 ? str : m->buffer, m->pos, FONT_SIZE, 0, FG);
}

int main(void) {
	InitWindow(screenWidth, screenHeight, "Metode Numerik");
	SetTargetFPS(30);
	
	const char *text_reference = "1010101010101010";
	
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
	Mode mode = NOOP;
	InputField gaussInput = {
		.pos = {
			.x = winEquationRect.x + 35,
			.y = winEquationRect.y + 55,
		},
		.rec = {
			.x = winEquationRect.x + 25,
			.y = winEquationRect.y + 50,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X1:",
	};
	bool input1focus = false;
	
	while (!WindowShouldClose()) {
		mouseP = GetMousePosition();
		BeginDrawing();
		{
			ClearBackground(COLOR7);
			RectwStroke(winAboutRect, BG, FG);
			RectwStroke(winMenuRect, BG, FG);
			RectwStroke(winEquationRect, BG, FG);
			
			DrawTextureV(iconBannerTex, iconBannerPos, COLOR1);
			DrawTextEx(font, tboxMessage, tboxAboutPos, FONT_SIZE, 0, FG);
			
			DrawTextEx(font, "Main Menu", tboxMainMenu, FONT_SIZE, 0, FG);
			
			if (MenuButton("Metode Eliminasi Gauss", rboxMenuGauss, tboxMenuGauss, font)) mode = GAUSS;
			if (MenuButton("Metode Secant", rboxMenuSecant, tboxMenuSecant, font)) mode = SECANT;
			if (MenuButton("Metode Lagrange", rboxMenuLagrange, tboxMenuLagrange, font)) mode = LAGRANGE;
			if (MenuButton("Metode Euler", rboxMenuEuler, tboxMenuEuler, font)) mode = EULER;
			if (MenuButton("Exit", rboxMenuExit, tboxMenuExit, font)) goto quit;
		}
		switch (mode) {
			case GAUSS:
				UpdateInput(&gaussInput, font);
				/* if (CheckCollisionPointRec(mouseP, rboxGaussInput1)) { */
				/* 	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) input1focus = true; */
				/* } else { */
				/* 	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) input1focus = false; */
				/* } */
				/* RoundedRectwStroke(rboxGaussInput1, BT, FG); */
				/* DrawTextEx(font, input1focus ? "Input...":"Clicked...", tboxGaussInput1, FONT_SIZE, 0, ST); */
				break;
			default:
				break;
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
