#include <penum/CardDistribution.h>
#include <gtest/gtest.h>
#include <locale>
#include <limits>
#include <string>

using namespace pokerstove;

TEST(CardDistribution, decimal_scientific_weights)
{
    for (const auto text : {"1", "+1", "1.", "1.0", "1e0", "1E+0", "100e-2"})
    {
        CardDistribution distribution;
        ASSERT_TRUE(distribution.parse(std::string("AcAs=") + text)) << text;
        EXPECT_DOUBLE_EQ(distribution.weight(), 1.0);
    }
    CardDistribution distribution;
    ASSERT_TRUE(distribution.parse("AcAs=+.125,KdKh=1e-3,2c3d=0"));
    EXPECT_EQ(distribution.size(), 3);
    EXPECT_DOUBLE_EQ(distribution.weight(), 0.126);
    EXPECT_EQ(distribution.str(), "AcAs=0.125,KdKh=0.001,2c3d=0.000");
    ASSERT_TRUE(distribution.parse("AcAs=-0"));
    EXPECT_EQ(distribution.str(), "AcAs=-0.000");
}

TEST(CardDistribution, failure_preserves_previous_state)
{
    for (const auto input : {"", ",", "AcAs,", ",AcAs", "AcAs,,KhKd", "AcAc",
                             "AcAs,xx", "AcAs=", "AcAs= ", "AcAs= 1", "AcAs=1 ",
                             "AcAs=1x", "AcAs=1=2", "AcAs=-1", "AcAs=nan",
                             "AcAs=+nan", "AcAs=inf", "AcAs=-inf", "AcAs=1e9999",
                             "AcAs=1e-9999", "AcAs=0x1p0", "AcAs=+", "AcAs=++1",
                             "AcAs=+-0", "KhKd=1,AcAs=bad"})
    {
        CardDistribution distribution(CardSet("2c3d"));
        EXPECT_FALSE(distribution.parse(input)) << input;
        EXPECT_EQ(distribution.str(), "2c3d=1.000") << input;
        EXPECT_EQ(distribution.size(), 1);
    }
}

TEST(CardDistribution, random_and_default_weight)
{
    CardDistribution distribution;
    ASSERT_TRUE(distribution.parse("AcAs,KhKd"));
    EXPECT_DOUBLE_EQ(distribution.weight(), 2.0);
    ASSERT_TRUE(distribution.parse("."));
    EXPECT_EQ(distribution.size(), 1);
    EXPECT_EQ(distribution[static_cast<size_t>(0)].size(), 0);
    EXPECT_EQ(distribution.str(), "=1.000");
}

namespace
{
struct comma_punctuation : std::numpunct<char>
{
    char do_decimal_point() const override { return ','; }
};
struct locale_guard
{
    std::locale original = std::locale();
    ~locale_guard() { std::locale::global(original); }
};
}

TEST(CardDistribution, numeric_parsing_is_locale_independent)
{
    locale_guard guard;
    std::locale::global(std::locale(std::locale::classic(), new comma_punctuation));
    CardDistribution distribution;
    ASSERT_TRUE(distribution.parse("AcAs=0.125"));
    EXPECT_DOUBLE_EQ(distribution.weight(), 0.125);
    EXPECT_EQ(distribution.str(), "AcAs=0,125");
}

TEST(CardDistribution, finite_numeric_limits)
{
    CardDistribution distribution;
    ASSERT_TRUE(distribution.parse("AcAs=1.7976931348623157e308"));
    EXPECT_EQ(distribution.weight(), std::numeric_limits<double>::max());
    ASSERT_TRUE(distribution.parse("AcAs=4.9406564584124654e-324"));
    EXPECT_EQ(distribution.weight(), std::numeric_limits<double>::denorm_min());
}
