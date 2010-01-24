#include "xf/XfRaise.h"
#include "ir/IRValues.h"
#include "slo/SloEnums.h"
#include "slo/SloShader.h"
#include "util/UtCast.h"
#include "util/UtLog.h"
#include <gtest/gtest.h>
#include <iostream>

class TestXfRaise : public testing::Test { };

TEST_F(TestXfRaise, TestStringConst)
{
    SloShader slo;
    slo.mStrings.push_back("hello");
    SloSymbol sym("str", // name
                  kSloString, // type
                  kSloConstant, // storage
                  -1, // space
                  0, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRStringConst* str = UtCast<IRStringConst*>(constant);
    ASSERT_TRUE(str != NULL);
    EXPECT_STREQ("hello", str->Get());
    delete constant;
}

TEST_F(TestXfRaise, TestFloatConst)
{
    SloShader slo;
    slo.mConstants.push_back(1.0f);
    SloSymbol sym("f", // name
                  kSloFloat, // type
                  kSloConstant, // storage
                  -1, // space
                  0, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRNumConst* num = UtCast<IRNumConst*>(constant);
    ASSERT_TRUE(num != NULL);
    ASSERT_TRUE(num->IsFloat());
    EXPECT_EQ(1.0f, num->GetFloat());
    delete constant;
}

TEST_F(TestXfRaise, TestPointConst)
{
    SloShader slo;
    float constants[] = {0, 1, 2, 3}; // point starts at offset 1.
    slo.mConstants = UtVector<float>(constants, constants+4);
    SloSymbol sym("p", // name
                  kSloPoint, // type
                  kSloConstant, // storage
                  -1, // space
                  1, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRNumConst* num = UtCast<IRNumConst*>(constant);
    ASSERT_TRUE(num != NULL);
    EXPECT_FALSE(num->IsFloat());
    EXPECT_EQ(IRTypes::GetPointTy(), num->GetType());
    const float* data = num->GetData();
    EXPECT_EQ(1.0f, data[0]);
    EXPECT_EQ(2.0f, data[1]);
    EXPECT_EQ(3.0f, data[2]);
    delete constant;
}

TEST_F(TestXfRaise, TestMatrixConst)
{
    SloShader slo;
    static const float m[] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    slo.mConstants = UtVector<float>(m, m+16);
    ASSERT_EQ(16U, slo.mConstants.size());
    SloSymbol sym("m", // name
                  kSloMatrix, // type
                  kSloConstant, // storage
                  -1, // space
                  0, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRNumConst* num = UtCast<IRNumConst*>(constant);
    ASSERT_TRUE(num != NULL);
    EXPECT_FALSE(num->IsFloat());
    EXPECT_EQ(IRTypes::GetMatrixTy(), num->GetType());
    const float* data = num->GetData();
    EXPECT_EQ(1.0f, data[0]);
    EXPECT_EQ(0.0f, data[1]);
    EXPECT_EQ(1.0f, data[15]);
    delete constant;
}

TEST_F(TestXfRaise, TestNumArray)
{
    SloShader slo;
    static const float constants[] = { 1, 2, 3, 4};
    slo.mConstants = UtVector<float>(constants, constants+4);
    SloSymbol sym("a", // name
                  kSloFloat, // type
                  kSloConstant, // storage
                  -1, // space
                  0, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0, // pc1,2
                  4 // length
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRNumArrayConst* array = UtCast<IRNumArrayConst*>(constant);
    ASSERT_TRUE(array != NULL);

    const float* data = array->GetData();
    ASSERT_TRUE(data != NULL);
    EXPECT_EQ(1.0f, data[0]);
    ASSERT_EQ(4U, array->GetLength());
    EXPECT_EQ(4.0f, *array->GetElement(3));

    const IRArrayType* arrayTy = UtCast<const IRArrayType*>(array->GetType());
    ASSERT_TRUE(arrayTy != NULL);
    EXPECT_EQ(IRTypes::GetFloatTy(), arrayTy->GetElementType());
    delete constant;
}

TEST_F(TestXfRaise, TestPointArray)
{
    SloShader slo;
    static const float constants[] = { 1,1,1, 2,2,2, 3,3,3 };
    slo.mConstants = UtVector<float>(constants, constants+12);
    SloSymbol sym("a", // name
                  kSloPoint, // type
                  kSloConstant, // storage
                  -1, // space
                  0, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0, // pc1,2
                  3 // length
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRNumArrayConst* array = UtCast<IRNumArrayConst*>(constant);
    ASSERT_TRUE(array != NULL);

    const float* data = array->GetData();
    ASSERT_TRUE(data != NULL);
    EXPECT_EQ(1.0f, data[0]);
    ASSERT_EQ(3U, array->GetLength());
    const float* p3 = array->GetElement(2);
    EXPECT_EQ(3.0f, p3[2]);

    const IRArrayType* arrayTy = UtCast<const IRArrayType*>(array->GetType());
    ASSERT_TRUE(arrayTy != NULL);
    EXPECT_EQ(IRTypes::GetPointTy(), arrayTy->GetElementType());
    delete constant;
}

TEST_F(TestXfRaise, TestStringArray)
{
    SloShader slo;
    slo.mStrings.push_back("zero");
    slo.mStrings.push_back("one");      // array starts here
    slo.mStrings.push_back("two");
    slo.mStrings.push_back("three");
    SloSymbol sym("a", // name
                  kSloString, // type
                  kSloConstant, // storage
                  -1, // space
                  1, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0, // pc1,2
                  3 // length
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRConst* constant = raise.RaiseConst(sym);
    std::cout << *constant << std::endl;
    IRStringArrayConst* array = UtCast<IRStringArrayConst*>(constant);
    ASSERT_TRUE(array != NULL);

    ASSERT_EQ(3U, array->GetLength());
    EXPECT_STREQ("three", array->GetElement(2));

    const IRArrayType* arrayTy = UtCast<const IRArrayType*>(array->GetType());
    ASSERT_TRUE(arrayTy != NULL);
    EXPECT_EQ(IRTypes::GetStringTy(), arrayTy->GetElementType());
    delete constant;
}

TEST_F(TestXfRaise, TestRaiseLocal)
{
    SloShader slo;
    slo.mStrings.push_back("world"); // space name
    SloSymbol sym("x", // name
                  kSloPoint, // type
                  kSloLocal, // storage
                  0, // space
                  0, // offset
                  kSloUniform, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRLocalVar* var = raise.RaiseLocal(sym);
    var->WriteDecl(std::cout, 0);
    std::cout << std::endl;
    EXPECT_STREQ("x", var->GetShortName());
    EXPECT_STREQ("world", var->GetSpace());
    delete var;
}

TEST_F(TestXfRaise, TestRaiseGlobal)
{
    SloShader slo;
    SloSymbol sym("P", // name
                  kSloPoint, // type
                  kSloGlobal, // storage
                  -1, // space
                  0, // offset
                  kSloVarying, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRGlobalVar* var = raise.RaiseGlobal(sym);
    var->WriteDecl(std::cout, 0);
    std::cout << std::endl;
    EXPECT_STREQ("P", var->GetShortName());
    EXPECT_STREQ("", var->GetSpace());
    delete var;
}

TEST_F(TestXfRaise, TestRaiseParam)
{
    SloShader slo;
    SloSymbol sym("x", // name
                  kSloFloat, // type
                  kSloInput, // storage
                  -1, // space
                  0, // offset
                  kSloVarying, // detail
                  0,0, // unknown1,2
                  0,0, // write1,2
                  0,0 // pc1,2
                  );
    UtLog log(stderr);
    XfRaiseImpl raise(slo, &log);
    IRShaderParam* var = raise.RaiseShaderParam(sym, 0);
    var->SetInitStmt(raise.RaiseInitializer(sym));
    var->WriteDecl(std::cout, 0);
    std::cout << std::endl;
    EXPECT_STREQ("x", var->GetShortName());
    EXPECT_STREQ("", var->GetSpace());
    delete var;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

