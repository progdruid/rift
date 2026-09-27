// shared block-grid ordered dithering: the screen is split into PixelSize-sized blocks,
// each block samples at its center and gets one Bayer 8x8 threshold

static const float BAYER8[64] = {
     0, 32,  8, 40,  2, 34, 10, 42,
    48, 16, 56, 24, 50, 18, 58, 26,
    12, 44,  4, 36, 14, 46,  6, 38,
    60, 28, 52, 20, 62, 30, 54, 22,
     3, 35, 11, 43,  1, 33,  9, 41,
    51, 19, 59, 27, 49, 17, 57, 25,
    15, 47,  7, 39, 13, 45,  5, 37,
    63, 31, 55, 23, 61, 29, 53, 21
};

struct DitherBlock {
    float2 SnappedUV;   // center of the block, for sampling
    float Threshold;    // ordered-dither threshold in (0, 1)
};

DitherBlock GetDitherBlock(float2 uv, float pixelSize, float2 screenSize) {
    float2 blockUV = pixelSize / screenSize;
    int2 index = int2(floor(uv / blockUV));

    DitherBlock block;
    block.SnappedUV = (float2(index) + 0.5) * blockUV;
    block.Threshold = (BAYER8[(index.y & 7) * 8 + (index.x & 7)] + 0.5) / 64.0;
    return block;
}
