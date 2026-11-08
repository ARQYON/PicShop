#pragma once
#include <string>
#include "Image.h"

// Blend modess supported by PicShop
enum class BlendMode
{
    Normal,
    Multiply,
    Screen,
    Overlay,
    Difference
};
/* Layer - a single iamge with opacity and blend mode. The LayerStack these from bottom to top. */
class Layer
{
private:
    Image m_image;
    double m_opacity;
    BlendMode m_blendMode;
    bool m_visible;
    std::string m_name;

public:
    // Construction
    Layer(int width, int height, const std::string &name = "Layer")
        : m_image(width, height), m_opacity(1.0), m_blendMode(BlendMode::Normal), m_visible(true), m_name(name)
    {
    }
    // Accessors
    Image &image() { return m_image; }
    const Image &image() const { return m_image; }
    double opacity() const { return m_opacity; }
    BlendMode blendMode() const { return m_blendMode; }
    bool visible() const { return m_visible; }
    const std::string &name() const { return m_name; }
    // Setters
    void setOpacity(double o)
    {
        // Clamp opacity to valid range.
        if (o < 0.0)
            o = 0.0;
        if (o > 1.0)
            o = 1.0;
        m_opacity = o;
    }
    void setBlendMode(BlendMode mode) { m_blendMode = mode; }
    void setVisible(bool v) { m_visible = v; }
    void setName(const std::string &n) { m_name = n; }
};