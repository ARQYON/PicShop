#pragma once
#include <algorithm>
#include <cmath>
#include "Image.h"
using namespace std;
/* ColorOps - color mathematics for PicShop.
   All operations work directly on an Image buffer.
   These are the building blocks of every adjustment tool. */

namespace ColorOps
{
    /* Clamp helper.
       Keeps a value inside [0, 255]. Used after every arithmetic operation
       on a color channel. */
    inline unsigned char clamp(int value)
    {
        if (value < 0)
            return 0;
        if (value > 255)
            return 255;
        return static_cast<unsigned char>(value);
    }

    inline void brightness(Image &img, int amount)
    {
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                img.setR(x, y, clamp(img.getR(x, y) + amount));
                img.setG(x, y, clamp(img.getG(x, y) + amount));
                img.setB(x, y, clamp(img.getB(x, y) + amount));
                // Alpha is never touched by brightness
            }
        }
    }

    /* Contrast
       amount: -255 (flat grey) to +255 (maximum contrast).
       Values outside this range are clamped rather than rejected, since
       contrast is typically driven by a UI slider -clamping is the more
       forgiving behavior for a value a user dragged too far. Without this,
       amount near/above 259 drives the denominator below in the factor
       formula to zero or negative, causing division-by-zero or inverted
       contrast. */
    inline void contrast(Image &img, int amount)
    {
        if (amount < -255)
            amount = -255;
        if (amount > 255)
            amount = 255;

        // This factor is the industry standard approach
        double factor = (259.5 * (amount + 255)) / (255.0 * (259 - amount));
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                img.setR(x, y, clamp(static_cast<int>(factor * (img.getR(x, y) - 128) + 128)));
                img.setG(x, y, clamp(static_cast<int>(factor * (img.getG(x, y) - 128) + 128)));
                img.setB(x, y, clamp(static_cast<int>(factor * (img.getB(x, y) - 128) + 128)));
            }
        }
    }

    /* Grayscale
       Uses luminance weights - matches human eye sensitivity.
       This is the correct formula. Simple average is wrong. */
    inline void grayscale(Image &img)
    {
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                unsigned char r = img.getR(x, y);
                unsigned char g = img.getG(x, y);
                unsigned char b = img.getB(x, y);
                // Luminance formula - ITU-R BT.601 standard
                unsigned char gray = static_cast<unsigned char>(
                    0.299 * r + 0.587 * g + 0.114 * b);
                img.setR(x, y, gray);
                img.setG(x, y, gray);
                img.setB(x, y, gray);
                // Alpha unchanged
            }
        }
    }

    /* Invert
       Flips every channel: new = 255 - old.
       Creates a photographic negative effect. */
    inline void invert(Image &img)
    {
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                img.setR(x, y, 255 - img.getR(x, y));
                img.setG(x, y, 255 - img.getG(x, y));
                img.setB(x, y, 255 - img.getB(x, y));
                // Alpha unchanged
            }
        }
    }

    /* RGB <-> HSV Conversion.
       Needed for saturation and hue adjustment.
       Input RGB: 0-255. Output HSV: H=0-360, S=0-1, V=0-1 */
    struct HSV
    {
        double h, s, v;
    };

    inline HSV rgbToHsv(unsigned char r, unsigned char g, unsigned char b)
    {
        double rd = r / 255.0;
        double gd = g / 255.0;
        double bd = b / 255.0;
        double maxC = (std::max)((std::max)(rd, gd), bd);
        double minC = (std::min)((std::min)(rd, gd), bd);
        double delta = maxC - minC;
        HSV hsv;
        hsv.v = maxC;
        hsv.s = (maxC == 0.0) ? 0.0 : delta / maxC;
        if (delta == 0.0)
        {
            hsv.h = 0.0; // achromatic -no hue
        }
        else if (maxC == rd)
        {
            hsv.h = 60.0 * std::fmod((gd - bd) / delta, 6.0);
        }
        else if (maxC == gd)
        {
            hsv.h = 60.0 * ((bd - rd) / delta + 2.0);
        }
        else
        {
            hsv.h = 60.0 * ((rd - gd) / delta + 4.0);
        }
        if (hsv.h < 0.0)
            hsv.h += 360.0;
        return hsv;
    }

    // Convert HSV back to RGB. (0-255 each channel)
    inline void hsvToRgb(HSV hsv, unsigned char &r, unsigned char &g, unsigned char &b)
    {
        double h = hsv.h;
        double s = hsv.s;
        double v = hsv.v;
        if (s == 0.0)
        {
            // Achromatic gray.
            r = g = b = static_cast<unsigned char>(v * 255);
            return;
        }
        double c = v * s;
        double x = c * (1.0 - std::fabs(std::fmod(h / 60.0, 2.0) - 1.0));
        double m = v - c;
        double rd, gd, bd;

        // Branches MUST be checked in ascending order of threshold.
        // Previously "h < 300" was checked before "h < 180"/"h < 240"/"h < 120",
        // which swallowed hues 60-299 into a single (wrong) branch and made
        // the later branches unreachable dead code.
        if (h < 60)
        {
            rd = c;
            gd = x;
            bd = 0;
        }
        else if (h < 120)
        {
            rd = x;
            gd = c;
            bd = 0;
        }
        else if (h < 180)
        {
            rd = 0;
            gd = c;
            bd = x;
        }
        else if (h < 240)
        {
            rd = 0;
            gd = x;
            bd = c;
        }
        else if (h < 300)
        {
            rd = x;
            gd = 0;
            bd = c;
        }
        else
        {
            rd = c;
            gd = 0;
            bd = x;
        }
        r = static_cast<unsigned char>((rd + m) * 255);
        g = static_cast<unsigned char>((gd + m) * 255);
        b = static_cast<unsigned char>((bd + m) * 255);
    }

    /* Saturation
       amount -1.0 (fully grey) to +1.0 (max vivid).
       Works in HSV space - this is why we needed the conversion. */
    inline void saturation(Image &img, double amount)
    {
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                unsigned char r = img.getR(x, y);
                unsigned char g = img.getG(x, y);
                unsigned char b = img.getB(x, y);
                // Step 1: go into HSV space
                HSV hsv = rgbToHsv(r, g, b);
                // Step 2: adjust and clamp
                hsv.s = clamp(static_cast<int>((hsv.s + amount) * 255)) / 255.0;
                unsigned char nr, ng, nb;
                hsvToRgb(hsv, nr, ng, nb);

                img.setR(x, y, nr);
                img.setG(x, y, ng);
                img.setB(x, y, nb);
            }
        }
    }

    /* Hue Shift
       degrees: any value, wraps around 360.
       Rotates the color around the HSV cylinder. */
    inline void hueShift(Image &img, double degrees)
    {
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                unsigned char r = img.getR(x, y);
                unsigned char g = img.getG(x, y);
                unsigned char b = img.getB(x, y);
                HSV hsv = rgbToHsv(r, g, b);
                // Rotate the hue & wrap around [0, 360]
                hsv.h = std::fmod(hsv.h + degrees, 360.0);
                if (hsv.h < 0.0)
                    hsv.h += 360.0;
                unsigned char nr, ng, nb;
                hsvToRgb(hsv, nr, ng, nb);

                img.setR(x, y, nr);
                img.setG(x, y, ng);
                img.setB(x, y, nb);
            }
        }
    }
} // namespace ColorOps
