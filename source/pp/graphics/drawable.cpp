//
// Created by dareb on 8/8/2020.
//

#include "pp/graphics/drawable.h"
#include "pp/graphics/draw.h"
#include "pp/collections/vector.h"
#include <MATH.h>
#include <OS/OSError.h>

namespace PP {
namespace Graphics {

Renderables renderables;

Point::Point(GXColor color, float x, float y, int thickness, bool is2D) {
    drawKind = 0;
    this->delay = 0;
    this->lifeTime = 1;
    this->color = color;
    this->x = x;
    this->y = y;
    this->is2D = is2D;
}

Point::Point(int delay, int lifetime, GXColor color, float x, float y, int thickness, bool is2D) {
    drawKind = 0;
    this->delay = delay;
    this->lifeTime = lifetime;
    this->color = color;
    this->is2D = is2D;
    this->x = x;
    this->y = y;
    this->thickness = thickness;
}

void Point::draw() {
    draw2DPoint(this->color, this->x, this->y, this->thickness);
}

Line::Line(GXColor color, float x1, float y1, float x2, float y2, int thickness, bool is2D) {
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

Line::Line(int delay, int lifetime, GXColor color, float x1, float y1, float x2, float y2, int thickness, bool is2D) {
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

void Line::draw() {
    draw2DLine(color, x1, y1, x2, y2, thickness);
}
RectOutline::RectOutline(GXColor color, float top, float bottom, float left, float right, int thickness, bool is2D) {
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

RectOutline::RectOutline(int delay, int lifetime, GXColor color, float top, float bottom, float left, float right, int thickness, bool is2D) {
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


void RectOutline::draw() {
    draw2DRectOutline(color, top, bottom, left, right, thickness);
}
Rect::Rect(GXColor color, float top, float bottom, float left, float right, bool is2D) {
    drawKind = 3;
    this->delay = 0;
    this->lifeTime = 1;
    this->color = color;
    this->is2D = is2D;
    this->top = top;
    this->bottom = bottom;
    this->left = left;
    this->right = right;
}

Rect::Rect(int delay, int lifetime, GXColor color, float top, float bottom, float left, float right, bool is2D) {
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

Rect::Rect(float x, float y, float width, float height, bool is2D, GXColor color) {
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

void Rect::draw() {
    /*
    OSReport("addr: %08x\n", this);
    OSReport("rect draw: (col, t, b, l, r): %08x, %.3f, %.3f, %.3f, %.3f\n", color, top, bottom, left, right);
    */
    draw2DRectangle(color, top, bottom, left, right);
}

Circle::Circle(float delay, float lifeTime, float x, float y, float radius, int vertCount, bool is2D, GXColor color) : vertCount(vertCount) {
    drawKind = 4;
    this->delay = delay;
    this->lifeTime = lifeTime;
    this->x = x;
    this->y = y;
    this->radius = radius;
    this->is2D = is2D;

}

Circle::Circle(float x, float y, float radius, int vertCount, bool is2D, GXColor color) :
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


void Circle::draw() {
    Position3D* vertices = new Position3D[vertCount];
    const float step = 360.0 / (float) vertCount;
    float angle = 90;
    for (int i = 0; i < vertCount; i++) {
        float relX = cos(angle * MATH_RAD);
        float relY = sin(angle * MATH_RAD);
        vertices[i] = Position3D(x + relX * radius, y + relY * radius, 0);
        angle += step;
    }
    drawTriangleFan(color, vertices, vertCount);
    delete vertices;
}
CircleWithBorder::CircleWithBorder(float delay, float lifeTime, float x, float y, float radius, int vertCount, bool is2D, GXColor color, float borderThickness, GXColor borderColor):
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

CircleWithBorder::CircleWithBorder(float x, float y, float radius, int vertCount, bool is2D, GXColor color, float borderThickness, GXColor borderColor):
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

void CircleWithBorder::draw() {
    Position3D* vertices = new Position3D[vertCount];
    const float step = 360.0 / (float) vertCount;
    float angle = 90;
    for (int i = 0; i < vertCount; i++) {
        float relX = cos(angle * MATH_RAD);
        float relY = sin(angle * MATH_RAD);
        vertices[i] = Position3D(x + relX * borderRadius, y + relY * borderRadius, 0);
        angle += step;
    }
    drawTriangleFan(borderColor, vertices, vertCount);
    const float ratio = radius / borderRadius;
    for (int i = 0; i < vertCount; i++) {
        vertices[i] -= Position3D(x, y, 0);
        vertices[i] *= ratio;
        vertices[i] += Position3D(x, y, 0);
    }
    drawTriangleFan(color, vertices, vertCount);
    delete vertices;
}

void drawItem(Drawable * item) {
    setupDrawPrimitives();
    if (item->delay == 0) {
        if (item->is2D) { 
            start2DDraw(); 
        }
        else { startNormalDraw(); }
        item->draw();
        if (item->autoTimer && item->lifeTime > 0) { item->lifeTime --; }
    } else { if (item->autoTimer) { item->delay --; } }
}

void Renderables::renderAll() {
    for (int i = 0; i < items.frame.size(); i++) {
        Drawable* drawable = reinterpret_cast<Drawable*>(items.frame[i]);
        drawItem(drawable);
        if (drawable->lifeTime <= 0) {
            // delete items.frame[i];
            items.frame.erase(i); 
            delete drawable;
            i -= 1;
        }
    }
    for (int i = 0; i < items.tick.size(); i++) { 
        drawItem(reinterpret_cast<Drawable*>(items.tick[i]));
    }
}

void Renderables::renderPre() {
    for (int i = 0; i < items.preFrame.size(); i++) {
        Drawable* drawable = reinterpret_cast<Drawable*>(items.preFrame[i]);

        if (drawable != NULL) {
            drawItem(drawable);
        }
        if (drawable->lifeTime <= 0) {
            // delete items.preFrame[i];
            items.preFrame.erase(i); 
            delete drawable;
            i -= 1;
        }
    }
}

void Renderables::updateTick() {
    for (int i = 0; i < items.tick.size(); i++) {
        Drawable* drawable = reinterpret_cast<Drawable*>(items.tick[i]);
        if (drawable->delay == 0) {
            if (drawable->lifeTime <= 0) { 
                items.tick.erase(i); 
                delete drawable;
                i -= 1;
            } else {
                drawable->lifeTime --;
            }
        } else {
            drawable->delay --;
        }
    }
}

void Renderables::clearAll() {
    int i;
    for (i = 0; i < items.frame.size(); i++) {
        delete reinterpret_cast<Drawable*>(items.frame[i]);
    }
    items.frame.clear();
    for (i = 0; i < items.preFrame.size(); i++) {
        delete reinterpret_cast<Drawable*>(items.preFrame[i]);
    }
    items.preFrame.clear();
    for (i = 0; i < items.tick.size(); i++) {
        delete reinterpret_cast<Drawable*>(items.tick[i]);
    }
    items.tick.clear();
}

}} // namespace