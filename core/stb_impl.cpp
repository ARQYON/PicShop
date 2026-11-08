// This file exists SOLELY to generate the stb_image / stb_image_write
// implementation code exactly once for the whole project.
//
// Do NOT define STB_IMAGE_IMPLEMENTATION or STB_IMAGE_WRITE_IMPLEMENTATION
// anywhere else. FileIO.h includes the stb headers WITHOUT these macros,
// so it only sees declarations and is safe to include from any number of
// .cpp files without causing duplicate-symbol linker errors.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"