/**
 * @file viewport.h
 * @brief Basic Viewport class.
 */

#pragma once

#include "types.h"
#include "sead/color.h"
#include "sead/bound.h"
#include "sead/projection.h"


namespace sead
{
    class TextWriter
    {
        public:
            virtual ~TextWriter();

            void printImpl_(char const*, int, bool, sead::BoundBox2<float>*);
            /* LinkerHints 0x00b1f868 _ZN4sead10TextWriter6printfEPKcz sead::TextWriter::printf */
            void printf(char const*, ...);
            /* LinkerHints 0x00b1f868 _ZN4sead10TextWriter6printfEPKDsz sead::TextWriter::printf */
            void printf(char16_t const*, ...);
            /* Disabled LinkerHints 0x00b1f9a8 _ZN4sead10TextWriter22setScaleFromFontHeightEf sead::TextWriter::setScaleFromFontHeight */
            void setScaleFromFontHeight(float);
            void beginDraw();
            /* LinkerHints 0x00b1f7b8 _ZN4sead10TextWriter13setupGraphicsEPNS_11DrawContextE sead::TextWriter::setupGraphics */
            static void setupGraphics(sead::DrawContext*);

            sead::Viewport *mViewport;
            sead::Projection *mProjection;
            sead::Camera *mCamera;
            int TextWriter_x20;
            int TextWriter_x24;
            int TextWriter_x28;
            int TextWriter_x2C;
            sead::Vector2<float> mScale;
            sead::Color4f mColor;
            int TextWriter_x48;
            float mLineSpace;
            sead::BoundBox2<float> *mBoundBox2;
            int TextWriter_x58;
            int TextWriter_x5C;
            char16_t *mFormatBuffer;
            int mFormatBufferSize;
            int TextWriter_x6C;
            sead::DrawContext *mDrawContext;
    };
};