#include "stratum/hal/lib/common/decimal_types_utils.h"

#include "gtest/gtest.h"

namespace stratum {

TEST(TypedDecimalInitializerTest, InitializeYangDecimal_Success) {
  const DecimalDigits digits = 101;
  const DecimalPrecision precision = 2;

  YangDecimal value = TypedDecimalInitializer<YangDecimal>(
      digits, precision).Init();

  EXPECT_EQ(value.digits(), digits);
  EXPECT_EQ(value.precision(), precision);
}

TEST(TypedDecimalInitializerTest, InitializeYangAllocatedDecimal_Success) {
  const DecimalDigits digits = 3901;
  const DecimalPrecision precision = 1;

  YangDecimal* value = TypedDecimalInitializer<YangDecimal>(
      digits, precision).InitAllocated();

  EXPECT_EQ(value->digits(), digits);
  EXPECT_EQ(value->precision(), precision);
}

TEST(TypedDecimalInitializerTest, InitializeGnmiDecimal_Success) {
  const DecimalDigits digits = 19;
  const DecimalPrecision precision = 0;

  GnmiDecimal value = TypedDecimalInitializer<GnmiDecimal>(
      digits, precision).Init();

  EXPECT_EQ(value.digits(), digits);
  EXPECT_EQ(value.precision(), precision);
}

TEST(TypedDecimalInitializerTest, InitializeGnmiAllocatedDecimal_Success) {
  const DecimalDigits digits = 201;
  const DecimalPrecision precision = 10;

  GnmiDecimal* value = TypedDecimalInitializer<GnmiDecimal>(
      digits, precision).InitAllocated();

  EXPECT_EQ(value->digits(), digits);
  EXPECT_EQ(value->precision(), precision);
}

TEST(TypedDecimalComparatorTest, EqualGnmiDifferentPrecision_False) {
  GnmiDecimal left = TypedDecimalInitializer<GnmiDecimal>(9011, 3).Init();
  GnmiDecimal right = TypedDecimalInitializer<GnmiDecimal>(9011, 2).Init();

  auto equal = TypedDecimalComparator::Equal<GnmiDecimal, GnmiDecimal>(
      left, right);
  EXPECT_FALSE(equal);
}

TEST(TypedDecimalComparatorTest, EqualGnmiDifferentDigits_False) {
  GnmiDecimal left = TypedDecimalInitializer<GnmiDecimal>(9010, 3).Init();
  GnmiDecimal right = TypedDecimalInitializer<GnmiDecimal>(901, 3).Init();

  auto equal = TypedDecimalComparator::Equal<GnmiDecimal, GnmiDecimal>(
      left, right);
  EXPECT_FALSE(equal);
}

TEST(TypedDecimalComparatorTest, EqualGnmiDifferentDigitsAndPrecision_False) {
  GnmiDecimal left = TypedDecimalInitializer<GnmiDecimal>(505, 2).Init();
  GnmiDecimal right = TypedDecimalInitializer<GnmiDecimal>(105, 3).Init();

  auto equal = TypedDecimalComparator::Equal<GnmiDecimal, GnmiDecimal>(
      left, right);
  EXPECT_FALSE(equal);
}

TEST(TypedDecimalComparatorTest, EqualGnmiEqualDigitsAndPrecision_True) {
  GnmiDecimal left = TypedDecimalInitializer<GnmiDecimal>(778, 1).Init();
  GnmiDecimal right = TypedDecimalInitializer<GnmiDecimal>(778, 1).Init();

  auto equal = TypedDecimalComparator::Equal<GnmiDecimal, GnmiDecimal>(
      left, right);
  EXPECT_TRUE(equal);
}

TEST(TypedDecimalComparatorTest, EqualYangDifferentPrecision_False) {
  YangDecimal left = TypedDecimalInitializer<YangDecimal>(100, 2).Init();
  YangDecimal right = TypedDecimalInitializer<YangDecimal>(100, 1).Init();

  auto equal = TypedDecimalComparator::Equal<YangDecimal, YangDecimal>(
      left, right);
  EXPECT_FALSE(equal);
}

TEST(TypedDecimalComparatorTest, EqualYangDifferentDigits_False) {
  YangDecimal left = TypedDecimalInitializer<YangDecimal>(100, 3).Init();
  YangDecimal right = TypedDecimalInitializer<YangDecimal>(1000, 3).Init();

  auto equal = TypedDecimalComparator::Equal<YangDecimal, YangDecimal>(
      left, right);
  EXPECT_FALSE(equal);
}

TEST(TypedDecimalComparatorTest, EqualYangDifferentDigitsAndPrecision_False) {
  YangDecimal left = TypedDecimalInitializer<YangDecimal>(109, 4).Init();
  YangDecimal right = TypedDecimalInitializer<YangDecimal>(9321, 3).Init();

  auto equal = TypedDecimalComparator::Equal<YangDecimal, YangDecimal>(
      left, right);
  EXPECT_FALSE(equal);
}

TEST(TypedDecimalComparatorTest, EqualYangEqualDigitsAndPrecision_True) {
  YangDecimal left = TypedDecimalInitializer<YangDecimal>(65, 2).Init();
  YangDecimal right = TypedDecimalInitializer<YangDecimal>(65, 2).Init();

  auto equal = TypedDecimalComparator::Equal<YangDecimal, YangDecimal>(
      left, right);
  EXPECT_TRUE(equal);
}

}  // namespace stratum