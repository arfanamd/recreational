#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define COLOR1  CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR7  CLITERAL(Color){   0,   0,   0, 255 }
#define BG CLITERAL(Color){ 39, 55, 77, 255 }
#define BT CLITERAL(Color){ 82, 109, 130, 77 }
#define ST CLITERAL(Color){ 221, 230, 237, 77 }
#define FG CLITERAL(Color){ 221, 230, 237, 255 }
#define IB CLITERAL(Color){ 47, 68, 98, 255 }

#define FONT_SIZE 16
#define MAX_BUF 9

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
	if (hovered) RoundedRectwStroke(rec, BT, FG);
	DrawTextEx(font, str, pos, FONT_SIZE, 0, FG);
	return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

bool PushButton(const char *str, Rectangle rec, Vector2 pos, Font font) {
	bool hovered = CheckCollisionPointRec(GetMousePosition(), rec);
	if (hovered) RoundedRectwStroke(rec, BT, FG);
	else RoundedRectwStroke(rec, ST, FG);
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
	InputField inputs[12];
} InputObject;

void UpdateInput(InputField *m, Font font) {
	Vector2 mouseP = GetMousePosition();
	
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		m->focus = CheckCollisionPointRec(mouseP, m->rec);

	if (m->focus) {
		int key = GetCharPressed();
		if (m->buflen < MAX_BUF - 1) {
			switch (key) {
				case 45: case 48: case 49:
				case 50: case 51: case 52: case 53:
				case 54: case 55: case 56: case 57:
					m->buffer[m->buflen++] = (char)key;
					m->buffer[m->buflen] = '\0';
					break;
			}
		}
		if (IsKeyPressed(KEY_BACKSPACE) && m->buflen > 0) m->buffer[m->buflen--] = '\0';
	}

	RoundedRectwStroke(m->rec, m->focus ? ST : IB, FG);
	DrawTextEx(font, m->buflen == 0 ? m->label : m->buffer, m->pos, FONT_SIZE, 0, FG);
}

bool CheckGauss(InputObject *gi) {
	for (size_t i = 0; i < 12; i++)
		if (gi->inputs[i].buflen == 0) return false;
	return true;
}

int CountGauss(InputObject *gi, double m[3][4], Font font) {
	const size_t NEQ = 3;
	const size_t COL = 4;

	size_t k = 0;
	for (size_t i = 0; i < NEQ; i++)
		for (size_t j = 0; j < COL; j++) m[i][j] = strtod(gi->inputs[k++].buffer, NULL);

	for (size_t i = 0; i < NEQ; i++) {
		int pivot = i;

		// pivot selection by absolute value
		for (size_t r = i + 1; r < NEQ; r++) {
			double vR = m[r][i]; //*(m + r * COL + i);
			double vP = m[pivot][i]; //*(m + pivot * COL + i);
			if ((vR < 0 ? -vR : vR) > (vP < 0 ? -vP : vP)) pivot = r;
		}

		// no solution. 0 pivot.
		//if (*(m + pivot * COL + i) == 0.0) return 1;
		if (m[pivot][i] == 0.0) return 0;

		// If the best pivot row isn’t the current row, it swaps them so
		// the pivot is in the right place.
		if (pivot != i) {
			for (size_t k = 0; k < COL; k++) {
				double tmp = m[i][k]; //*(m + i * COL + k);
				m[i][k] = m[pivot][k]; //*(m + i * COL + k) = *(m + pivot * COL + k);
				m[pivot][k] = tmp; //*(m + pivot * COL + k) = tmp;
			}
		}

		// Scale pivot row to make pivot = 1
		double pivotVal = m[i][i]; //*(m + i * COL + i);
		for (size_t k = 0; k < COL; k++) m[i][k] /= pivotVal; //*(m + i * COL + k) /= pivotVal;

		// Eliminate this column in all other rows
		// This ensures you end up with 0s above and below the pivot
		for (size_t r = 0; r < NEQ; r++) {
			if (r == i) continue;
			double factor = m[r][i]; //*(m + r * COL + i);
			for (size_t k = 0; k < COL; k++) m[r][k] -= factor * m[i][k]; //*(m + r * COL + k) -= factor * *(m + i * COL + k);
		}
	}

	return 1;
}

void ResetGauss(InputObject *gi) {
	for (size_t i = 0; i < 12; i++) {
		gi->inputs[i].buffer[0] = '\0';
		gi->inputs[i].buflen = 0;
	}
}

int main(void) {
	InitWindow(screenWidth, screenHeight, "Metode Numerik");
	SetTargetFPS(30);

	const char *text_reference = "101010101";

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

	InputObject gi;

	gi.inputs[0] = (InputField){
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
	gi.inputs[1] = (InputField){
		.pos = {
			.x = gi.inputs[0].rec.width + gi.inputs[0].pos.x + 35,
			.y = gi.inputs[0].pos.y,
		},
		.rec = {
			.x = gi.inputs[0].rec.width + gi.inputs[0].pos.x + 25,
			.y = gi.inputs[0].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X2:",
	};
	gi.inputs[2] = (InputField){
		.pos = {
			.x = gi.inputs[1].rec.width + gi.inputs[1].pos.x + 35,
			.y = gi.inputs[1].pos.y,
		},
		.rec = {
			.x = gi.inputs[1].rec.width + gi.inputs[1].pos.x + 25,
			.y = gi.inputs[1].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X3:",
	};
	gi.inputs[3] = (InputField){
		.pos = {
			.x = gi.inputs[2].rec.width + gi.inputs[2].pos.x + 35,
			.y = gi.inputs[2].pos.y,
		},
		.rec = {
			.x = gi.inputs[2].rec.width + gi.inputs[2].pos.x + 25,
			.y = gi.inputs[2].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X:",
	};

	gi.inputs[4] = (InputField){
		.pos = {
			.x = gi.inputs[0].pos.x,
			.y = gi.inputs[0].rec.height + gi.inputs[0].pos.y + 35,
		},
		.rec = {
			.x = gi.inputs[0].rec.x,
			.y = gi.inputs[0].rec.height + gi.inputs[0].pos.y + 30,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X1:",
	};
	gi.inputs[5] = (InputField){
		.pos = {
			.x = gi.inputs[4].rec.width + gi.inputs[4].pos.x + 35,
			.y = gi.inputs[4].pos.y,
		},
		.rec = {
			.x = gi.inputs[4].rec.width + gi.inputs[4].pos.x + 25,
			.y = gi.inputs[4].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X2:",
	};
	gi.inputs[6] = (InputField){
		.pos = {
			.x = gi.inputs[5].rec.width + gi.inputs[5].pos.x + 35,
			.y = gi.inputs[5].pos.y,
		},
		.rec = {
			.x = gi.inputs[5].rec.width + gi.inputs[5].pos.x + 25,
			.y = gi.inputs[5].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X3:",
	};
	gi.inputs[7] = (InputField){
		.pos = {
			.x = gi.inputs[6].rec.width + gi.inputs[6].pos.x + 35,
			.y = gi.inputs[6].pos.y,
		},
		.rec = {
			.x = gi.inputs[6].rec.width + gi.inputs[6].pos.x + 25,
			.y = gi.inputs[6].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X:",
	};

	gi.inputs[8] = (InputField){
		.pos = {
			.x = gi.inputs[4].pos.x,
			.y = gi.inputs[4].rec.height + gi.inputs[4].pos.y + 35,
		},
		.rec = {
			.x = gi.inputs[4].rec.x,
			.y = gi.inputs[4].rec.height + gi.inputs[4].pos.y + 30,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X1:",
	};
	gi.inputs[9] = (InputField){
		.pos = {
			.x = gi.inputs[8].rec.width + gi.inputs[8].pos.x + 35,
			.y = gi.inputs[8].pos.y,
		},
		.rec = {
			.x = gi.inputs[8].rec.width + gi.inputs[8].pos.x + 25,
			.y = gi.inputs[8].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X2:",
	};
	gi.inputs[10] = (InputField){
		.pos = {
			.x = gi.inputs[9].rec.width + gi.inputs[9].pos.x + 35,
			.y = gi.inputs[9].pos.y,
		},
		.rec = {
			.x = gi.inputs[9].rec.width + gi.inputs[9].pos.x + 25,
			.y = gi.inputs[9].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X3:",
	};
	gi.inputs[11] = (InputField){
		.pos = {
			.x = gi.inputs[10].rec.width + gi.inputs[10].pos.x + 35,
			.y = gi.inputs[10].pos.y,
		},
		.rec = {
			.x = gi.inputs[10].rec.width + gi.inputs[10].pos.x + 25,
			.y = gi.inputs[10].rec.y,
			.width = MeasureTextEx(font, text_reference, FONT_SIZE, 0).x + 20,
			.height = MeasureTextEx(font, text_reference, FONT_SIZE, 0).y + 10,
		},
		.label = "X:",
	};

	Vector2 tboxGaussHitung = {
		.x = winEquationRect.x + 50,
		.y = winEquationRect.height * 40 / 100,
	};
	Rectangle rboxGaussHitung = {
		.x = tboxGaussHitung.x - 10,
		.y = tboxGaussHitung.y - 6,
		.width = MeasureTextEx(font, "HITUNG", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "HITUNG", FONT_SIZE, 0).y + 10,
	};
	Vector2 tboxGaussReset = {
		.x = winEquationRect.x + 400,
		.y = winEquationRect.height * 40 / 100,
	};
	Rectangle rboxGaussReset = {
		.x = tboxGaussReset.x - 10,
		.y = tboxGaussReset.y - 6,
		.width = MeasureTextEx(font, "RESET", FONT_SIZE, 0).x + 20,
		.height = MeasureTextEx(font, "RESET", FONT_SIZE, 0).y + 10,
	};
	
	Vector2 tboxGaussResult = {
		.x = winEquationRect.x + (winEquationRect.x * 5 / 100),
		.y = winEquationRect.height * 60 / 100 - 40,
	};
	Rectangle rboxGaussResult = {
		.x = tboxGaussResult.x,
		.y = tboxGaussResult.y,
		.width = winEquationRect.width - (winEquationRect.width * 5 / 100),
		.height = winEquationRect.height * 40 / 100,
	};


	double gaussMatrix[3][4] = {0};
	bool isGaussReady = false;
	int statusCountGauss = 0;
	Mode mode = NOOP;

	while (!WindowShouldClose()) {
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
		/* printf("%lf\n", MeasureTextEx(font, "WARNING: No unique solution", FONT_SIZE, 0).x); */
		/* printf("%lf\n", MeasureTextEx(font, "WARNING: No unique solution", FONT_SIZE, 0).y); */
		switch (mode) {
			case GAUSS:
				for (size_t i = 0; i < 12; i++) {
					UpdateInput(&gi.inputs[i], font);
					if ((i % 4) < 2)
						DrawTextEx(font, "+", (Vector2){.x = gi.inputs[i].pos.x + gi.inputs[i].rec.width + 4, .y = gi.inputs[i].pos.y}, FONT_SIZE, 0, FG);
					else if ((i % 4) == 2)
						DrawTextEx(font, "=", (Vector2){.x = gi.inputs[i].pos.x + gi.inputs[i].rec.width + 4, .y = gi.inputs[i].pos.y}, FONT_SIZE, 0, FG);
				}
				if (PushButton("HITUNG", rboxGaussHitung, tboxGaussHitung, font))
					if (CheckGauss(&gi)) {
						isGaussReady = true;
						statusCountGauss = CountGauss(&gi, gaussMatrix, font);
					}

				DrawRectangleRounded(rboxGaussResult, 0.05f, 0, ST);
				DrawRectangleRoundedLinesEx(rboxGaussResult, 0.05f, 0, 0.01f, FG);

				if (!isGaussReady) {
					DrawTextEx(font, "WARNING: All values must be present", (Vector2){.x = tboxGaussResult.x + rboxGaussResult.width * 50 / 100 - 149, .y = tboxGaussResult.y + rboxGaussResult.height * 50 / 100}, FONT_SIZE, 0, FG);
				} else {
					if (statusCountGauss) {
						char buff[16];
						for (size_t i = 0; i < 3; i++) {
							for (size_t j = 0; j < 3; j++) {
								snprintf(buff, sizeof(buff), "%0.02g", gaussMatrix[i][j]);
								DrawTextEx(font, buff, (Vector2){.x = tboxGaussResult.x + 20 + (j*40), .y = tboxGaussResult.y + 20 + (i*30)}, FONT_SIZE, 0, FG);
							}
						}
						for (size_t j = 0; j < 3; j++) {
							snprintf(buff, sizeof(buff), "X%ld: %0.02g", j + 1, gaussMatrix[j][3]);
							DrawTextEx(font, buff, (Vector2){.x = tboxGaussResult.x + 20 + 150, .y = tboxGaussResult.y + 20 + (j*30)}, FONT_SIZE, 0, FG);
						}
					} else
						DrawTextEx(font, "WARNING: No unique solution", (Vector2){.x = tboxGaussResult.x + rboxGaussResult.width * 50 / 100 - 115, .y = tboxGaussResult.y + rboxGaussResult.height * 50 / 100}, FONT_SIZE, 0, FG);
				}

				if (PushButton("RESET", rboxGaussReset, tboxGaussReset, font)) {
					ResetGauss(&gi);
					isGaussReady = false;
				}
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
