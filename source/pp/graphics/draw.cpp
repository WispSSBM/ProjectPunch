//
// Created by johno on 7/18/2020.
//

#include "pp/graphics/draw.h"

#include <GX/GXGeometry.h>
#include <GX/GXPixel.h>
#include <GX/GXTev.h>
#include <GX/GXHardware.h>
#include <gf/gf_camera.h>

#include <OS/OSError.h>

namespace ProjectPunch {
namespace Graphics {


void start2DDraw() {
    gfDrawSetupCoord2D();
}


u32 gxColorToU32(const GXColor& color) {
    return (u32)(
        ((color.r & 0xFF) << 24) 
        | ((color.g & 0xFF) << 16) 
        | ((color.b & 0xFF) << 8) 
        | (color.a & 0xFF)
    );
}

GXColor makeGXColor(u8 r, u8 g, u8 b, u8 a) {
    GXColor color = GXColor();
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}


void setupDrawPrimitives() {
    //assumes you never want your primitives to be culled
    GXSetCullMode(GX_CULL_NONE);
    gfDrawSetVtxPosColorPrimEnvironment();
    GXSetZMode(true, GX_LEQUAL, false);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
}

void startNormalDraw() {
    gfCameraManager* manager = gfCameraManager::getManager();
    manager->m_cameras->setGX();
}

void drawTriangleFan(GXColor color, Position3D* vertices, u32 numVertices) {
    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT1, numVertices);

    drawPrimitive(color, vertices, numVertices);
}

void draw2DRectangle(GXColor color, float top, float bottom, float left, float right, float zPos) {
    GXBegin(GX_QUADS, GX_VTXFMT1, 4);

    WGPIPE.f = left;
    WGPIPE.f = top;
    WGPIPE.f = zPos;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = right;
    WGPIPE.f = top;
    WGPIPE.f = zPos;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = right;
    WGPIPE.f = bottom;
    WGPIPE.f = zPos;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = left;
    WGPIPE.f = bottom;
    WGPIPE.f = zPos;
    WGPIPE.ui = gxColorToU32(color);
}

void draw2DLine(GXColor color, float x1, float y1, float x2, float y2, u8 thickness) {
    GXSetPointSize(0, 0);
    GXSetLineWidth(thickness, 0);
    GXBegin(GX_LINES, GX_VTXFMT1, 2);

    WGPIPE.f = x1;
    WGPIPE.f = y1;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = x2;
    WGPIPE.f = y2;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);
}

void draw2DPoint(GXColor color, float x1, float y1, u8 thickness) {
    GXSetPointSize(thickness, 0);
    GXBegin(GX_POINTS, GX_VTXFMT1, 1);

    WGPIPE.f = x1;
    WGPIPE.f = y1;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);
}

void draw2DRectOutline(GXColor color, float top, float bottom, float left, float right, u8 thickness) {
    GXSetPointSize(thickness, 0);
    GXSetLineWidth(thickness, 0);
    GXBegin(GX_LINESTRIP, GX_VTXFMT1, 5);

    WGPIPE.f = left;
    WGPIPE.f = top;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = right;
    WGPIPE.f = top;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = right;
    WGPIPE.f = bottom;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = left;
    WGPIPE.f = bottom;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);

    WGPIPE.f = left;
    WGPIPE.f = top;
    WGPIPE.f = 0;
    WGPIPE.ui = gxColorToU32(color);
}

/*void drawQuads(GXColor color, Position3D* vertices, u32 numVertices) {
    _GXBegin(GXPrimitive::GX_QUADS, 1, numVertices);

    drawPrimitive(color, vertices, numVertices);
}*/


void drawPrimitive(GXColor color, Position3D* vertices, u32 numVertices) {
    for(int i = 0; i < numVertices; i++) {
        WGPIPE.f = vertices[i].x;
        WGPIPE.f = vertices[i].y;
        WGPIPE.f = vertices[i].z;
        WGPIPE.ui = gxColorToU32(color);
    }
}

}} // namespace