#ifndef PP_DRAWABLE
#define PP_DRAWABLE
//
// Created by dareb on 8/8/2020.
// ported to syriinge by wisp on 03/2024
//

#include <GX/GXTypes.h>
#include "pp/collections/vector.h"

// Math literal constants that brawl already uses.
#define ONE_RAD_IN_DEG (*(float*) 0x805a1eb4)
#define PI (*(float*) 0x805a37f0)
#define MIN_FLOAT (*(float*) 0x805a298c)
#define FLOAT_MIN_VALUE (*(float*) 0x805a2974)
#define ZERO (*(float*) 0x805a3780)
#define NEGATIVE_ONE (*(float*) 0x805a3784)
#define MATH_RAD (*(float*)0x805a4ad8)

#define LINE_SCALE 21

namespace ProjectPunch {
namespace Graphics {
using namespace ProjectPunch::Collections;

struct Drawable {
    Drawable() {
        lifeTime = 1;
        delay = 0;
        autoTimer = true;
    }

    virtual void draw() = 0;

    GXColor color;
    unsigned int lifeTime;
    unsigned int delay;
    bool autoTimer;
    bool is2D;
    int drawKind;
};

struct Point : Drawable {
    Point(GXColor color, float x, float y, int thickness, bool is2D) {
        drawKind = 0;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->x = x;
        this->y = y;
        this->is2D = is2D;
    }

    Point(int delay, int lifetime, GXColor color, float x, float y, int thickness, bool is2D) {
        drawKind = 0;
        this->delay = delay;
        this->lifeTime = lifetime;
        this->color = color;
        this->is2D = is2D;
        this->x = x;
        this->y = y;
        this->thickness = thickness;
    }

    void draw();
    float x;
    float y;
    int thickness;
};

struct Line : Drawable {
    Line(GXColor color, float x1, float y1, float x2, float y2, int thickness, bool is2D) {
        drawKind = 1;
        this->delay = 0;
        this->lifeTime = 1;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->thickness = thickness;
        this->is2D = is2D;
    };

    Line(int delay, int lifetime, GXColor color, float x1, float y1, float x2, float y2, int thickness, bool is2D) {
        drawKind = 1;
        this->delay = delay;
        this->lifeTime = lifetime;
        this->color = color;
        this->is2D = is2D;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->thickness = thickness;
    }

    void draw();
    float x1;
    float y1;
    float x2;
    float y2;
    int thickness;
};

struct RectOutline : Drawable {
    RectOutline(GXColor color, float top, float bottom, float left, float right, int thickness, bool is2D) {
        drawKind = 2;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->is2D = is2D;
        this->top = top;
        this->bottom = bottom;
        this->left = left;
        this->right = right;
        this->thickness = thickness;
    }

    RectOutline(int delay, int lifetime, GXColor color, float top, float bottom, float left, float right, int thickness, bool is2D) {
        drawKind = 2;
        this->delay = delay;
        this->lifeTime = lifetime;
        this->color = color;
        this->is2D = is2D;
        this->top = top;
        this->bottom = bottom;
        this->left = left;
        this->right = right;
        this->thickness = thickness;
    }

    void draw();
    float top;
    float bottom;
    float left;
    float right;
    int thickness;
};

struct Rect : Drawable {
    Rect(GXColor color, float top, float bottom, float left, float right, bool is2D) {
        drawKind = 3;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->is2D = is2D;
        this->top = top;
        this->bottom = bottom;
        this->left = left;
        this->right = right;
    };

    Rect(int delay, int lifetime, GXColor color, float top, float bottom, float left, float right, bool is2D) {
        drawKind = 3;
        this->delay = delay;
        this->lifeTime = lifetime;
        this->color = color;
        this->is2D = is2D;
        this->top = top;
        this->bottom = bottom;
        this->left = left;
        this->right = right;
    }

    Rect(float x, float y, float width, float height, bool is2D, GXColor color) {
        drawKind = 3;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->top = y + (height / 2);
        this->bottom = y - (height / 2);
        this->left = x - (width / 2);
        this->right = x + (width / 2);
        this->is2D = is2D;
    }

    void draw();
    float top;
    float bottom;
    float left;
    float right;
};

struct Circle : Drawable {
    Circle(float delay, float lifeTime, float x, float y, float radius, int vertCount, bool is2D, GXColor color): vertCount(vertCount) {
        drawKind = 4;
        this->delay = delay;
        this->lifeTime = lifeTime;
        this->x = x;
        this->y = y;
        this->radius = radius;
        this->is2D = is2D;

    }

    Circle(float x, float y, float radius, int vertCount, bool is2D, GXColor color):
        vertCount(vertCount) {
        drawKind = 4;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->x = x;
        this->y = y;
        this->radius = radius;
        this->is2D = is2D;
    }

    void draw();
    float x;
    float y;
    float radius;
    const int vertCount;
};

struct CircleWithBorder : Drawable {
    CircleWithBorder(float delay, float lifeTime, float x, float y, float radius, int vertCount, bool is2D, GXColor color, float borderThickness, GXColor borderColor):
        vertCount(vertCount) {
        drawKind = 4;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->x = x;
        this->y = y;
        this->radius = radius;
        this->is2D = is2D;
    }

    CircleWithBorder(float x, float y, float radius, int vertCount, bool is2D, GXColor color, float borderThickness, GXColor borderColor):
        vertCount(vertCount) {
        drawKind = 5;
        this->borderRadius = radius + borderThickness;
        this->borderColor = borderColor;
        this->delay = 0;
        this->lifeTime = 1;
        this->color = color;
        this->x = x;
        this->y = y;
        this->radius = radius;
        this->is2D = is2D;
    }

    void draw();
    float x;
    float y;
    float radius;
    float borderRadius;
    GXColor borderColor;
    const int vertCount;
};

template<class T>
struct RenderTimes {
    vector<T> preFrame;
    vector<T> frame;
    vector<T> tick;
};

struct Renderables {
    void renderPre();
    void renderAll();
    void updateTick();
    void clearAll();

    RenderTimes<Drawable *> items;
};

extern Renderables renderables;

}
}

#endif