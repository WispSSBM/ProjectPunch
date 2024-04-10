//
// Created by dareb on 8/8/2020.
//

#include "pp/graphics/drawable.h"
#include "pp/graphics/draw.h"
#include "pp/collections/vector.h"
#include <MATH.h>
#include <OS/OSError.h>

namespace ProjectPunch {
namespace Graphics {

Renderables renderables;

void Point::draw() {
    draw2DPoint(this->color, this->x, this->y, this->thickness);
}

void Line::draw() {
    draw2DLine(color, x1, y1, x2, y2, thickness);
}

void RectOutline::draw() {
    draw2DRectOutline(color, top, bottom, left, right, thickness);
}

void Rect::draw() {
    /*
    OSReport("addr: %08x\n", this);
    OSReport("rect draw: (col, t, b, l, r): %08x, %.3f, %.3f, %.3f, %.3f\n", color, top, bottom, left, right);
    */
    draw2DRectangle(color, top, bottom, left, right);
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
        drawItem(items.frame[i]);
        if (items.frame[i]->lifeTime <= 0) {
            // delete items.frame[i];
            items.frame.erase(i); 
            i -= 1;
        }
    }
    for (int i = 0; i < items.tick.size(); i++) { 
        drawItem(items.tick[i]);
    }
}

void Renderables::renderPre() {
    for (int i = 0; i < items.preFrame.size(); i++) {
        drawItem(items.preFrame[i]);
        if (items.preFrame[i]->lifeTime <= 0) {
            // delete items.preFrame[i];
            items.preFrame.erase(i); 
            i -= 1;
        }
    }
}

void Renderables::updateTick() {
    for (int i = 0; i < items.tick.size(); i++) {
        if (items.tick[i]->delay == 0) {
            if (items.tick[i]->lifeTime <= 0) { 
                items.tick.erase(i); 
                i -= 1;
            } else {
                items.tick[i]->lifeTime --;
            }
        } else {
            items.tick[i]->delay --;
        }
    }
}

void Renderables::clearAll() {
    items.frame.clear();
    items.preFrame.clear();
    items.tick.clear();
}

}} // namespace