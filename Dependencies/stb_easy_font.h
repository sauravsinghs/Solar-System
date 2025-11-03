#ifndef STB_EASY_FONT_H
#define STB_EASY_FONT_H

// Minimal drop-in replacement for stb_easy_font that generates 2D triangle lists
// for a simple 5x7 uppercase bitmap font. Only the subset of the API needed by
// this project is implemented.

#include <cstdint>
#include <cctype>

namespace stb_easy_font
{
    constexpr int glyphHeight = 7;

    struct Glyph
    {
        uint8_t width;
        uint8_t rows[glyphHeight];
    };

    constexpr Glyph makeGlyph(uint8_t width,
        uint8_t r0, uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6)
    {
        return Glyph{ width,{ r0, r1, r2, r3, r4, r5, r6 } };
    }

    constexpr Glyph blankGlyph(uint8_t width = 3)
    {
        return Glyph{ width,{ 0,0,0,0,0,0,0 } };
    }

    constexpr Glyph glyphTable[128] = {
        /* 0 */ blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(),
        /* 8 */ blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(),
        /*16 */ blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(),
        /*24 */ blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(), blankGlyph(),
        /*32 ' '*/ blankGlyph(3),
        /*33 ! */ blankGlyph(),
        /*34 "*/ blankGlyph(),
        /*35 # */ blankGlyph(),
        /*36 $ */ blankGlyph(),
        /*37 % */ blankGlyph(),
        /*38 & */ blankGlyph(),
        /*39 ' */ blankGlyph(),
        /*40 ( */ blankGlyph(),
        /*41 ) */ blankGlyph(),
        /*42 * */ blankGlyph(),
        /*43 + */ blankGlyph(),
        /*44 , */ blankGlyph(),
        /*45 - */ makeGlyph(4, 0b0000, 0b0000, 0b0000, 0b1111, 0b0000, 0b0000, 0b0000),
        /*46 . */ makeGlyph(2, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b010),
        /*47 / */ makeGlyph(5, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000, 0b00000),
        /*48 0 */ makeGlyph(5, 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110),
        /*49 1 */ makeGlyph(5, 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110),
        /*50 2 */ makeGlyph(5, 0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111),
        /*51 3 */ makeGlyph(5, 0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110),
        /*52 4 */ makeGlyph(5, 0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010),
        /*53 5 */ makeGlyph(5, 0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110),
        /*54 6 */ makeGlyph(5, 0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110),
        /*55 7 */ makeGlyph(5, 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000),
        /*56 8 */ makeGlyph(5, 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110),
        /*57 9 */ makeGlyph(5, 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100),
        /*58 : */ blankGlyph(),
        /*59 ; */ blankGlyph(),
        /*60 < */ blankGlyph(),
        /*61 = */ blankGlyph(),
        /*62 > */ blankGlyph(),
        /*63 ? */ blankGlyph(),
        /*64 @ */ blankGlyph(),
        /*65 A */ makeGlyph(5, 0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001),
        /*66 B */ makeGlyph(5, 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110),
        /*67 C */ makeGlyph(5, 0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110),
        /*68 D */ makeGlyph(5, 0b11100, 0b10010, 0b10001, 0b10001, 0b10001, 0b10010, 0b11100),
        /*69 E */ makeGlyph(5, 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111),
        /*70 F */ makeGlyph(5, 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000),
        /*71 G */ makeGlyph(5, 0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110),
        /*72 H */ makeGlyph(5, 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001),
        /*73 I */ makeGlyph(3, 0b111, 0b010, 0b010, 0b010, 0b010, 0b010, 0b111),
        /*74 J */ makeGlyph(5, 0b00111, 0b00010, 0b00010, 0b00010, 0b10010, 0b10010, 0b01100),
        /*75 K */ makeGlyph(5, 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001),
        /*76 L */ makeGlyph(5, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111),
        /*77 M */ makeGlyph(5, 0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001),
        /*78 N */ makeGlyph(5, 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001),
        /*79 O */ makeGlyph(5, 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110),
        /*80 P */ makeGlyph(5, 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000),
        /*81 Q */ makeGlyph(5, 0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101),
        /*82 R */ makeGlyph(5, 0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001),
        /*83 S */ makeGlyph(5, 0b01110, 0b10001, 0b10000, 0b01110, 0b00001, 0b10001, 0b01110),
        /*84 T */ makeGlyph(5, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100),
        /*85 U */ makeGlyph(5, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110),
        /*86 V */ makeGlyph(5, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100),
        /*87 W */ makeGlyph(5, 0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010),
        /*88 X */ makeGlyph(5, 0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001),
        /*89 Y */ makeGlyph(5, 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100),
        /*90 Z */ makeGlyph(5, 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111),
        /*91 [ */ blankGlyph(),
        /*92 \*/ blankGlyph(),
        /*93 ] */ blankGlyph(),
        /*94 ^ */ blankGlyph(),
        /*95 _ */ blankGlyph(),
        /*96 ` */ blankGlyph(),
        /*97 a */ makeGlyph(5, 0b00000, 0b00000, 0b01110, 0b00001, 0b01111, 0b10001, 0b01111),
        /*98 b */ makeGlyph(5, 0b10000, 0b10000, 0b10110, 0b11001, 0b10001, 0b10001, 0b11110),
        /*99 c */ makeGlyph(5, 0b00000, 0b00000, 0b01110, 0b10000, 0b10000, 0b10001, 0b01110),
        /*100 d*/ makeGlyph(5, 0b00001, 0b00001, 0b01101, 0b10011, 0b10001, 0b10001, 0b01111),
        /*101 e*/ makeGlyph(5, 0b00000, 0b00000, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110),
        /*102 f*/ makeGlyph(4, 0b0011, 0b0100, 0b1110, 0b0100, 0b0100, 0b0100, 0b0100),
        /*103 g*/ makeGlyph(5, 0b00000, 0b00000, 0b01111, 0b10001, 0b01111, 0b00001, 0b01110),
        /*104 h*/ makeGlyph(5, 0b10000, 0b10000, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001),
        /*105 i*/ makeGlyph(3, 0b010, 0b000, 0b110, 0b010, 0b010, 0b010, 0b111),
        /*106 j*/ makeGlyph(4, 0b0010, 0b0000, 0b0011, 0b0001, 0b0001, 0b1001, 0b0110),
        /*107 k*/ makeGlyph(5, 0b10000, 0b10000, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010),
        /*108 l*/ makeGlyph(3, 0b110, 0b010, 0b010, 0b010, 0b010, 0b010, 0b111),
        /*109 m*/ makeGlyph(5, 0b00000, 0b00000, 0b11010, 0b10101, 0b10101, 0b10001, 0b10001),
        /*110 n*/ makeGlyph(5, 0b00000, 0b00000, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001),
        /*111 o*/ makeGlyph(5, 0b00000, 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110),
        /*112 p*/ makeGlyph(5, 0b00000, 0b00000, 0b11110, 0b10001, 0b11110, 0b10000, 0b10000),
        /*113 q*/ makeGlyph(5, 0b00000, 0b00000, 0b01111, 0b10001, 0b01111, 0b00001, 0b00001),
        /*114 r*/ makeGlyph(5, 0b00000, 0b00000, 0b10110, 0b11001, 0b10000, 0b10000, 0b10000),
        /*115 s*/ makeGlyph(5, 0b00000, 0b00000, 0b01111, 0b10000, 0b01110, 0b00001, 0b11110),
        /*116 t*/ makeGlyph(4, 0b0100, 0b0100, 0b1110, 0b0100, 0b0100, 0b0101, 0b0010),
        /*117 u*/ makeGlyph(5, 0b00000, 0b00000, 0b10001, 0b10001, 0b10001, 0b10011, 0b01101),
        /*118 v*/ makeGlyph(5, 0b00000, 0b00000, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100),
        /*119 w*/ makeGlyph(5, 0b00000, 0b00000, 0b10001, 0b10001, 0b10101, 0b10101, 0b01010),
        /*120 x*/ makeGlyph(5, 0b00000, 0b00000, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001),
        /*121 y*/ makeGlyph(5, 0b00000, 0b00000, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110),
        /*122 z*/ makeGlyph(5, 0b00000, 0b00000, 0b11111, 0b00010, 0b00100, 0b01000, 0b11111),
        /*123 { */ blankGlyph(),
        /*124 | */ blankGlyph(),
        /*125 } */ blankGlyph(),
        /*126 ~ */ blankGlyph(),
        /*127   */ blankGlyph()
    };

    inline const Glyph& getGlyph(char c)
    {
        unsigned char idx = static_cast<unsigned char>(c);
        return glyphTable[idx];
    }

    inline float& glyphSpacing()
    {
        static float spacing = 1.0f;
        return spacing;
    }

    inline void stb_easy_font_spacing(float spacing)
    {
        glyphSpacing() = spacing;
    }

    inline int stb_easy_font_width(const char* text)
    {
        if (!text) return 0;
        float width = 0.0f;
        float maxWidth = 0.0f;
        const float advanceY = static_cast<float>(glyphHeight) + glyphSpacing();
        (void)advanceY;
        for (const char* c = text; *c; ++c)
        {
            if (*c == '\n')
            {
                if (width > maxWidth) maxWidth = width;
                width = 0.0f;
                continue;
            }
            char ch = *c;
            if (std::islower(static_cast<unsigned char>(ch))) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            const Glyph& glyph = getGlyph(ch);
            width += glyph.width + glyphSpacing();
        }
        if (width > maxWidth) maxWidth = width;
        return static_cast<int>(maxWidth);
    }

    inline int stb_easy_font_height(const char* text)
    {
        if (!text || !*text) return 0;
        int lines = 1;
        for (const char* c = text; *c; ++c)
        {
            if (*c == '\n') ++lines;
        }
        return lines * (glyphHeight + static_cast<int>(glyphSpacing()));
    }

    inline int stb_easy_font_print(float x, float y, const char* text, const unsigned char* /*color*/, void* vertex_buffer, int vbuf_size)
    {
        if (!text || !vertex_buffer || vbuf_size <= 0) return 0;

        float* out = static_cast<float*>(vertex_buffer);
        const int maxVertices = vbuf_size / (sizeof(float) * 2);
        int vertexCount = 0;

        float cursorX = x;
        float cursorY = y;

        const float advanceY = static_cast<float>(glyphHeight) + glyphSpacing();

        for (const char* c = text; *c; ++c)
        {
            if (*c == '\n')
            {
                cursorX = x;
                cursorY += advanceY;
                continue;
            }

            char ch = *c;
            if (std::islower(static_cast<unsigned char>(ch))) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));

            const Glyph& glyph = getGlyph(ch);
            if (glyph.width == 0)
            {
                cursorX += glyphSpacing();
                continue;
            }

            for (int row = 0; row < glyphHeight; ++row)
            {
                uint8_t bits = glyph.rows[row];
                for (int col = 0; col < glyph.width; ++col)
                {
                    int bitIndex = glyph.width - 1 - col;
                    if ((bits >> bitIndex) & 0x1)
                    {
                        float x0 = cursorX + static_cast<float>(col);
                        float y0 = cursorY + static_cast<float>(row);
                        float x1 = x0 + 1.0f;
                        float y1 = y0 + 1.0f;

                        if (vertexCount + 6 > maxVertices)
                        {
                            return vertexCount;
                        }

                        // First triangle
                        out[vertexCount * 2 + 0] = x0; out[vertexCount * 2 + 1] = y0;
                        out[vertexCount * 2 + 2] = x1; out[vertexCount * 2 + 3] = y0;
                        out[vertexCount * 2 + 4] = x1; out[vertexCount * 2 + 5] = y1;
                        vertexCount += 3;

                        // Second triangle
                        out[vertexCount * 2 + 0] = x0; out[vertexCount * 2 + 1] = y0;
                        out[vertexCount * 2 + 2] = x1; out[vertexCount * 2 + 3] = y1;
                        out[vertexCount * 2 + 4] = x0; out[vertexCount * 2 + 5] = y1;
                        vertexCount += 3;
                    }
                }
            }

            cursorX += glyph.width + glyphSpacing();
        }

        return vertexCount;
    }
}

using stb_easy_font::stb_easy_font_print;
using stb_easy_font::stb_easy_font_width;
using stb_easy_font::stb_easy_font_height;
using stb_easy_font::stb_easy_font_spacing;

#endif // STB_EASY_FONT_H

