#include "../00_Utils/lfast_comms.h"
#include <gtest/gtest.h>
#include <map>
// cd build && ctest --output-on-failure .

TEST(lfast_comms_tests, test_No_arg)
{
    LFAST::TxMessage msg("TestMessage");

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":""})");
}

TEST(lfast_comms_tests, oneIntArgPos)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg", 1137);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":1137}})");
}
TEST(lfast_comms_tests, oneIntArgNeg)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg", -1137);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":-1137}})");
}
TEST(lfast_comms_tests, oneUintArg)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg", 0x1234ABCDU);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":"0x1234abcd"}})");
}

TEST(lfast_comms_tests, oneBoolArgTrue)
{
    LFAST::TxMessage msg("TestMessage");
    bool testVal = true;
    msg.addArgument("TestArg", testVal);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":true}})");
}

TEST(lfast_comms_tests, oneBoolArgFalse)
{
    LFAST::TxMessage msg("TestMessage");
    bool testVal = false;
    msg.addArgument("TestArg", testVal);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":false}})");
}

TEST(lfast_comms_tests, oneStringArg)
{
    LFAST::TxMessage msg("TestMessage");
    std::string argStr("Hello World.");
    msg.addArgument("TestArg", argStr);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":"Hello World."}})");
}

TEST(lfast_comms_tests, oneCharStringArg)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg", "Hello World.");
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":"Hello World."}})");
}

TEST(lfast_comms_tests, oneDoubleArg)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg", 77.1234567);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":77.1234567}})");
}

TEST(lfast_comms_tests, twoDoubleArgs)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg1", 77.1234567);
    msg.addArgument("TestArg2", -99.7654321);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg1":77.1234567,"TestArg2":-99.7654321}})");
}

TEST(lfast_comms_tests, oneDoubleOneBool)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg1", 77.1234567);
    msg.addArgument("TestArg2", true);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg1":77.1234567,"TestArg2":true}})");
}

TEST(lfast_comms_tests, oneDoubleOneBoolOneInt)
{
    LFAST::TxMessage msg("TestMessage");
    msg.addArgument("TestArg1", 77.1234567);
    msg.addArgument("TestArg2", true);
    msg.addArgument("TestArg3", 17);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg1":77.1234567,"TestArg2":true,"TestArg3":17}})");
}

TEST(lfast_comms_tests, nestedCommand0)
{
    LFAST::TxMessage msgParent("ParentMessage");
    msgParent.addArgument("ParentArg1", 77.1234567);

    LFAST::TxMessage msgChild("ChildMessageOutside");
    msgChild.addArgument("ChildArg1", true);
    msgChild.addArgument("ChildArg2", 17);

    msgParent.addArgument("ChildMessageInside", msgChild);
    EXPECT_STREQ(msgParent.getMessageStr().c_str(), R"({"ParentMessage":{"ParentArg1":77.1234567,"ChildMessageInside":{"ChildMessageOutside":{"ChildArg1":true,"ChildArg2":17}}}})");
}

TEST(lfast_comms_tests, nestedCommand1)
{
    LFAST::TxMessage msgParent("ParentMessage");
    msgParent.addArgument("ParentArg1", 77.1234567);

    LFAST::TxMessage msgChild;
    msgChild.addArgument("ChildArg1", true);
    msgChild.addArgument("ChildArg2", 17);

    msgParent.addArgument("ChildMessage", msgChild);
    EXPECT_STREQ(msgParent.getMessageStr().c_str(), R"({"ParentMessage":{"ParentArg1":77.1234567,"ChildMessage":{"ChildArg1":true,"ChildArg2":17}}})");
}

TEST(lfast_comms_tests, isNumeric_doubleTests)
{
    std::string testStr1("1.2345");
    EXPECT_TRUE(LFAST::isNumeric(testStr1));

    std::string testStr2("12345");
    EXPECT_TRUE(LFAST::isNumeric(testStr2));

    std::string testStr3("1.23a45");
    EXPECT_FALSE(LFAST::isNumeric(testStr3));
}

TEST(lfast_comms_tests, isObjTest)
{
    EXPECT_TRUE(LFAST::isObject(R"({"ObjLabel":{"ObjKey1":1137}})"));
    EXPECT_FALSE(LFAST::isObject(R"("ObjLabel":{"ObjKey1":1137})"));
}

TEST(lfast_comms_tests, simpleRxParserTest)
{
    auto rxMsg = new LFAST::RxMessage(R"({"ObjKey":1137})");
    EXPECT_STREQ(rxMsg->data["ObjKey"].c_str(), "1137");
    EXPECT_FALSE(rxMsg->child);
}

TEST(lfast_comms_tests, nestedRxParserTest_oneDeep)
{
    auto rxMsgParent = new LFAST::RxMessage(R"({"ParentKey":{"ChildKey":1137}})");
    std::string childStr = rxMsgParent->data["ParentKey"];
    EXPECT_STREQ(childStr.c_str(), R"({"ChildKey":1137})");

    std::string childVal;
    auto rxMsgChild = rxMsgParent->child;
    if (rxMsgChild)
    {
        childVal = rxMsgChild->data["ChildKey"];
    }
    EXPECT_STREQ(childVal.c_str(), "1137");
}

// TEST(lfast_comms_tests, objParse)
// {
//     std::map<std::string, std::string> kvMap;
//     if (LFAST::tryGetObjectContents(R"({"ObjLabel":{"ObjKey1":1137}})", kvMap))
//     {
//         auto keyStr = kvMap["ObjLabel"];
//         EXPECT_STREQ(keyStr.c_str(), R"({"ObjKey1":1137})");
//     }
//     else
//     {
//         LFAST::print_map(kvMap);
//         FAIL();
//     }
// }

// TEST(lfast_comms_tests, keyValueParse)
// {
//     std::map<std::string, std::string> kvMap;

//     if (LFAST::tryGetKeyValueMap(R"("KeyStr": "ValueStr")", kvMap))
//     {
//         auto keyStr = kvMap["KeyStr"];
//         EXPECT_STREQ(keyStr.c_str(), "ValueStr");
//     }
//     else
//     {
//         LFAST::print_map(kvMap);
//         FAIL();
//     }
// }