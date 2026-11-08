#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>
#include "Layer.h"

/* LayerStack - manages all layers and composites them into a single final image.
   This is the central object of PicShop's document model. */
class LayerStack
{
private:
    int m_width;
    int m_height;
    std::vector<std::unique_ptr<Layer>> m_layers;

public:
    /* Construction. Dimensions are validated immediately so bad values fail here, not later inside addLayer() with a confusing stack trace. */
    LayerStack(int width, int height)
        : m_width(width), m_height(height)
    {
        if (width <= 0 || height <= 0)
            throw std::invalid_argument("LayerStack dimensions must be positive.");
    }

    // Dimensions
    int width() const { return m_width; }
    int height() const { return m_height; }

    /* Layer Management | Add a new blank layer on top */
    void addLayer(const std::string &name = "Layer")
    {
        m_layers.push_back(std::make_unique<Layer>(m_width, m_height, name));
    }

    // Total number of layers
    int layerCount() const
    {
        return static_cast<int>(m_layers.size());
    }

    // Access a layer by index (0 = bottom)
    Layer &getLayer(int index)
    {
        if (index < 0 || index >= layerCount())
            throw std::out_of_range("Layer index out of range.");
        return *m_layers[index];
    }

    // Remove a layer by index
    void removeLayer(int index)
    {
        if (index < 0 || index >= layerCount())
            throw std::out_of_range("Layer index out of range.");
        m_layers.erase(m_layers.begin() + index);
    }

    // Move a layer up in the stack (toward higher indices / top). Guard protects the top end: nothing to swap with above the last index.
    void moveLayerUp(int index)
    {
        if (index < 0 || index >= layerCount() - 1)
            return;
        std::swap(m_layers[index], m_layers[index + 1]);
    }

    /* Move a layer down in the stack (toward index 0 / bottom). Guard protects the BOTTOM end: index <= 0 means there's nothing below to swap with. (Previously this checked the wrong end and allowed moveLayerDown(0), which read m_layers[-1].) */
    void moveLayerDown(int index)
    {
        if (index <= 0 || index >= layerCount())
            return;
        std::swap(m_layers[index], m_layers[index - 1]);
    }

    /* Blend mode math.
       All values normalized to 0.0-1.0.
       S = source (layer above), D = destination (below) */
    double applyBlend(double S, double D, BlendMode mode) const
    {
        switch (mode)
        {
        case BlendMode::Normal:
            return S;
        case BlendMode::Multiply:
            return S * D;
        case BlendMode::Screen:
            return 1.0 - (1.0 - S) * (1.0 - D);
        case BlendMode::Overlay:
            // Standard overlay: low half is multiply-like, high half is screen-like. The high branch must use (1-S)(1-D), not S*D again.
            if (D < 0.5)
                return 2.0 * S * D;
            else
                return 1.0 - 2.0 * (1.0 - S) * (1.0 - D);
        case BlendMode::Difference:
            return std::fabs(S - D);
        default:
            return S;
        }
    }

    /* Composite | Flatten all visible layers into one final Image. Processes bottom to
       top - each layer blends onto the result of all layers below it.
       Alpha is now tracked through the loop and computed with the same
       "over" formula as the color channels, instead of being hardcoded. */
    Image composite() const
    {
        // Start with a fully transparent black canvas
        Image result(m_width, m_height);
        for (const auto &layerPtr : m_layers)
        {
            const Layer &layer = *layerPtr;
            // Skip invisible layers
            if (!layer.visible())
                continue;

            double opacity = layer.opacity();

            for (int y = 0; y < m_height; y++)
            {
                for (int x = 0; x < m_width; x++)
                {
                    // Source - this layer's pixel (normalized)
                    double sR = layer.image().getR(x, y) / 255.0;
                    double sG = layer.image().getG(x, y) / 255.0;
                    double sB = layer.image().getB(x, y) / 255.0;
                    double sA = layer.image().getA(x, y) / 255.0;

                    // Destination - current composite result (normalized)
                    double dR = result.getR(x, y) / 255.0;
                    double dG = result.getG(x, y) / 255.0;
                    double dB = result.getB(x, y) / 255.0;
                    double dA = result.getA(x, y) / 255.0;

                    // Step 1: apply blend mode
                    double bR = applyBlend(sR, dR, layer.blendMode());
                    double bG = applyBlend(sG, dG, layer.blendMode());
                    double bB = applyBlend(sB, dB, layer.blendMode());

                    // Step 2: apply opacity and layer alpha
                    // Effective alpha = layer opacity * pixel alpha
                    double effectiveAlpha = opacity * sA;

                    // Step 3: alpha composite onto destination (standard "over" operator)
                    double rR = bR * effectiveAlpha + dR * (1.0 - effectiveAlpha);
                    double rG = bG * effectiveAlpha + dG * (1.0 - effectiveAlpha);
                    double rB = bB * effectiveAlpha + dB * (1.0 - effectiveAlpha);
                    double rA = effectiveAlpha + dA * (1.0 - effectiveAlpha);

                    // Write back to result (convert to 0-255)
                    result.setR(x, y, static_cast<unsigned char>(rR * 255));
                    result.setG(x, y, static_cast<unsigned char>(rG * 255));
                    result.setB(x, y, static_cast<unsigned char>(rB * 255));
                    result.setA(x, y, static_cast<unsigned char>(rA * 255));
                }
            }
        }
        return result;
    }
};