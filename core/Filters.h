#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "Image.h"

/*
Filters - convolution-based image filters for PicShop.
All filters use a separate output buffer to avoid reading modified pixels during processing.
*/
namespace Filters
{
    // Clamp coordinate to image bounds
    inline int clampCoord(int value, int max)
    {
        if (max <= 0)
            return 0;
        if (value < 0)
            return 0;
        if (value >= max)
            return max - 1;
        return value;
    }

    // Clamp pixel value to 0-255
    inline unsigned char clampValue(int value)
    {
        if (value < 0)
            return 0;
        if (value > 255)
            return 255;
        return static_cast<unsigned char>(value);
    }

    /* Core convolution engine.
       This is a single function that powers ALL kernel-based filters.
       KERNEL     | Flat array of kernelSize*kernelSize values
       KERNELSIZE | Must be odd and positive: 3, 5, 7, etc. (we use 3)
       DIVISOR    | Sum to divide result by (for normalization). Must not be zero.
       OFFSET     | Added after division (for emboss etc.)

       Marked inline: this header may end up included from more than one
       .cpp file once the project grows (e.g. Qt6 integration), and a
       non-inline function defined in a header causes duplicate-symbol
       linker errors in that situation. */
    inline void applyKernel(Image &img, const std::vector<double> &kernel,
                            int kernelSize = 3,
                            double divisor = 1.0,
                            int offset = 0)
    {
        if (kernelSize <= 0 || kernelSize % 2 == 0)
            throw std::invalid_argument("Filters::applyKernel: kernelSize must be odd and positive.");
        if (kernel.size() != static_cast<size_t>(kernelSize) * static_cast<size_t>(kernelSize))
            throw std::invalid_argument("Filters::applyKernel: kernel vector size does not match kernelSize.");
        if (divisor == 0.0)
            throw std::invalid_argument("Filters::applyKernel: divisor must not be zero.");

        int w = img.width();
        int h = img.height();
        int half = kernelSize / 2;

        /* Output buffer - same size as original. We never write into img until the end. */
        std::vector<unsigned char> output(w * h * 4, 0);

        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                double sumR = 0, sumG = 0, sumB = 0;
                // Slide the kernel over the neighborhood
                for (int ky = 0; ky < kernelSize; ky++)
                {
                    for (int kx = 0; kx < kernelSize; kx++)
                    {
                        int nx = clampCoord(x + kx - half, w);
                        int ny = clampCoord(y + ky - half, h);
                        // Kernel weight at this position
                        double weight = kernel[ky * kernelSize + kx];
                        // Accumulate weighted color values.
                        sumR += img.getR(nx, ny) * weight;
                        sumG += img.getG(nx, ny) * weight;
                        sumB += img.getB(nx, ny) * weight;
                    }
                }
                // Normalize by divisor, add offset, clamp to 0-255
                int i = img.pixelIndex(x, y);
                output[i + 0] = clampValue(static_cast<int>(sumR / divisor) + offset);
                output[i + 1] = clampValue(static_cast<int>(sumG / divisor) + offset);
                output[i + 2] = clampValue(static_cast<int>(sumB / divisor) + offset);
                output[i + 3] = img.getA(x, y);
            }
        }
        std::copy(output.begin(), output.end(), img.data());
    }

    /* Box Blur
       Average each pixel with its 8 neighbors.
       Divisor = 9 normalizes the sum back to 0-255 range. */
    inline void boxBlur(Image &img)
    {
        std::vector<double> kernel = {
            1, 1, 1,
            1, 1, 1,
            1, 1, 1
        };
        applyKernel(img, kernel, 3, 9.0);
    }

    /* Gaussian Blur
       Smoother than box blur. Center pixel weighted more, neighbors weighted
       by distance. Looks natural. */
    inline void gaussianBlur(Image &img)
    {
        std::vector<double> kernel = {
            1, 2, 1,
            2, 4, 2,
            1, 2, 1};
        applyKernel(img, kernel, 3, 16.0);
    }

    /* Sharpen
       Amplifies the center pixel, subtracts neighbors. Makes edges and
       details more pronounced. */
    inline void sharpen(Image &img)
    {
        std::vector<double> kernel = {
            0, -1, 0,
            -1, 5, -1,
            0, -1, 0};
        applyKernel(img, kernel, 3, 1.0);
    }

    /* Edge Detection
       Laplacian kernel. Dark where flat, bright where edges are. Offset 128
       centers the result so mid gray = no edge. */
    inline void edgeDetect(Image &img)
    {
        std::vector<double> kernel = {
            0, -1, 0,
            -1, 4, -1,
            0, -1, 0};
        applyKernel(img, kernel, 3, 1.0, 128);
    }

    /* Emboss
       Creates a 3D raised/stamped effect. Offset 128 gives the grey midpoint
       baseline. */
    inline void emboss(Image &img)
    {
        std::vector<double> kernel = {
            -2, -1, 0,
            -1, 1, 1,
            0, 1, 2};
        applyKernel(img, kernel, 3, 16.0, 128);
    }

    /* Unsharp Mask
       Professional sharpening technique used in real editors. Subtracts a
       blurred copy from the original to find edges, then adds these edges
       back amplified.
       strength: 0.0 = no effect, 1.0 = full, 2.0+ = heavy.
       (Renamed from unsharoMask - was a typo in the original name.) */
    inline void unsharpMask(Image &img, double strength = 1.0)
    {
        // Step 1: make a blurred copy.
        Image blurred = img;
        gaussianBlur(blurred);
        // Step 2: for each pixel, add the difference back.
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                int r = img.getR(x, y) + static_cast<int>(strength * (img.getR(x, y) - blurred.getR(x, y)));
                int g = img.getG(x, y) + static_cast<int>(strength * (img.getG(x, y) - blurred.getG(x, y)));
                int b = img.getB(x, y) + static_cast<int>(strength * (img.getB(x, y) - blurred.getB(x, y)));
                img.setR(x, y, clampValue(r));
                img.setG(x, y, clampValue(g));
                img.setB(x, y, clampValue(b));
            }
        }
    }
} // namespace Filters
