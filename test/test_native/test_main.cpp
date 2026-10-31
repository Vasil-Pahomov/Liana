// Host-side unit tests (pio test -e native) for the Arduino-free parts of the firmware:
// color math, palette interpolation, enable masks and the random cycling choice.
#include <unity.h>
#include <math.h>
#include "color.h"
#include "palette.h"
#include "enable_mask.h"
#include "cycling.h"

// the firmware defines these in Liana.cpp
static unsigned int rngState = 12345;
unsigned int rng() {
    rngState = rngState * 1103515245u + 12345u;
    return rngState;
}
byte rngb() { return (byte)(rng() >> 16); }

void setUp(void) {}
void tearDown(void) {}

// ---------- Color ----------

void test_color_from_code() {
    Color c(0x123456);
    TEST_ASSERT_EQUAL_UINT8(0x12, c.r);
    TEST_ASSERT_EQUAL_UINT8(0x34, c.g);
    TEST_ASSERT_EQUAL_UINT8(0x56, c.b);
}

void test_color_interpolate_endpoints() {
    Color a(10, 20, 30), b(110, 120, 130);
    Color x0 = a.interpolate(b, 0.0f);
    Color x1 = a.interpolate(b, 1.0f);
    Color xh = a.interpolate(b, 0.5f);
    TEST_ASSERT_EQUAL_UINT8(10, x0.r);
    TEST_ASSERT_EQUAL_UINT8(110, x1.r);
    TEST_ASSERT_EQUAL_UINT8(60, xh.r);
    TEST_ASSERT_EQUAL_UINT8(80, xh.b);
}

void test_color_fade_saturates_at_zero() {
    Color c(5, 100, 255);
    c.fade(50);
    TEST_ASSERT_EQUAL_UINT8(0, c.r);
    TEST_ASSERT_EQUAL_UINT8(50, c.g);
    TEST_ASSERT_EQUAL_UINT8(205, c.b);
}

void test_color_add_saturates_at_255() {
    Color c(200, 10, 0);
    c.addSaturate(Color(100, 10, 0));
    TEST_ASSERT_EQUAL_UINT8(255, c.r);
    TEST_ASSERT_EQUAL_UINT8(20, c.g);
    TEST_ASSERT_EQUAL_UINT8(0, c.b);
}

void test_color_brightness_scales() {
    Color c(255, 128, 0);
    Color d = c.brightness(127);
    TEST_ASSERT_EQUAL_UINT8(127, d.r);
    TEST_ASSERT_EQUAL_UINT8(63, d.g);
    TEST_ASSERT_EQUAL_UINT8(0, d.b);
}

void test_color_diff() {
    TEST_ASSERT_EQUAL_INT(0, Color(1, 2, 3).colorDiff(Color(1, 2, 3)));
    TEST_ASSERT_EQUAL_INT(255 * 3, Color(0, 0, 0).colorDiff(Color(255, 255, 255)));
}

// ---------- Palette ----------

static Color palColors[] = { 0xFF0000, 0x00FF00, 0x0000FF };
static Palette pal = { 3, palColors };

void test_palette_endpoints() {
    Color first = pal.getPalColor(0.0f);
    Color last = pal.getPalColor(1.0f);
    TEST_ASSERT_EQUAL_UINT8(255, first.r);
    TEST_ASSERT_EQUAL_UINT8(0, first.b);
    TEST_ASSERT_EQUAL_UINT8(0, last.r);
    TEST_ASSERT_EQUAL_UINT8(255, last.b);
}

void test_palette_middle_is_second_color() {
    Color mid = pal.getPalColor(0.5f);
    TEST_ASSERT_EQUAL_UINT8(0, mid.r);
    TEST_ASSERT_EQUAL_UINT8(255, mid.g);
    TEST_ASSERT_EQUAL_UINT8(0, mid.b);
}

void test_palette_cycle_wraps_to_first_color() {
    Color c0 = pal.getPalColorCycle(0.0f);
    Color cAlmost1 = pal.getPalColorCycle(0.999f);
    TEST_ASSERT_EQUAL_UINT8(255, c0.r);
    // just before the end the color approaches the first one again
    TEST_ASSERT_TRUE(cAlmost1.r > 200);
    TEST_ASSERT_TRUE(cAlmost1.b < 60);
}

void test_palette_different_color_differs() {
    Color c = pal.getDifferentColor(Color(0xFF0000));
    TEST_ASSERT_TRUE(c.colorDiff(Color(0xFF0000)) > 0);
}

// ---------- enable masks ----------

void test_enabled_count_counts_zero_bits() {
    TEST_ASSERT_EQUAL_INT(10, enabledCount(0, 10));
    TEST_ASSERT_EQUAL_INT(8, enabledCount(0x0003, 10));
    // bits above `count` are ignored
    TEST_ASSERT_EQUAL_INT(10, enabledCount(0xFC00, 10));
}

void test_is_enabled_range() {
    TEST_ASSERT_TRUE(isEnabled(0x0002, 0, 10));
    TEST_ASSERT_FALSE(isEnabled(0x0002, 1, 10));
    TEST_ASSERT_FALSE(isEnabled(0, 10, 10));
    TEST_ASSERT_FALSE(isEnabled(0, -1, 10));
}

void test_with_enabled_sets_and_clears() {
    uint16_t m = withEnabled(0, 3, 10, false);
    TEST_ASSERT_EQUAL_UINT16(0x0008, m);
    m = withEnabled(m, 3, 10, true);
    TEST_ASSERT_EQUAL_UINT16(0, m);
    // out of range leaves the mask unchanged
    TEST_ASSERT_EQUAL_UINT16(0x0008, withEnabled(0x0008, 12, 10, false));
}

// ---------- cycling ----------

static int nextRandom = 0;
static int fakeRandom(int from, int to) {
    int span = to - from;
    return from + (span > 0 ? (nextRandom % span) : 0);
}

void test_pick_skips_disabled_and_current() {
    // 10 effects, 1..9 cycle, 2 and 3 disabled, current is 1 -> candidates 4..9
    uint16_t mask = withEnabled(withEnabled(0, 2, 10, false), 3, 10, false);
    nextRandom = 0;
    TEST_ASSERT_EQUAL_INT(4, pickRandomEnabled(mask, 10, 1, 10, 1, fakeRandom));
    nextRandom = 5;
    TEST_ASSERT_EQUAL_INT(9, pickRandomEnabled(mask, 10, 1, 10, 1, fakeRandom));
}

void test_pick_returns_minus_one_without_candidates() {
    // everything but effect 5 disabled and 5 is the current one
    uint16_t mask = 0x03FF & ~(1u << 5);
    TEST_ASSERT_EQUAL_INT(-1, pickRandomEnabled(mask, 10, 1, 10, 5, fakeRandom));
    // with nothing excluded the only enabled one is chosen
    TEST_ASSERT_EQUAL_INT(5, pickRandomEnabled(mask, 10, 1, 10, -1, fakeRandom));
}

void test_pick_never_returns_start_animation() {
    for (nextRandom = 0; nextRandom < 20; nextRandom++) {
        int r = pickRandomEnabled(0, 10, 1, 10, -1, fakeRandom);
        TEST_ASSERT_TRUE(r >= 1 && r <= 9);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_color_from_code);
    RUN_TEST(test_color_interpolate_endpoints);
    RUN_TEST(test_color_fade_saturates_at_zero);
    RUN_TEST(test_color_add_saturates_at_255);
    RUN_TEST(test_color_brightness_scales);
    RUN_TEST(test_color_diff);
    RUN_TEST(test_palette_endpoints);
    RUN_TEST(test_palette_middle_is_second_color);
    RUN_TEST(test_palette_cycle_wraps_to_first_color);
    RUN_TEST(test_palette_different_color_differs);
    RUN_TEST(test_enabled_count_counts_zero_bits);
    RUN_TEST(test_is_enabled_range);
    RUN_TEST(test_with_enabled_sets_and_clears);
    RUN_TEST(test_pick_skips_disabled_and_current);
    RUN_TEST(test_pick_returns_minus_one_without_candidates);
    RUN_TEST(test_pick_never_returns_start_animation);
    return UNITY_END();
}
