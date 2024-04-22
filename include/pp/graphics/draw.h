#pragma once
//
// Created by johno on 7/18/2020.
// Ported from fracture C++ by wisp on 03/2024
//

#include <gf/gf_camera.h>
#include <GX/GXTypes.h>
#include <gf/gf_draw.h>

namespace PP {
namespace Graphics {

struct Position3D {
    f32 x;
    f32 y;
    f32 z;
    
    Position3D() {
        Position3D(0, 0, 0);
    }
    Position3D(f32 x, f32 y, f32 z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    inline Position3D& operator-=(Position3D rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }
    inline Position3D& operator+=(Position3D rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z;
        return *this;
    }
    inline Position3D& operator*=(float rhs) {
        x *= rhs; y *= rhs; z *= rhs;
        return *this;
    }
};

void start2DDraw();
void startNormalDraw();
void setupDrawPrimitives();

void drawTriangleFan(GXColor color, Position3D* vertices, u32 numVertices);
void draw2DRectangle(GXColor color, float top, float bottom, float left, float right, float zPos = 0);
void draw2DLine(GXColor color, float x1, float y1, float x2, float y2, u8 thickness);
void draw2DPoint(GXColor color, float x, float y, u8 size);
void draw2DRectOutline(GXColor color, float top, float bottom, float left, float right, u8 thickness);
void drawPrimitive(GXColor color, Position3D* vertices, u32 numVertices);

GXColor makeGXColor(u8 r, u8 g, u8 b, u8 a);
u32 gxColorToU32(const GXColor& color);

}
}