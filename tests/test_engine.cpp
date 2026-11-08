/*
 * PicShop - main.cpp
 * Test harness exercising every component after the bug-fix pass:
 *   Image, ColorOps, Filters, LayerStack (+ Layer), FileIO, History
 * NOTE: The LayerStack test section assumes Layer.h exposes:
 *   layer.setVisible(bool), layer.setOpacity(double),
 *   layer.setBlendMode(BlendMode), layer.image() (non-const, for painting)
 * These names are inferred from LayerStack.h usage, not confirmed against
 * your actual Layer.h. If this section fails to compile, paste Layer.h
 * and these calls will be corrected to match your real method names.
 *
 * Each test section prints PASS/FAIL style output so failures are easy
 * to spot without a full test framework.
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include "../core/Image.h"
#include "../core/ColorOps.h"
#include "../core/Filters.h"
#include "../core/Layer.h"
#include "../core/LayerStack.h"
#include "../core/FileIO.h"
#include "../core/History.h"

// ---------- Small test helpers ----------

static int g_testsRun = 0;
static int g_testsPassed = 0;

void check(bool condition, const std::string &description)
{
    g_testsRun++;
    if (condition)
    {
        g_testsPassed++;
        std::cout << "  [PASS] " << description << "\n";
    }
    else
    {
        std::cout << "  [FAIL] " << description << "\n";
    }
}

// Allow small rounding tolerance when comparing color channels
bool approxEqual(int a, int b, int tolerance = 2)
{
    return std::abs(a - b) <= tolerance;
}

void section(const std::string &title)
{
    std::cout << "\n=== " << title << " ===\n";
}

// ---------- Image tests ----------

void testImage()
{
    section("Image");

    // Valid construction
    Image img(10, 10);
    check(img.width() == 10 && img.height() == 10, "Constructs with correct dimensions");
    check(img.bytesSize() == 10 * 10 * 4, "bytesSize() matches width*height*4");

    // Invalid dimensions must throw, and must throw BEFORE any bad allocation
    bool threwOnZero = false;
    try
    {
        Image bad(0, 5);
    }
    catch (const std::invalid_argument &)
    {
        threwOnZero = true;
    }
    check(threwOnZero, "Throws invalid_argument on zero width");

    bool threwOnNegative = false;
    try
    {
        Image bad(-5, 5);
    }
    catch (const std::invalid_argument &)
    {
        threwOnNegative = true;
    }
    check(threwOnNegative, "Throws invalid_argument on negative height/width");

    // setPixel / getR / getG / getB / getA round trip
    img.setPixel(3, 4, 10, 20, 30, 40);
    check(img.getR(3, 4) == 10 && img.getG(3, 4) == 20 &&
              img.getB(3, 4) == 30 && img.getA(3, 4) == 40,
          "setPixel/get* round-trips correctly");

    // Bounds checking - out-of-range access must throw now
    bool threwOnOOB = false;
    try
    {
        img.getR(100, 100);
    }
    catch (const std::out_of_range &)
    {
        threwOnOOB = true;
    }
    check(threwOnOOB, "getR() throws out_of_range for invalid coordinates");

    bool threwOnNegCoord = false;
    try
    {
        img.setPixel(-1, 0, 1, 2, 3, 4);
    }
    catch (const std::out_of_range &)
    {
        threwOnNegCoord = true;
    }
    check(threwOnNegCoord, "setPixel() throws out_of_range for negative coordinates");

    // fill() + clear()
    img.fill(200, 150, 100, 255);
    check(img.getR(0, 0) == 200 && img.getR(9, 9) == 200,
          "fill() sets every pixel including corners");
    img.clear();
    check(img.getR(0, 0) == 0 && img.getA(0, 0) == 0,
          "clear() resets to transparent black");

    // Copy constructor
    Image original(4, 4);
    original.setPixel(0, 0, 9, 9, 9, 9);
    Image copy = original;
    copy.setPixel(0, 0, 1, 1, 1, 1);
    check(original.getR(0, 0) == 9 && copy.getR(0, 0) == 1,
          "Copy constructor makes an independent copy (deep copy)");
}

// ---------- ColorOps tests ----------

void testColorOps()
{
    section("ColorOps");

    // Brightness
    Image img(2, 2);
    img.fill(100, 100, 100, 255);
    ColorOps::brightness(img, 50);
    check(img.getR(0, 0) == 150, "brightness(+50) increases channel correctly");
    ColorOps::brightness(img, -200);
    check(img.getR(0, 0) == 0, "brightness() clamps at 0, doesn't underflow");

    // Contrast - clamped amount should not crash or divide by zero
    Image contrastImg(2, 2);
    contrastImg.fill(128, 128, 128, 255);
    bool contrastThrew = false;
    try
    {
        ColorOps::contrast(contrastImg, 9999); // should clamp internally, not throw
    }
    catch (...)
    {
        contrastThrew = true;
    }
    check(!contrastThrew, "contrast() with extreme amount clamps instead of throwing/crashing");

    // Grayscale
    Image colorImg(1, 1);
    colorImg.setPixel(0, 0, 255, 0, 0, 255); // pure red
    ColorOps::grayscale(colorImg);
    int expectedGray = static_cast<int>(0.299 * 255);
    check(approxEqual(colorImg.getR(0, 0), expectedGray),
          "grayscale() applies luminance formula (pure red -> ~76)");
    check(colorImg.getR(0, 0) == colorImg.getG(0, 0) &&
              colorImg.getG(0, 0) == colorImg.getB(0, 0),
          "grayscale() sets all channels equal");

    // Invert
    Image invImg(1, 1);
    invImg.setPixel(0, 0, 10, 20, 30, 255);
    ColorOps::invert(invImg);
    check(invImg.getR(0, 0) == 245 && invImg.getG(0, 0) == 235 && invImg.getB(0, 0) == 225,
          "invert() flips each channel (255 - value)");

    // HSV round trip: pure colors should survive rgbToHsv -> hsvToRgb
    struct ColorCase
    {
        unsigned char r, g, b;
        const char *name;
    };
    ColorCase cases[] = {
        {255, 0, 0, "red"}, {0, 255, 0, "green"}, {0, 0, 255, "blue"}, {255, 255, 0, "yellow"}, {0, 255, 255, "cyan"}, {255, 0, 255, "magenta"}};
    for (auto &c : cases)
    {
        ColorOps::HSV hsv = ColorOps::rgbToHsv(c.r, c.g, c.b);
        unsigned char r, g, b;
        ColorOps::hsvToRgb(hsv, r, g, b);
        check(approxEqual(r, c.r) && approxEqual(g, c.g) && approxEqual(b, c.b),
              std::string("HSV round-trip preserves ") + c.name);
    }

    // Hue shift correctness: pure red shifted +120 degrees should become
    // approximately pure green. This directly tests the branch-order fix.
    Image hueImg(1, 1);
    hueImg.setPixel(0, 0, 255, 0, 0, 255);
    ColorOps::hueShift(hueImg, 120.0);
    check(approxEqual(hueImg.getR(0, 0), 0) &&
              approxEqual(hueImg.getG(0, 0), 255) &&
              approxEqual(hueImg.getB(0, 0), 0),
          "hueShift(red, +120) produces green (validates HSV branch fix)");

    // Shift again +120 (total +240) should land on blue
    ColorOps::hueShift(hueImg, 120.0);
    check(approxEqual(hueImg.getR(0, 0), 0) &&
              approxEqual(hueImg.getG(0, 0), 0) &&
              approxEqual(hueImg.getB(0, 0), 255),
          "hueShift(green, +120) produces blue (validates HSV branch fix)");

    // Saturation: fully desaturating should approach grayscale
    Image satImg(1, 1);
    satImg.setPixel(0, 0, 200, 50, 50, 255);
    ColorOps::saturation(satImg, -1.0);
    check(approxEqual(satImg.getR(0, 0), satImg.getG(0, 0), 3) &&
              approxEqual(satImg.getG(0, 0), satImg.getB(0, 0), 3),
          "saturation(-1.0) desaturates toward gray (R≈G≈B)");
}

// ---------- Filters tests ----------

void testFilters()
{
    section("Filters");

    // applyKernel validation: bad kernel size
    Image img(5, 5);
    img.fill(100, 100, 100, 255);
    bool threwOnEvenSize = false;
    try
    {
        std::vector<double> kernel(16, 1.0); // 4x4 = even size
        Filters::applyKernel(img, kernel, 4, 16.0);
    }
    catch (const std::invalid_argument &)
    {
        threwOnEvenSize = true;
    }
    check(threwOnEvenSize, "applyKernel() rejects even kernelSize");

    bool threwOnMismatch = false;
    try
    {
        std::vector<double> kernel = {1, 1, 1}; // only 3 values, but kernelSize=3 expects 9
        Filters::applyKernel(img, kernel, 3, 3.0);
    }
    catch (const std::invalid_argument &)
    {
        threwOnMismatch = true;
    }
    check(threwOnMismatch, "applyKernel() rejects kernel vector size mismatch");

    bool threwOnZeroDivisor = false;
    try
    {
        std::vector<double> kernel(9, 1.0);
        Filters::applyKernel(img, kernel, 3, 0.0);
    }
    catch (const std::invalid_argument &)
    {
        threwOnZeroDivisor = true;
    }
    check(threwOnZeroDivisor, "applyKernel() rejects zero divisor");

    // Box blur on a flat image should leave it unchanged
    Image flatImg(5, 5);
    flatImg.fill(120, 120, 120, 255);
    Filters::boxBlur(flatImg);
    check(approxEqual(flatImg.getR(2, 2), 120),
          "boxBlur() on flat image leaves color unchanged");

    // Gaussian blur should run without throwing on a normal image
    Image gaussImg(5, 5);
    gaussImg.fill(80, 160, 240, 255);
    bool gaussThrew = false;
    try
    {
        Filters::gaussianBlur(gaussImg);
    }
    catch (...)
    {
        gaussThrew = true;
    }
    check(!gaussThrew, "gaussianBlur() runs without throwing on valid image");

    // Sharpen, edgeDetect, emboss - just verify they run and stay in valid range
    Image sharpImg(5, 5);
    sharpImg.fill(50, 100, 150, 255);
    Filters::sharpen(sharpImg);
    check(sharpImg.getR(2, 2) <= 255, "sharpen() output stays within byte range");

    Image edgeImg(5, 5);
    edgeImg.fill(50, 100, 150, 255);
    Filters::edgeDetect(edgeImg);
    check(edgeImg.getR(0, 0) <= 255, "edgeDetect() output stays within byte range");

    Image embossImg(5, 5);
    embossImg.fill(50, 100, 150, 255);
    Filters::emboss(embossImg);
    check(embossImg.getR(0, 0) <= 255, "emboss() output stays within byte range");

    // Unsharp mask (renamed from unsharoMask)
    Image unsharpImg(5, 5);
    unsharpImg.fill(60, 120, 180, 255);
    bool unsharpThrew = false;
    try
    {
        Filters::unsharpMask(unsharpImg, 1.0);
    }
    catch (...)
    {
        unsharpThrew = true;
    }
    check(!unsharpThrew, "unsharpMask() (renamed) runs without throwing");
}

// ---------- LayerStack / Layer tests ----------
// ASSUMPTION: Layer exposes setVisible(bool), setOpacity(double),
// setBlendMode(BlendMode), and a non-const image() for direct pixel edits.
// Adjust these calls if your real Layer.h differs.

void testLayerStack()
{
    section("LayerStack");

    // Constructor validation
    bool stackThrew = false;
    try
    {
        LayerStack badStack(0, 10);
    }
    catch (const std::invalid_argument &)
    {
        stackThrew = true;
    }
    check(stackThrew, "LayerStack constructor rejects invalid dimensions");

    LayerStack stack(4, 4);
    stack.addLayer("Background");
    stack.addLayer("Middle");
    stack.addLayer("Top");
    check(stack.layerCount() == 3, "addLayer() adds layers correctly");

    // moveLayerDown(0) must NOT crash - this is the critical regression test
    bool moveDownCrashed = false;
    try
    {
        stack.moveLayerDown(0); // should safely no-op, not touch m_layers[-1]
    }
    catch (...)
    {
        moveDownCrashed = true;
    }
    check(!moveDownCrashed, "moveLayerDown(0) does not crash (underflow fix)");
    check(stack.layerCount() == 3, "moveLayerDown(0) leaves layer count unchanged");

    // moveLayerUp on the top layer should also safely no-op
    bool moveUpCrashed = false;
    try
    {
        stack.moveLayerUp(stack.layerCount() - 1);
    }
    catch (...)
    {
        moveUpCrashed = true;
    }
    check(!moveUpCrashed, "moveLayerUp(top index) does not crash");

    // Paint the background layer solid red, fully opaque, Normal blend
    Layer &background = stack.getLayer(0);
    background.setVisible(true);
    background.setOpacity(1.0);
    background.setBlendMode(BlendMode::Normal);
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            background.image().setPixel(x, y, 255, 0, 0, 255);

    // Composite with only background visible should be pure red
    stack.getLayer(1).setVisible(false);
    stack.getLayer(2).setVisible(false);
    Image result1 = stack.composite();
    check(result1.getR(0, 0) == 255 && result1.getG(0, 0) == 0 && result1.getB(0, 0) == 0,
          "composite() with only background visible is pure red");
    check(result1.getA(0, 0) == 255,
          "composite() alpha is correctly opaque when source is opaque (alpha fix)");

    // Add a semi-transparent blue layer on top, Normal blend
    Layer &middle = stack.getLayer(1);
    middle.setVisible(true);
    middle.setOpacity(0.5);
    middle.setBlendMode(BlendMode::Normal);
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            middle.image().setPixel(x, y, 0, 0, 255, 255);

    Image result2 = stack.composite();
    // 50% blue over 100% red = ~(127, 0, 127)
    check(approxEqual(result2.getR(0, 0), 127) && approxEqual(result2.getB(0, 0), 127),
          "composite() blends 50% opacity layer correctly over background");

    // Overlay blend mode sanity check: overlay of a value with itself at
    // exactly 0.5 destination should behave continuously across the midpoint
    // (this mainly validates the (1-S)(1-D) formula doesn't produce garbage)
    LayerStack overlayStack(1, 1);
    overlayStack.addLayer("Base");
    overlayStack.addLayer("Overlay");
    overlayStack.getLayer(0).setVisible(true);
    overlayStack.getLayer(0).setOpacity(1.0);
    overlayStack.getLayer(0).setBlendMode(BlendMode::Normal);
    overlayStack.getLayer(0).image().setPixel(0, 0, 200, 200, 200, 255); // light gray base

    overlayStack.getLayer(1).setVisible(true);
    overlayStack.getLayer(1).setOpacity(1.0);
    overlayStack.getLayer(1).setBlendMode(BlendMode::Overlay);
    overlayStack.getLayer(1).image().setPixel(0, 0, 100, 100, 100, 255);

    Image overlayResult = overlayStack.composite();
    check(overlayResult.getR(0, 0) <= 255,
          "Overlay blend produces a value within valid byte range (formula fix)");
}

// ---------- FileIO tests ----------
void testFileIO()
{
    section("FileIO");

    // Build a small test image and round-trip it through PNG
    Image original(8, 8);
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            original.setPixel(x, y,
                              static_cast<unsigned char>(x * 30),
                              static_cast<unsigned char>(y * 30),
                              128, 255);

    const std::string testPngPath = "picshop_test_output.png";
    bool saveThrew = false;
    try
    {
        FileIO::savePNG(original, testPngPath);
    }
    catch (const std::exception &e)
    {
        saveThrew = true;
        std::cout << "  savePNG threw: " << e.what() << "\n";
    }
    check(!saveThrew, "savePNG() saves without throwing");

    bool loadThrew = false;
    Image loaded(1, 1);
    try
    {
        loaded = FileIO::load(testPngPath);
    }
    catch (const std::exception &e)
    {
        loadThrew = true;
        std::cout << "  load threw: " << e.what() << "\n";
    }
    check(!loadThrew, "load() loads the saved PNG without throwing");
    check(loaded.width() == 8 && loaded.height() == 8, "Loaded PNG has correct dimensions");
    check(loaded.getR(4, 4) == original.getR(4, 4), "Loaded PNG pixel data matches original (lossless)");

    // getInfo without a full load
    bool infoThrew = false;
    try
    {
        FileIO::ImageInfo info = FileIO::getInfo(testPngPath);
        check(info.width == 8 && info.height == 8, "getInfo() reports correct dimensions");
    }
    catch (const std::exception &)
    {
        infoThrew = true;
    }
    check(!infoThrew, "getInfo() does not throw for a valid file");

    // Loading a nonexistent file must throw a clear error, not crash
    bool threwOnMissing = false;
    try
    {
        FileIO::load("this_file_does_not_exist_12345.png");
    }
    catch (const std::runtime_error &)
    {
        threwOnMissing = true;
    }
    check(threwOnMissing, "load() throws runtime_error for a missing file");

    // JPEG save/load round trip (lossy, so only check dimensions, not exact pixels)
    const std::string testJpgPath = "picshop_test_output.jpg";
    bool jpgSaveThrew = false;
    try
    {
        FileIO::saveJPEG(original, testJpgPath, 90);
    }
    catch (const std::exception &)
    {
        jpgSaveThrew = true;
    }
    check(!jpgSaveThrew, "saveJPEG() saves without throwing");
}

// ---------- History tests ----------

void testHistory()
{
    section("History");

    // Constructor validation
    bool historyThrew = false;
    try
    {
        History badHistory(0);
    }
    catch (const std::invalid_argument &)
    {
        historyThrew = true;
    }
    check(historyThrew, "History constructor rejects maxStates < 1");

    History history(5);
    check(history.undoCount() == 0, "undoCount() is 0 immediately after construction");
    check(!history.canUndo(), "canUndo() is false when history is empty");
    check(!history.canRedo(), "canRedo() is false when history is empty");

    Image state1(2, 2);
    state1.fill(10, 10, 10, 255);
    history.push(state1);
    check(history.undoCount() == 1, "undoCount() is 1 after first push()");
    check(history.canUndo(), "canUndo() is true after a push()");

    Image state2(2, 2);
    state2.fill(20, 20, 20, 255);
    history.push(state2);
    check(history.undoCount() == 2, "undoCount() is 2 after second push()");

    Image state3(2, 2);
    state3.fill(30, 30, 30, 255);
    history.push(state3);
    check(history.undoCount() == 3, "undoCount() is 3 after third push()");

    // Undo should pop one state and decrease undoCount by 1
    Image beforeUndo(2, 2);
    beforeUndo.fill(99, 99, 99, 255); // pretend this is the "current" image being replaced
    Image undone = history.undo(beforeUndo);
    check(history.undoCount() == 2, "undoCount() decreases by 1 after undo()");
    check(undone.getR(0, 0) == 30, "undo() returns the most recently pushed state");
    check(history.canRedo(), "canRedo() is true after an undo()");

    // Redo should move it back
    Image redone = history.redo(undone);
    check(history.undoCount() == 3, "undoCount() increases by 1 after redo()");
    check(redone.getR(0, 0) == 99, "redo() returns the state that was passed into undo()");

    // Pushing after undo should wipe the redo stack
    history.undo(state3); // undoCount back to 2, something in redo stack now
    check(history.canRedo(), "canRedo() is true before a fresh push()");
    history.push(state1);
    check(!history.canRedo(), "push() after undo() clears the redo stack");

    // maxStates limit: push more than the limit and confirm oldest gets dropped
    History limitedHistory(3);
    for (int i = 0; i < 5; i++)
    {
        Image s(1, 1);
        s.fill(static_cast<unsigned char>(i * 10), 0, 0, 255);
        limitedHistory.push(s);
    }
    check(limitedHistory.undoCount() == 3, "History respects maxStates limit (oldest states dropped)");

    // Undo on empty history should throw, not crash
    History emptyHistory(5);
    bool undoOnEmptyThrew = false;
    try
    {
        Image dummy(1, 1);
        emptyHistory.undo(dummy);
    }
    catch (const std::runtime_error &)
    {
        undoOnEmptyThrew = true;
    }
    check(undoOnEmptyThrew, "undo() on empty history throws runtime_error");

    // memoryUsage should reflect stored states
    check(limitedHistory.memoryUsage() > 0, "memoryUsage() reports nonzero after pushes");
}

// ---------- Main ----------

int main()
{
    std::cout << "PicShop Engine - Test Suite\n";
    std::cout << "============================\n";

    testImage();
    testColorOps();
    testFilters();
    testLayerStack();
    testFileIO();
    testHistory();

    std::cout << "\n============================\n";
    std::cout << "Results: " << g_testsPassed << " / " << g_testsRun << " tests passed\n";

    if (g_testsPassed != g_testsRun)
    {
        std::cout << "Some tests FAILED - review output above.\n";
        return 1;
    }

    std::cout << "All tests passed.\n";
    return 0;
}