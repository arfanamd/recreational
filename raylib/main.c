#include <stdio.h>
#include "raylib.h"

#define FPS 30
#define MAXBUF 16

typedef struct {
	Color foreground;
	Color background;
	Color altground;
	Color stroke;
} ColorProp;

typedef struct {
	Rectangle rect;
	Vector2   vect;
	ColorProp color;
	char      *label;
	float     font_s;
} Button;

typedef struct {
	Rectangle rect;
	Vector2   vect;
	ColorProp color;
	char      *label;
	char      buffer[MAXBUF];
	size_t    buflen;
	float     font_s;
	bool      focus;
} Input;

Button InitButton(float x, float y, char *label, Font font, float font_size, ColorProp color_prop) {
	const float padding = 10;
	Vector2 label_size = MeasureTextEx(font, label, font_size, 0);
	Button bt = {
		.rect   = {
			.x      = x <= 0 ? 0 : x - (label_size.x * 50 / 100),
			.y      = y <= 0 ? 0 : y - (label_size.y * 50 / 100),
			.width  = label_size.x + (label_size.x * 2.5f / 100) + padding,
			.height = label_size.y + (label_size.y * 2.5f / 100) + padding,
		},
		.vect   = {
			.x      = x <= 0 ? (padding / 2) : x - (label_size.x * 50 / 100) + (padding/2),
			.y      = y <= 0 ? (padding / 2) : y - (label_size.y * 50 / 100) + (padding/2),
		},
		.color  = color_prop,
		.label  = label,
		.font_s = font_size,
	};
	
	return bt;
}

Input InitInput(float x, float y, char *label, Font font, float font_size, ColorProp color_prop) {
	const float padding = 10;
	Vector2 label_size = MeasureTextEx(font, label, font_size, 0);
	Input in = {
		.rect   = {
			.x      = x <= 0 ? 0 : x - (label_size.x * 50 / 100),
			.y      = y <= 0 ? 0 : y - (label_size.y * 50 / 100),
			.width  = label_size.x + (label_size.x * 2.5f / 100) + padding,
			.height = label_size.y + (label_size.y * 2.5f / 100) + padding,
		},
		.vect   = {
			.x      = x <= 0 ? (padding / 2) : x - (label_size.x * 50 / 100) + (padding/2),
			.y      = y <= 0 ? (padding / 2) : y - (label_size.y * 50 / 100) + (padding/2),
		},
		.color     = color_prop,
		.label     = label,
		.buffer[0] = '\0',
		.buflen    = 0,
		.font_s    = font_size,
		.focus     = 0,
	};
	
	return in;
}

void CheckButton(const Button *bt, Font font) {
	fprintf(stderr, "rect:\n  x: %f\n  y: %f\n  w: %f\n  h: %f\n", bt->rect.x, bt->rect.y, bt->rect.width, bt->rect.height);
	fprintf(stderr, "vect:\n  x: %f\n  y: %f\n", bt->vect.x, bt->vect.y);
	fprintf(stderr, "color:\n  fg: %d\n  bg: %d\n  alt: %d\n  st: %d\n", ColorToInt(bt->color.foreground), ColorToInt(bt->color.background), ColorToInt(bt->color.altground), ColorToInt(bt->color.stroke));
	fprintf(stderr, "label:\n  text: %s\n", bt->label);
	fprintf(stderr, "font:\n  size: %f\n", bt->font_s);
}

bool RoundedButton(const Button *bt, Font font) {
	float roundness = 0.3f;
	bool  hovered   = CheckCollisionPointRec(GetMousePosition(), bt->rect);
	
	DrawRectangleRounded(bt->rect, roundness, 0, hovered ? bt->color.altground : bt->color.background);
	DrawRectangleRoundedLinesEx(bt->rect, roundness, 0, hovered ? 3.0f : 0.3f, bt->color.stroke);
	DrawTextEx(font, bt->label, bt->vect, bt->font_s, 0, bt->color.foreground);
	
	return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

bool RoundedInput(Input *in, Font font) {
	printf("focus: %d\n", in->focus);
	float roundness = 0.3f;
	bool  hovered   = CheckCollisionPointRec(GetMousePosition(), in->rect);
	
	if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		in->focus = 1;
		if (in->focus) {
			if (in->buflen < MAXBUF - 1) {
				int key = GetCharPressed();
				if (key >= 32 && key <= 126) {
					in->buffer[in->buflen++] = (char)key;
					in->buffer[in->buflen]   = '\0';
				}
			}
			if (IsKeyPressed(KEY_BACKSPACE) && in->buflen > 0) {
				in->buffer[in->buflen--] = '\0';
			}
		}
	} else {
		in->focus = 0;
	}
	
	DrawRectangleRounded(in->rect, roundness, 0, hovered || in->focus ? in->color.altground : in->color.background);
	DrawRectangleRoundedLinesEx(in->rect, roundness, 0, hovered ? 3.0f : 0.3f, in->color.stroke);
	DrawTextEx(font, in->buflen == 0 || !in->focus ? in->label : in->buffer, in->vect, in->font_s, 0, in->color.foreground);
	
	return (in->focus);
}

int main(void) {
	const int screen_width  = 1080; //GetScreenWidth();
	const int screen_height = 720;  //GetScreenHeight();
	InitWindow(screen_width, screen_height, "Recreational");
	SetTargetFPS(FPS);
	const Font font = LoadFont("AzeretMono-Bold.ttf");
	
	ColorProp color_properties = {
		.foreground = GetColor(0xf1f2f6),
		.background = GetColor(0x3867d6),
		.altground  = GetColor(0x45aaf2),
		.stroke     = GetColor(0xd1d8e0),
	};
	
	Button b = InitButton(
		(float)(screen_width*50/100),
		(float)(screen_height*50/100),
		"button 1",
		font,
		16.0f,
		color_properties
	);

	Input i = InitInput(
		(float)(screen_width*50/100),
		(float)(screen_height*30/100),
		"input 1...",
		font,
		16,
		color_properties
	);
	
	while (!WindowShouldClose()) {
		BeginDrawing(); {
			ClearBackground(BLANK);
			if (RoundedButton(&b, font)) fprintf(stderr, "%s: [CLICKED]\n", b.label);
			if (RoundedInput(&i, font)) fprintf(stderr, "[OK]\n");
		} EndDrawing();
	}
	
	UnloadFont(font);
	CloseWindow();
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
