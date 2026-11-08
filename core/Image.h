#pragma once
#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>

/* Image - core pixel buffer. Stores a flat RGBA image in memory.
   This is the foundation of the PicShop engine. */
class Image
{
private:
    int m_width;
    int m_height;
    std::vector<unsigned char> m_data; // Flat RGBA buffer

    // Validates a single dimension value. Runs BEFORE m_data is sized,
    // because m_width/m_height are declared (and therefore initialized)
    // before m_data in this class.
    static int validateDim(int value, const char *name)
    {
        if (value <= 0)
            throw std::invalid_argument(std::string("Image dimension '") + name + "' must be positive.");
        return value;
    }

    // Bounds-checked index. Throws instead of silently reading/writing
    // out of the buffer. Use this from all public pixel accessors.
    int checkedIndex(int x, int y) const
    {
        if (!isBounds(x, y))
            throw std::out_of_range("Pixel coordinates out of bounds.");
        return pixelIndex(x, y);
    }

public:
    /* Construction
       Create blank image filled with transparent black (0,0,0,0).
       Dimensions are validated via validateDim() before m_data's size
       is ever computed, so invalid input never reaches the allocation. */
    Image(int width, int height)
        : m_width(validateDim(width, "width")),
          m_height(validateDim(height, "height")),
          m_data(static_cast<size_t>(m_width) * static_cast<size_t>(m_height) * 4, 0)
    {
    }

    Image(const Image &other)
        : m_width(other.m_width), m_height(other.m_height), m_data(other.m_data)
    {
    }

    // Dimensions
    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return 4; } // Always RGBA

    /* Total bytes this image occupies in memory. size_t avoids overflow
       for large width*height combinations. */
    size_t bytesSize() const
    {
        return static_cast<size_t>(m_width) * static_cast<size_t>(m_height) * 4;
    }

    /* Raw data access. Direct pointer to the raw pixel buffer (needed later
       for Qt6 and file I/O). No bounds checking here by design -callers
       using the raw pointer are responsible for staying in range. */
    unsigned char *data() { return m_data.data(); }
    const unsigned char *data() const { return m_data.data(); }

    /* Pixel access. THE key formula: given (x, y), find the byte index.
       This stays UNCHECKED and public intentionally -it's the fast path
       for callers (like fill(), or filters) who have already verified
       (x, y) is in range and don't want a bounds check on every pixel
       in a tight loop. */
    int pixelIndex(int x, int y) const
    {
        return (y * m_width + x) * 4;
    }

    // Bounds check - return true if (x, y) is inside the image
    bool isBounds(int x, int y) const
    {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    // Get individual channel at pixel (x, y). Bounds-checked.
    unsigned char getR(int x, int y) const { return m_data[checkedIndex(x, y) + 0]; }
    unsigned char getG(int x, int y) const { return m_data[checkedIndex(x, y) + 1]; }
    unsigned char getB(int x, int y) const { return m_data[checkedIndex(x, y) + 2]; }
    unsigned char getA(int x, int y) const { return m_data[checkedIndex(x, y) + 3]; }

    // Set individual channel at pixel (x, y). Bounds-checked.
    void setR(int x, int y, unsigned char r) { m_data[checkedIndex(x, y) + 0] = r; }
    void setG(int x, int y, unsigned char g) { m_data[checkedIndex(x, y) + 1] = g; }
    void setB(int x, int y, unsigned char b) { m_data[checkedIndex(x, y) + 2] = b; }
    void setA(int x, int y, unsigned char a) { m_data[checkedIndex(x, y) + 3] = a; }

    // Set all 4 channels of a pixel at once (the most common operation). Bounds-checked.
    void setPixel(int x, int y,
                  unsigned char r,
                  unsigned char g,
                  unsigned char b,
                  unsigned char a = 255)
    {
        int i = checkedIndex(x, y);
        m_data[i + 0] = r;
        m_data[i + 1] = g;
        m_data[i + 2] = b;
        m_data[i + 3] = a;
    }

    /* Utility
       Fill the entire image with one color. Uses the unchecked pixelIndex
       directly since the loop bounds are already guaranteed valid. */
    void fill(unsigned char r,
              unsigned char g,
              unsigned char b,
              unsigned char a = 255)
    {
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                int i = pixelIndex(x, y);
                m_data[i + 0] = r;
                m_data[i + 1] = g;
                m_data[i + 2] = b;
                m_data[i + 3] = a;
            }
        }
    }

    // Clear to fully transparent black
    void clear()
    {
        std::fill(m_data.begin(), m_data.end(), 0);
    }
};