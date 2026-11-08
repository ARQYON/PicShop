#pragma once
#include <string>
#include <stdexcept>
#include <algorithm>
/* stb_image setup.
   IMPORTANT: this header intentionally does NOT define
   STB_IMAGE_IMPLEMENTATION / STB_IMAGE_WRITE_IMPLEMENTATION. Those macros
   are defined exactly once, in stb_impl.cpp. Including the stb headers
   here without the macros only pulls in declarations, which is safe to
   do from any number of translation units. If you add the implementation
   macros back here, you WILL get duplicate-symbol linker errors as soon
   as more than one .cpp file includes this header. */
#include "stb_image.h"
#include "stb_image_write.h"
#include "Image.h"

/* FileIO - load and save images for PicShop.
   Supports PNG, JPEG, BMP on load. Supports PNG and JPEG on save. */

namespace FileIO
{
    /* Load | Loads any supported image file into a PicShop Image.
       Always forces RGBA (4 channels) regardless of source.
       Marked inline for the same reason as the functions below: safe
       multi-.cpp inclusion. */
    inline Image load(const std::string &filepath)
    {
        int width, height, channels;
        unsigned char *data = stbi_load(
            filepath.c_str(),
            &width, &height, &channels, 4);
        if (!data)
        {
            throw std::runtime_error(
                "FileIO: Failed to load: " + filepath + "\nReason: " + stbi_failure_reason());
        }
        // Create PicShop Image with loaded dimensions.
        Image img(width, height);
        /* Copy pixel data from stb buffer into our Image.
           Both are flat RGBA arrays - direct copy works perfectly. */
        std::copy(data, data + width * height * 4, img.data());
        // Free stb's buffer - we have our own copy now
        stbi_image_free(data);
        return img;
    }

    /* Save PNG
       PNG is lossless - pixel perfect. Use for working files. */
    inline void savePNG(const Image &img, const std::string &filepath)
    {
        int result = stbi_write_png(
            filepath.c_str(),
            img.width(),
            img.height(),
            4,
            img.data(),
            img.width() * 4);
        if (!result)
            throw std::runtime_error(
                "FileIO: Failed to save PNG: " + filepath);
    }

    /* Save JPEG
       JPEG is lossy - smaller file, loses some quality.
       quality 1 (worst) to 100 (best). 85-95 is standard.
       Note: JPEG has no alpha channel, so the alpha byte in each RGBA
       pixel is silently discarded by stb during encoding. If you need
       alpha preserved, use savePNG instead. */
    inline void saveJPEG(const Image &img,
                         const std::string &filepath,
                         int quality = 90)
    {
        if (quality < 1)
            quality = 1;
        if (quality > 100)
            quality = 100;
        int result = stbi_write_jpg(
            filepath.c_str(),
            img.width(),
            img.height(), 4,
            img.data(),
            quality);
        if (!result)
            throw std::runtime_error(
                "FileIO: Failed to save JPEG: " + filepath);
    }

    /* Get image info without loading.
       Use for checking dimensions before a full load. */
    struct ImageInfo
    {
        int width;
        int height;
        int channels;
    };

    inline ImageInfo getInfo(const std::string &filepath)
    {
        ImageInfo info;
        int ok = stbi_info(
            filepath.c_str(),
            &info.width,
            &info.height,
            &info.channels);
        if (!ok)
        {
            throw std::runtime_error(
                "FileIO: Cannot read file info: " + filepath);
        }
        return info;
    }
} // namespace FileIO