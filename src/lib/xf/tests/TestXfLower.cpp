#include "ir/IRStmts.h"
#include "ir/IRTypes.h"
#include "ir/IRValues.h"
#include "slo/SloShader.h"
#include "slo/SloOutputFile.h"
#include "util/UtLog.h"
#include "xf/XfLower.h"
#include <gtest/gtest.h>
#include <stdio.h>

class TestXfLower : public testing::Test { };

TEST_F(TestXfLower, TestLowerFloatTy)
{
    UtLog log(stderr);
    XfLowerImpl lower(&log);
    IRTypes types;
    SloSymbol sym;
    const IRType* ty = types.GetFloatTy();
    lower.LowerType(ty, &sym);
    EXPECT_EQ(kSloFloat, sym.mType);
    EXPECT_FALSE(sym.IsArray());
}

TEST_F(TestXfLower, TestLowerArrayTy)
{
    UtLog log(stderr);
    XfLowerImpl lower(&log);
    IRTypes types;
    const IRType* ty = types.GetArrayType(types.GetFloatTy(), 3);
    SloSymbol sym;
    lower.LowerType(ty, &sym);
    EXPECT_TRUE(sym.IsArray());
    EXPECT_EQ(kSloFloat, sym.mType);
    EXPECT_EQ(3, sym.mLength);
}

TEST_F(TestXfLower, TestLowerFloatConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRNumConst c1(1.0f, "C1");
    int index = lower.TestLowerConstant(&c1);
    
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("C1", sym.mName.c_str());
    EXPECT_EQ(kSloFloat, sym.mType);
    EXPECT_EQ(kSloConstant, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloUniform, sym.mDetail);
    // The length is kSloSymbolNoLength, which is private.
    EXPECT_FALSE(sym.IsArray());
    EXPECT_EQ(1U, shader->mConstants.size());
    EXPECT_EQ(1.0f, shader->mConstants[sym.mOffset]);

    IRNumConst c2(1.0f, "C2");
    index = lower.TestLowerConstant(&c2);
    const SloSymbol& sym2 = lower.GetShader()->mSymbols[index];
    EXPECT_EQ(1, sym2.mOffset);
    EXPECT_EQ(2U, shader->mConstants.size());
}

TEST_F(TestXfLower, TestLowerPointConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRTypes types;
    const float data[] = { 1.0f, 2.0f, 3.0f };
    IRNumConst c1(data, types.GetPointTy(), "P1");
    int index = lower.TestLowerConstant(&c1);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("P1", sym.mName.c_str());
    EXPECT_EQ(kSloPoint, sym.mType);
    EXPECT_EQ(kSloConstant, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloUniform, sym.mDetail);
    // The length is kSloSymbolNoLength, which is private.
    EXPECT_FALSE(sym.IsArray());
    EXPECT_EQ(3U, shader->mConstants.size());
    float* got = &shader->mConstants[sym.mOffset];
    EXPECT_EQ(1.0f, got[0]);
    EXPECT_EQ(2.0f, got[1]);
    EXPECT_EQ(3.0f, got[2]);

    const float data2[] = { 4.0f, 5.0f, 6.0f };
    IRNumConst c2(data, types.GetPointTy(), "P2");
    index = lower.TestLowerConstant(&c2);
    const SloSymbol& sym2 = lower.GetShader()->mSymbols[index];
    EXPECT_EQ(3, sym2.mOffset);
    EXPECT_EQ(6U, shader->mConstants.size());
}

TEST_F(TestXfLower, TestLowerStringConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRStringConst c1("hello", "S1");
    int index = lower.TestLowerConstant(&c1);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("S1", sym.mName.c_str());
    EXPECT_EQ(kSloString, sym.mType);
    EXPECT_EQ(kSloConstant, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloUniform, sym.mDetail);
    // The length is kSloSymbolNoLength, which is private.
    EXPECT_FALSE(sym.IsArray());
    EXPECT_EQ(1U, shader->mStrings.size());
    EXPECT_STREQ("hello", shader->mStrings[sym.mOffset].c_str());

    IRStringConst c2("goodbye", "S2");
    index = lower.TestLowerConstant(&c2);
    const SloSymbol& sym2 = lower.GetShader()->mSymbols[index];
    EXPECT_EQ(1, sym2.mOffset);
    EXPECT_EQ(2U, shader->mStrings.size());
}

TEST_F(TestXfLower, TestRetainEmptyStringConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRStringConst empty("");
    int emptyIndex = lower.TestLowerConstant(&empty);
    EXPECT_EQ(emptyIndex, lower.GetEmptyString());
}

TEST_F(TestXfLower, TestNewEmptyStringConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    int emptyIndex = lower.GetEmptyString();
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[emptyIndex];

    EXPECT_STREQ("", shader->mStrings[sym.mOffset].c_str());
    EXPECT_EQ(kSloString, sym.mType);
    EXPECT_EQ(kSloConstant, sym.mStorage);
    EXPECT_EQ(kSloUniform, sym.mDetail);
    // The length is kSloSymbolNoLength, which is private.
    EXPECT_FALSE(sym.IsArray());
    EXPECT_EQ(1U, shader->mStrings.size());

    // Request another empty string.
    EXPECT_EQ(emptyIndex, lower.GetEmptyString());
    EXPECT_EQ(1U, shader->mStrings.size());
}

TEST_F(TestXfLower, TestLowerArrayConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRTypes types;
    const IRArrayType* ty = types.GetArrayType(types.GetFloatTy(), 3);
    const float elements[] = { 1.0f, 2.0f, 3.0f };
    IRNumArrayConst c1(elements, ty, "A1");
    int index = lower.TestLowerConstant(&c1);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("A1", sym.mName.c_str());
    EXPECT_EQ(kSloFloat, sym.mType);
    EXPECT_EQ(kSloConstant, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloUniform, sym.mDetail);
    EXPECT_TRUE(sym.IsArray());
    EXPECT_EQ(3, sym.mLength);
    EXPECT_EQ(3U, shader->mConstants.size());
    EXPECT_EQ(3.0f, shader->mConstants[sym.mOffset+2]);

    IRNumConst c2(1.0f, "C2");
    index = lower.TestLowerConstant(&c2);
    const SloSymbol& sym2 = lower.GetShader()->mSymbols[index];
    EXPECT_EQ(3, sym2.mOffset);
    EXPECT_EQ(4U, shader->mConstants.size());
}

TEST_F(TestXfLower, TestLowerStringArrayConst)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRTypes types;
    const IRArrayType* ty = types.GetArrayType(types.GetStringTy(), 3);
    std::vector<std::string> elements;
    elements.push_back("one");
    elements.push_back("two");
    elements.push_back("three");
    IRStringArrayConst c(&elements[0], ty, "SA1");
    int index = lower.TestLowerConstant(&c);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("SA1", sym.mName.c_str());
    EXPECT_EQ(kSloString, sym.mType);
    EXPECT_EQ(kSloConstant, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloUniform, sym.mDetail);
    EXPECT_TRUE(sym.IsArray());
    EXPECT_EQ(3, sym.mLength);
    EXPECT_EQ(3U, shader->mStrings.size());
    EXPECT_STREQ("three", shader->mStrings[sym.mOffset+2].c_str());

    IRStringConst c2("goodbye", "S2");
    index = lower.TestLowerConstant(&c2);
    const SloSymbol& sym2 = lower.GetShader()->mSymbols[index];
    EXPECT_EQ(3, sym2.mOffset);
    EXPECT_EQ(4U, shader->mStrings.size());
}

TEST_F(TestXfLower, TestAnonConsts)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);

    IRNumConst c1(1.0f);
    int index = lower.TestLowerConstant(&c1);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    IRStringConst c2("hello");
    index = lower.TestLowerConstant(&c2);
    const SloSymbol& sym2 = shader->mSymbols[index];
    sym2.Write(&out);

    IRTypes types;
    const IRArrayType* ty3 = types.GetArrayType(types.GetFloatTy(), 3);
    const float elements3[] = { 1.0f, 2.0f, 3.0f };
    IRNumArrayConst c3(elements3, ty3);
    index = lower.TestLowerConstant(&c3);
    const SloSymbol& sym3 = shader->mSymbols[index];
    sym3.Write(&out);

    const IRArrayType* ty4 = types.GetArrayType(types.GetStringTy(), 3);
    std::vector<std::string> elements4;
    elements4.push_back("one");
    elements4.push_back("two");
    elements4.push_back("three");
    IRStringArrayConst c4(&elements4[0], ty4);
    index = lower.TestLowerConstant(&c4);
    const SloSymbol& sym4 = shader->mSymbols[index];
    sym4.Write(&out);
}

TEST_F(TestXfLower, TestLowerGlobal)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);
    IRTypes types;

    IRGlobalVar v("P", types.GetPointTy(), kIRVarying, NULL, false);
    int index = lower.LowerGlobal(&v);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("P", sym.mName.c_str());
    EXPECT_EQ(kSloPoint, sym.mType);
    EXPECT_EQ(kSloGlobal, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloVarying, sym.mDetail);
    // The length is kSloSymbolNoLength, which is private.
    EXPECT_FALSE(sym.IsArray());
    EXPECT_EQ(1U, shader->mSymbols.size());
}

TEST_F(TestXfLower, TestLowerParam)
{
    UtLog log(stderr);
    SloOutputFile out(stdout, &log);
    XfLowerImpl lower(&log);
    IRTypes types;

    IRShaderParam v("v", types.GetPointTy(), kIRVarying, NULL,
                    true, new IRSeq());
    int index = lower.LowerShaderParam(&v);
    SloShader* shader = lower.GetShader();
    const SloSymbol& sym = shader->mSymbols[index];
    sym.Write(&out);

    EXPECT_STREQ("v", sym.mName.c_str());
    EXPECT_EQ(kSloPoint, sym.mType);
    EXPECT_EQ(kSloOutput, sym.mStorage);
    EXPECT_EQ(-1, sym.mSpace);
    EXPECT_EQ(0, sym.mOffset);
    EXPECT_EQ(kSloVarying, sym.mDetail);
    // The length is kSloSymbolNoLength, which is private.
    EXPECT_FALSE(sym.IsArray());
    EXPECT_EQ(1U, shader->mSymbols.size());
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

