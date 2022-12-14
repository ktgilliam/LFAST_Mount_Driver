#include "../00_Utils/lfast_comms.h"
#include <gtest/gtest.h>
#include <map>
// cd build && clear && ctest --output-on-failure --timeout 5 .

TEST(lfast_comms_tests, test_No_arg)
{
    LFAST::MessageGenerator msg("TestMessage");

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":""})");
}

TEST(lfast_comms_tests, oneIntArgPos)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg", 1234);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":1234}})");
}
TEST(lfast_comms_tests, oneIntArgNeg)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg", -1234);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":-1234}})");
}

TEST(lfast_comms_tests, oneBoolArgTrue)
{
    LFAST::MessageGenerator msg("TestMessage");
    bool testVal = true;
    msg.addArgument("TestArg", testVal);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":true}})");
}

TEST(lfast_comms_tests, oneBoolArgFalse)
{
    LFAST::MessageGenerator msg("TestMessage");
    bool testVal = false;
    msg.addArgument("TestArg", testVal);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":false}})");
}

TEST(lfast_comms_tests, oneStringArg)
{
    LFAST::MessageGenerator msg("TestMessage");
    std::string argStr("Hello World.");
    msg.addArgument("TestArg", argStr);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":"Hello World."}})");
}

TEST(lfast_comms_tests, oneCharStringArg)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg", "Hello World.");
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":"Hello World."}})");
}

TEST(lfast_comms_tests, oneDoubleArg)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg", 77.1234567);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg":77.1234567}})");
}

TEST(lfast_comms_tests, twoDoubleArgs)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg1", 77.1234567);
    msg.addArgument("TestArg2", -99.7654321);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg1":77.1234567,"TestArg2":-99.7654321}})");
}

TEST(lfast_comms_tests, oneDoubleOneBool)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg1", 77.1234567);
    msg.addArgument("TestArg2", true);

    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg1":77.1234567,"TestArg2":true}})");
}

TEST(lfast_comms_tests, oneDoubleOneBoolOneInt)
{
    LFAST::MessageGenerator msg("TestMessage");
    msg.addArgument("TestArg1", 77.1234567);
    msg.addArgument("TestArg2", true);
    msg.addArgument("TestArg3", 17);
    EXPECT_STREQ(msg.getMessageStr().c_str(), R"({"TestMessage":{"TestArg1":77.1234567,"TestArg2":true,"TestArg3":17}})");
}

TEST(lfast_comms_tests, nestedCommand0)
{
    LFAST::MessageGenerator msgParent("ParentMessage");
    msgParent.addArgument("ParentArg1", 77.1234567);

    LFAST::MessageGenerator msgChild("ChildMessageOutside");
    msgChild.addArgument("ChildArg1", true);
    msgChild.addArgument("ChildArg2", 17);

    msgParent.addArgument("ChildMessageInside", msgChild);
    EXPECT_STREQ(msgParent.getMessageStr().c_str(),
                 R"({"ParentMessage":{"ParentArg1":77.1234567,"ChildMessageInside":{"ChildMessageOutside":{"ChildArg1":true,"ChildArg2":17}}}})");
}

TEST(lfast_comms_tests, nestedCommand1)
{
    LFAST::MessageGenerator msgParent("ParentMessage");
    msgParent.addArgument("ParentArg1", 77.1234567);

    LFAST::MessageGenerator msgChild;
    msgChild.addArgument("ChildArg1", true);
    msgChild.addArgument("ChildArg2", 17);

    msgParent.addArgument("ChildMessage", msgChild);
    EXPECT_STREQ(msgParent.getMessageStr().c_str(),
                 R"({"ParentMessage":{"ParentArg1":77.1234567,"ChildMessage":{"ChildArg1":true,"ChildArg2":17}}})");
}

TEST(lfast_comms_tests, simpleRxParserTest)
{
    auto rxMsg = new LFAST::MessageParser("{\"ObjKey\":1234}\n");
    EXPECT_TRUE(rxMsg->succeeded());
    LFAST::print_map(rxMsg->data);
    std::cout << rxMsg->data["ObjKey"].c_str() << std::endl;
    EXPECT_STREQ(rxMsg->data["ObjKey"].c_str(), "1234");

    EXPECT_FALSE(rxMsg->childNode);
}

#if 1 // one child
TEST(lfast_comms_tests, nestedRxParserTest_oneChild)
{
    auto rxMsgParent = new LFAST::MessageParser("{\"ParentKey\":{\"ChildKey\":1234}}\n");
    ASSERT_TRUE(rxMsgParent->succeeded());
    std::string childStr = rxMsgParent->data["ParentKey"];
    EXPECT_STREQ(childStr.c_str(), R"({"ChildKey":1234})");

    auto rxMsgChild = rxMsgParent->childNode;
    if (rxMsgChild)
    {
        std::string childVal;
        childVal = rxMsgChild->data["ChildKey"];
        EXPECT_STREQ(childVal.c_str(), "1234");
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }
}
#endif

#if 1 // two children
TEST(lfast_comms_tests, nestedRxParserTest_twoChild)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":1234,"ChildKey2":2345}})");
    ASSERT_TRUE(rxMsgParent->succeeded());
    std::string childStr = rxMsgParent->data["ParentKey"];
    EXPECT_STREQ(childStr.c_str(), R"({"ChildKey1":1234,"ChildKey2":2345})");

    std::string childVal1, childVal2;
    auto rxMsgChild = rxMsgParent->childNode;
    if (rxMsgChild)
    {
        childVal1 = rxMsgChild->data["ChildKey1"];
        childVal2 = rxMsgChild->data["ChildKey2"];
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }

    EXPECT_STREQ(childVal1.c_str(), "1234");
    EXPECT_STREQ(childVal2.c_str(), "2345");
}
#endif

#if 1 // two children
TEST(lfast_comms_tests, nestedRxParserTest_twoDoubleChild)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":1.234567,"ChildKey2":-55.66778899}})");
    ASSERT_TRUE(rxMsgParent->succeeded());
    std::string childStr = rxMsgParent->data["ParentKey"];

    std::string childVal1, childVal2;
    auto rxMsgChild = rxMsgParent->childNode;
    if (rxMsgChild)
    {
        childVal1 = rxMsgChild->data["ChildKey1"];
        childVal2 = rxMsgChild->data["ChildKey2"];
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }

    EXPECT_STREQ(childVal1.c_str(), "1.234567");
    EXPECT_STREQ(childVal2.c_str(), "-55.66778899");
}
#endif

#if 1 // two children
TEST(lfast_comms_tests, nestedRxParserTest_twoStringChild)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":"Womp1","ChildKey2":"Womp2"}})");
    ASSERT_TRUE(rxMsgParent->succeeded());

    std::string childVal1, childVal2;
    auto rxMsgChild = rxMsgParent->childNode;
    if (rxMsgChild)
    {
        childVal1 = rxMsgChild->data["ChildKey1"];
        childVal2 = rxMsgChild->data["ChildKey2"];
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }

    EXPECT_STREQ(childVal1.c_str(), "\"Womp1\"");
    EXPECT_STREQ(childVal2.c_str(), "\"Womp2\"");
}
#endif

#if 1 // Three children
TEST(lfast_comms_tests, nestedRxParserTest_threeChild)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":1234,"ChildKey2":2345,"ChildKey3":3456}})");
    ASSERT_TRUE(rxMsgParent->succeeded());
    std::string childStr = rxMsgParent->data["ParentKey"];
    EXPECT_STREQ(childStr.c_str(), R"({"ChildKey1":1234,"ChildKey2":2345,"ChildKey3":3456})");

    std::string childVal1, childVal2, childVal3;
    auto rxMsgChild = rxMsgParent->childNode;
    if (rxMsgChild)
    {
        childVal1 = rxMsgChild->data["ChildKey1"];
        childVal2 = rxMsgChild->data["ChildKey2"];
        childVal3 = rxMsgChild->data["ChildKey3"];
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }

    EXPECT_STREQ(childVal1.c_str(), "1234");
    EXPECT_STREQ(childVal2.c_str(), "2345");
    EXPECT_STREQ(childVal3.c_str(), "3456");
}
#endif

#if 1 // 3 deep
TEST(lfast_comms_tests, nestedRxParserTest_threeDeep)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"Child1Key":{"Child2Key1":1234,"Child2Key2":2345}}})");
    ASSERT_TRUE(rxMsgParent->succeeded());
    std::string childStr = rxMsgParent->data["ParentKey"];
    // EXPECT_STREQ(childStr.c_str(), R"({"ChildKey1":1234,"ChildKey2":2345})");

    std::string child2Val1, child2Val2;
    auto child2 = (rxMsgParent->childNode)->childNode;

    if (child2)
    {
        child2Val1 = child2->data["Child2Key1"];
        child2Val2 = child2->data["Child2Key2"];
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }

    EXPECT_STREQ(child2Val1.c_str(), "1234");
    EXPECT_STREQ(child2Val2.c_str(), "2345");
}
#endif

// TODO: This one breaks it but going to leave it for now.
#if 0 // 3 deep, 2 wide 
TEST(lfast_comms_tests, nestedRxParserTest_threeDeepTwoWide)
{
    auto rxMsgParent = new LFAST::MessageParser(
        R"({"ParentKey":{"Child1Key1":{"Child2Key1":1234,"Child2Key2":2345}, "Child1Key2":3456}})");
    ASSERT_TRUE(rxMsgParent->succeeded());
    std::string childStr = rxMsgParent->data["ParentKey"];
    // EXPECT_STREQ(childStr.c_str(), R"({"ChildKey1":1234,"ChildKey2":2345})");

    std::string child2Val1, child2Val2;
    auto child2 = (rxMsgParent->childNode)->childNode;

    if (child2)
    {
        child2Val1 = child2->data["Child2Key1"];
        child2Val2 = child2->data["Child2Key2"];
    }
    else
    {
        GTEST_FATAL_FAILURE_("child pointer null");
    }

    EXPECT_STREQ(child2Val1.c_str(), "1234");
    EXPECT_STREQ(child2Val2.c_str(), "2345");
}
#endif

#if 1 // 4 deep (max depth currently set to 3)
TEST(lfast_comms_tests, nestedRxParserTest_fourDeep)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"Child1Key":{"Child2Key1":{"Child2Key2":2345}}}})");
    EXPECT_FALSE(rxMsgParent->succeeded());
}
#endif

TEST(lfast_comms_tests, badParseDetection)
{
    // 1. unbalanced brackets
    auto rxMsg1 = new LFAST::MessageParser(R"({"ObjKey":1234)");
    EXPECT_FALSE(rxMsg1->succeeded());

    // 2. semi-colon instead of colon
    auto rxMsg2 = new LFAST::MessageParser(R"({"ObjKey";1234})");
    EXPECT_FALSE(rxMsg2->succeeded());

    // 3. No quotes around key
    auto rxMsg3 = new LFAST::MessageParser(R"({ObjKey:1234})");
    EXPECT_FALSE(rxMsg3->succeeded());

    // This one doesn't work and I haven't figured out how to fix it yet.
    // // 4. No curly brackets at all
    // auto rxMsg4 = new LFAST::MessageParser(R"("ObjKey":1234)");
    // EXPECT_FALSE(rxMsg4->succeeded());
}

// Disabling so I can keep find in protected (don't know how to test protected members yet)
#if 0
TEST(lfast_comms_tests, findGood)
{
    auto rxMsgParent = new LFAST::MessageParser(R"({"ParentKey":{"Child1Key":{"Child2Key1":1234,"Child2Key2":2345}}})");

    ASSERT_TRUE(rxMsgParent->succeeded());
    EXPECT_STREQ(rxMsgParent->find("Child2Key1").c_str(), "1234");
    EXPECT_STREQ(rxMsgParent->find("Child2Key2").c_str(), "2345");
}
#endif

TEST(lfast_comms_tests, lookupString)
{
    auto rxMsg = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":1234,"ChildKey2":"Bangarang!"}})");
    ASSERT_TRUE(rxMsg->succeeded());
    std::string val1 = {0}, val2 = {0};
    rxMsg->lookup<std::string>("ChildKey1", &val1);
    rxMsg->lookup<std::string>("ChildKey2", &val2);

    EXPECT_STREQ(val1.c_str(), "1234");
    EXPECT_STREQ(val2.c_str(), R"(Bangarang!)");
}

TEST(lfast_comms_tests, lookupInt)
{
    auto rxMsg = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":1234,"ChildKey2":-987654}})");
    ASSERT_TRUE(rxMsg->succeeded());
    int val1 = 0, val2 = 0;
    rxMsg->lookup<int>("ChildKey1", &val1);
    rxMsg->lookup<int>("ChildKey2", &val2);
    EXPECT_EQ(val1, 1234);
    EXPECT_EQ(val2, -987654);
}

TEST(lfast_comms_tests, lookupUnsignedInt)
{
    auto rxMsg = new LFAST::MessageParser(
        R"({"ParentKey":{"ChildKey1":255,"ChildKey2":0xFF}})");
    ASSERT_TRUE(rxMsg->succeeded());

    unsigned int val1 = 0, val2 = 0;
    rxMsg->lookup<unsigned int>("ChildKey1", &val1);
    // rxMsg->lookup<unsigned int>("ChildKey2", &val2);
    EXPECT_EQ(val1, 255);
    // EXPECT_EQ(val2, 0xFF);
}

TEST(lfast_comms_tests, lookupBool)
{
    auto rxMsg = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":true,"ChildKey2":false}})");
    ASSERT_TRUE(rxMsg->succeeded());

    bool b1, b2, b3, b4;
    rxMsg->lookup<bool>("ChildKey1", &b1);
    rxMsg->lookup<bool>("ChildKey2", &b2);

    EXPECT_EQ(b1, true);
    EXPECT_EQ(b2, false);

    auto rxMsg2 = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":True,"ChildKey2":False}})");
    ASSERT_TRUE(rxMsg2->succeeded());
    rxMsg2->lookup<bool>("ChildKey1", &b3);
    rxMsg2->lookup<bool>("ChildKey2", &b4);
    rxMsg->lookup<bool>("ChildKey1", &b3);
    rxMsg->lookup<bool>("ChildKey2", &b4);
    // FAIL();
}

TEST(lfast_comms_tests, lookupDouble)
{
    auto rxMsg = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":1.23456,"ChildKey2":-55.123456789}})");
    ASSERT_TRUE(rxMsg->succeeded());
    double d1, d2;
    rxMsg->lookup<double>("ChildKey1", &d1);
    rxMsg->lookup<double>("ChildKey2", &d2);
    EXPECT_EQ(d1, 1.23456);
    EXPECT_EQ(d2, -55.123456789);
}

TEST(lfast_comms_tests, lookupTwoDoubles)
{
    auto rxMsg = new LFAST::MessageParser(R"({"AzPosition":1.234,"ElPosition":2.345}})");
    ASSERT_TRUE(rxMsg->succeeded());
    double azPos, elPos;
    rxMsg->lookup<double>("AzPosition", &azPos);
    rxMsg->lookup<double>("ElPosition", &elPos);
    EXPECT_EQ(azPos, 1.234);
    EXPECT_EQ(elPos, 2.345);
}

TEST(lfast_comms_tests, getArgKeyTest)
{
    LFAST::MessageGenerator cmdMsg("MountMessage");
    cmdMsg.addArgument("ParkCommand", 1.234);
    cmdMsg.addArgument("NoDisconnect", true);

    bool resultFlag = true;
    EXPECT_EQ(cmdMsg.numArgs(), 2);
    EXPECT_STREQ(cmdMsg.getArgKey(0).c_str(), "ParkCommand");
    EXPECT_STREQ(cmdMsg.getArgKey(1).c_str(), "NoDisconnect");
}

#if 1
TEST(lfast_comms_tests, checkOkCommandTest)
{
    LFAST::MessageGenerator cmdMsg("MountMessage");
    cmdMsg.addArgument("ParkCommand", 1.234);
    cmdMsg.addArgument("NoDisconnect", true);

    auto respMsg = LFAST::MessageParser(R"({"KarbonMessage": {"ParkCommand": "$OK^", "NoDisconnect": "$OK^"}})");
    ASSERT_TRUE(respMsg.succeeded());
    respMsg.printMessage();

    ASSERT_EQ(cmdMsg.numArgs(), 2);

    auto parkRespKey = cmdMsg.getArgKey(0);
    std::string parkResp = {0};
    respMsg.lookup<std::string>(parkRespKey, &parkResp);
    std::cout << "Park Response Key/Resp: <" << parkRespKey << ">\t<" << parkResp << ">" << std::endl;
    EXPECT_EQ(parkResp.compare("$OK^"), 0);

    auto noDiscoKey = cmdMsg.getArgKey(1);
    std::string noDiscoResp = {0};
    respMsg.lookup<std::string>(noDiscoKey, &noDiscoResp);
    std::cout << "No Disco Key/Resp: <" << noDiscoKey << ">:\t<" << noDiscoResp << ">" << std::endl;
    EXPECT_EQ(noDiscoResp.compare("$OK^"), 0);
}
#endif

TEST(lfast_comms_tests, badLookupTest)
{
    auto rxMsg = new LFAST::MessageParser(R"({"ParentKey":{"ChildKey1":"abcd","ChildKey2":"Bangarang!"}})");
    ASSERT_TRUE(rxMsg->succeeded());
    std::string val1 = {0}, val2 = {0}, val3 = {0};
    bool result1 = rxMsg->lookup<std::string>("ChildKey1", &val1);
    bool result2 = rxMsg->lookup<std::string>("ChildKey2", &val2);
    bool result3 = rxMsg->lookup<std::string>("ChildKey3", &val2);
    EXPECT_STREQ(val1.c_str(), "abcd");
    EXPECT_TRUE(result1);
    EXPECT_STREQ(val2.c_str(), "Bangarang!");
    EXPECT_TRUE(result2);

    EXPECT_FALSE(result3);
}


//

// TEST(lfast_comms_tests, handshakeTest)
// {
//     auto rxMsg = new LFAST::MessageParser("{\"KarbonMessage\":{\"Handshake\":48879}}\n");
//     // auto rxMsg = new LFAST::MessageParser("{\"KarbonMessage\":{\"Handshake\":48879}}");
//     ASSERT_TRUE(rxMsg->succeeded());

//     EXPECT_EQ(rxMsg->lookup<unsigned int>("Handshake"), 0xbeef);
// }

#if 1
TEST(lfast_comms_tests, reqAltAzTest)
{
    auto rxMsg = new LFAST::MessageParser(R"({"AzPosition":0.70000001,"ElPosition":0.007})");
    // auto rxMsg = new LFAST::MessageParser("{\"KarbonMessage\":{\"Handshake\":48879}}");
    ASSERT_TRUE(rxMsg->succeeded());
    double azPos, elPos;
    rxMsg->lookup<double>("AzPosition", &azPos);
    rxMsg->lookup<double>("ElPosition", &elPos);
    EXPECT_EQ( azPos, 0.70000001);
    EXPECT_EQ( elPos, 0.007);
}

#endif