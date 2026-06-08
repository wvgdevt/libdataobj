#include <algorithm>
#include <cassert>
#include <iostream>
#include <ostream>
#include <string>

#include "Mods.h"
#include "../DataObject.h"
#include "../ConvertFile.h"
using namespace dataobject;
using namespace dataobject::tests;
using namespace std;

#define PRINT_FUNC_NAME() std::cout << __PRETTY_FUNCTION__ << std::endl;

namespace {
bool tryParseJson(string const& _data)
{
    try
    {
        ConvertJsoncppStringToData(_data);
    } catch (DataObjectException const& _ex)
    {
        // std::cout << _ex.what() << std::endl;
        return false;
    }
    return true;
}
}

void dataobject_sort()
{
    PRINT_FUNC_NAME();

    DataObject obj;
    obj.setAutosort(true);
    obj["indexes"] = "1";
    obj["hash"]    = "2";
    obj["txbytes"] = "3";
    obj["logs"]    = "4";

    DataObject obj2;
    obj2["indexes"] = "1";
    obj2["hash"]    = "2";
    obj2["txbytes"] = "3";
    obj2["logs"]    = "4";
    obj2.performModifier(mod_sortKeys);

    assert(obj.asJson(0, false) == obj2.asJson(0, false));
}

void dataobject_bracers()
{
    PRINT_FUNC_NAME();
    string data = R"(
      {
      "logs": [
          {
           "address": "0x0f572e5295c57f15886f9b263e2f6d2d6c7b5ec6",
           "topics": [
            "0x000000000000000000000000095e7baea6a6c7c4c2dfeb977efac326af552d87"
           ],
           "logIndex": "0x0",
           "removed": false
          }
         ]
      }
    )";

    try
    {
        spDataObject a = ConvertJsoncppStringToData(data);
        assert(a->atKey("logs").at(0).atKey("removed").asBool() == false);
    } catch (DataObjectException const& _ex)
    {
        std::cerr << "Unexpected DataObject exception when parsing json: " + std::string(_ex.what());
    }
}

void dataobject_EscapeChars()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
                  "jsonrpc":"2.0",
                  "id":14,
                  "error":{
                    "code":-32602,
                    "message":"invalid argument 0: invalid hex or decimal integer \"0x\""
                  }
        }
    )";
    try
    {
        spDataObject a = ConvertJsoncppStringToData(data);
        assert(a->atKey("error").atKey("message").asString() ==
            "invalid argument 0: invalid hex or decimal integer \\\"0x\\\"");
    } catch (DataObjectException const&)
    {
        std::cerr << "Unexpected DataObject exception when parsing json!" << std::endl;
    }
}

void dataobject_invalidJson1()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_invalidJson2()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {]
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!";
}

void dataobject_invalidJson3()
{
    PRINT_FUNC_NAME();
    string data = R"(
        [
            "name" : {}
        ]
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_invalidJson4()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : "value" : "attempt another value"
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_invalidJson5()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : [ "element" ] : "attempt another value"
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_invalidJson6()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "account" : {
                "balance" : "0",
                "code" : ""
                "nonce" : "0"
            }
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_invalidJson7()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "account" : {
                "balance" : "0",
                "code" : "",
                "nonce" : "0
            }
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_invalidJson8()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            [
                "a",
                "b",
                "c
            }
        }
    )";
    if (!tryParseJson(data))
        return;
    std::cerr << "Expected DataObject exception when parsing json!" << std::endl;
}

void dataobject_readJson1()
{
    PRINT_FUNC_NAME();
    string data       = R"(
        {
            "name" : {
            }
        }
    )";
    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":{}}");
}

void dataobject_readJson2()
{
    PRINT_FUNC_NAME();
    string data       = R"(
        {
            "name" : {
            }
        }
    )";
    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":{}}");
}

void dataobject_readJson3()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {
                "key" : [
                ]
            }
        }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":{\"key\":[]}}");
}

void dataobject_readJson4()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {
                "key" : [
                    "aaa",
                    "bbb"
                ]
            }
        }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":{\"key\":[\"aaa\",\"bbb\"]}}");
}

void dataobject_readJson5()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {
                "key" : [
                    12,
                    34
                ]
            }
        }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":{\"key\":[12,34]}}");
}

void dataobject_readJson5b()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {
                "key" : [
                    12,
                    34
                ],
                "key2" : [
                    12,
                    34
                ]
            },
            "name2" : "value"
        }
    )";

    CJOptions opt{.stopper = "key"};
    spDataObject dObj = ConvertJsoncppStringToData(data, opt);
    assert(dObj->asJson(0, false) == "{\"name\":{\"key\":[12,34]}}");
}

void dataobject_readJson6()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : {
                "key" : -123
            }
        }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":{\"key\":-123}}");
}

void dataobject_readJson7()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
    "name" : {
        "_info" : {
            "comment" : "A tesesult",
            "filledwith" : "testee5c"
        },
        "blocks" : [
            {
                "blockHeader" : {
                    "difficulty" : "0x020000",
                    "extraData" : ""
                },
                "rlp" : "0xf90262f0",
                "transactions" : [
                    {
                        "data" : "0x",
                        "value" : "0x0186a0"
                    }
                ],
                "uncleHeaders" : [
                ]
            }
        ],
        "genesisBlockHeader" : {
            "number" : "0x00"
        },
        "genesisRLP" : "0xf9c6f04171167ec0c0",
        "lastblockhash" : "0x00606595b80acde5",
        "network" : "Homestead",
        "postState" : {
            "0x095e7baea6a6c7c4c2dfeb977efac326af552d87" : {
                "storage" : {
                    "0x00" : "0x02"
                }
            },
            "0x2adc25665018aa1fe0e6bc666dac8fc2697ff9ba" : {
                "storage" : {
                }
            },
            "0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b" : {
                "nonce" : "0x01",
                "storage" : {
                }
            }
        },
        "pre" : {
            "0x095e7baea6a6c7c4c2dfeb977efac326af552d87" : {
            },
            "0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b" : {
            }
        },
        "sealEngine" : "NoProof"
    }
        }
    )";

    string const res =
            R"({"name":{"_info":{"comment":"A tesesult","filledwith":"testee5c"},"blocks":[{"blockHeader":{"difficulty":"0x020000","extraData":""},"rlp":"0xf90262f0","transactions":[{"data":"0x","value":"0x0186a0"}],"uncleHeaders":[]}],"genesisBlockHeader":{"number":"0x00"},"genesisRLP":"0xf9c6f04171167ec0c0","lastblockhash":"0x00606595b80acde5","network":"Homestead","postState":{"0x095e7baea6a6c7c4c2dfeb977efac326af552d87":{"storage":{"0x00":"0x02"}},"0x2adc25665018aa1fe0e6bc666dac8fc2697ff9ba":{"storage":{}},"0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b":{"nonce":"0x01","storage":{}}},"pre":{"0x095e7baea6a6c7c4c2dfeb977efac326af552d87":{},"0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b":{}},"sealEngine":"NoProof"}})";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == res);
}

void dataobject_readJson8()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "name" : 12.34
        }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"name\":12.3400}");
}

void dataobject_readJson8a()
{
    PRINT_FUNC_NAME();
    DataObject dObj;
    dObj["key"] = 12.34;
    assert(dObj.asJson(0, false) == "{\"key\":12.3400}");
}

void dataobject_readJson_doublefields()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "code" :"pragma solidity ^0.4.0;",
            "code" :"contract suicideContract",
            "code" :"{",
            "code" :"    function run",
            "code" :"    {",
            "code" :"        suicide 0x1000000000000000000000000000000000000001;",
            "code" :"    }",
            "code" :"}"
        }
    )";

    try
    {
        spDataObject dObj = ConvertJsoncppStringToData(data);
        assert(dObj->asJson(0, false) == "{\"code\":\"}\"}");
    } catch (std::exception const& _ex)
    {
        assert(string(_ex.what()).find("Reading json with dublicate fields") != string::npos);
    }
}

void dataobject_readJson_doublefields_allowComments()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "code" :"pragma solidity ^0.4.0;",
            "code" :"contract suicideContract",
            "code" :"{",
            "code" :"    function run",
            "code" :"    {",
            "code" :"        suicide 0x1000000000000000000000000000000000000001;",
            "code" :"    }",
            "code" :"}"
        }
    )";

    try
    {
        dataobject::CJOptions opt{.jsonParse = CJOptions::JsonParse::ALLOW_COMMENTS};
        spDataObject dObj = ConvertJsoncppStringToData(data, opt);
        assert(dObj->asJson(0, false) == "{\"code\":\"}\"}");
    } catch (std::exception const& _ex)
    {
        assert(string(_ex.what()).find("Reading json with dublicate fields") != string::npos);
    }
}

void dataobject_readJson_doublefields_allowComments_actualComments()
{
    PRINT_FUNC_NAME();
    string data = R"(
        {
            "//code" :"pragma solidity ^0.4.0;",
            "//code" :"contract suicideContract",
            "//code" :"{",
            "//code" :"    function run",
            "//code" :"    {",
            "//code" :"        suicide 0x1000000000000000000000000000000000000001;",
            "//code" :"    }",
            "code" :"}"
        }
    )";

    dataobject::CJOptions opt{.jsonParse = CJOptions::JsonParse::ALLOW_COMMENTS};
    spDataObject dObj = ConvertJsoncppStringToData(data, opt);
    assert(dObj->asJson(0, false) == "{\"//code\":\"    }\",\"code\":\"}\"}");
}

void dataobject_readJson9()
{
    PRINT_FUNC_NAME();
    string data = R"(
    {
        "array" : [
           1, 2, -1, 5, 3
        ]
    }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"array\":[1,2,-1,5,3]}");
}

void dataobject_readJson10()
{
    PRINT_FUNC_NAME();
    string data = R"(
    {
        "array" : [
           "1", "2", "-1", "5", "3"
        ]
    }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->asJson(0, false) == "{\"array\":[\"1\",\"2\",\"-1\",\"5\",\"3\"]}");
}

void dataobject_readJson11()
{
    PRINT_FUNC_NAME();
    string data = R"(
{
    "name" : "Ethereum GO on TCP",
    "socketType" : "tcp",
    "socketAddress" : [
                "127.0.0.1:8545",
                "127.0.0.1:8552"
        ],
        "forks" : [
                "Frontier",
                "ConstantinopleFix"
        ]
}
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    assert(dObj->count("forks"));
}

void dataobject_readJson12()
{
    PRINT_FUNC_NAME();
    string data = R"(
    {
        "Byzantium" : [
            {
                "hash" : "0x",
                "indexes" : {
                    "data" : 0,
                    "value" : 0
                 }
            }
        ],
        "Constantinople" : [
            {
                "hash" : "0x",
                "indexes" : {
                    "data" : 0,
                    "value" : 0
                 }
            }
        ]
    }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    string const res  =
            R"({"Byzantium":[{"hash":"0x","indexes":{"data":0,"value":0}}],"Constantinople":[{"hash":"0x","indexes":{"data":0,"value":0}}]})";
    assert(dObj->asJson(0, false) == res);
}

void dataobject_readJson13()
{
    PRINT_FUNC_NAME();
    string data = R"(
    {
       "expect" : [
        {
              "result" : {
                  "0x0f572e5295c57f15886f9b263e2f6d2d6c7b5ec6" : {
                  }
             }
        },
        {
              "result" : {
                  "0xd27e800c69122409ac5609fe4df903745f3988a0" : {
                       "storage" : {
                          "0x01" : "0x01"
                       }
                  }
             }
        }
                  ]
    }
    )";

    spDataObject dObj = ConvertJsoncppStringToData(data);
    string const res  =
            R"({"expect":[{"result":{"0x0f572e5295c57f15886f9b263e2f6d2d6c7b5ec6":{}}},{"result":{"0xd27e800c69122409ac5609fe4df903745f3988a0":{"storage":{"0x01":"0x01"}}}}]})";
    assert(dObj->asJson(0, false) == res);
}

void dataobject_readJson14()
{
    PRINT_FUNC_NAME();
    string data       = R"( {"jsonrpc":"2.0","id":1,"result":true}   )";
    spDataObject dObj = ConvertJsoncppStringToData(data);
    string res        = R"({"jsonrpc":"2.0","id":1,"result":true})";
    assert(dObj->asJson(0, false) == res);
}

void dataobject_readJson15()
{
    PRINT_FUNC_NAME();
    string data       = R"(
                {
                    "array" : [
                        null
                    ],
                    "object" : null
                }
       )";
    spDataObject dObj = ConvertJsoncppStringToData(data);
    string res        = R"({"array":[null],"object":null})";
    assert(dObj->asJson(0, false) == res);
}

void dataobject_readJson16()
{
    PRINT_FUNC_NAME();
    string data       = R"(
                {
                    "int_field" : "u64::12345"
                }
       )";
    spDataObject dObj = ConvertJsoncppStringToData(data);
    string res        = R"({"int_field":"u64::12345"})";
    assert(dObj->asJson(0, false) == res);
    assert(dObj->atKey("int_field").type() == DataType::UInteger64);
    assert(dObj->atKey("int_field").asUInt64() == 12345);

    spDataObject out;
    out["key"]     = std::uint64_t(12345);
    string res_out = R"({"key":"u64::12345"})";
    assert(out->asJson(0, false) == res_out);
}

void dataobject_findOrderedKeyPosition_before1_of3()
{
    PRINT_FUNC_NAME();
    string const key = "aab0";
    DataObject data(DataType::Object);
    data["aab1"] = "1";
    data["abb2"] = "2";
    data["adb3"] = "3";
    size_t pos   = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 0);
}

void dataobject_findOrderedKeyPosition_before2_of3()
{
    PRINT_FUNC_NAME();
    string const key = "abb0";
    DataObject data(DataType::Object);
    data["aab1"] = "1";
    data["abb2"] = "2";
    data["adb3"] = "3";
    size_t pos   = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 1);
}

void dataobject_findOrderedKeyPosition_before3_of3()
{
    PRINT_FUNC_NAME();
    string const key = "acb4";
    DataObject data(DataType::Object);
    data["aab1"] = "1";
    data["abb2"] = "2";
    data["adb3"] = "3";
    size_t pos   = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 2);
}

void dataobject_findOrderedKeyPosition_after3_of3()
{
    PRINT_FUNC_NAME();
    string const key = "adb4";
    DataObject data(DataType::Object);
    data["aab1"] = "1";
    data["abb2"] = "2";
    data["adb3"] = "3";
    size_t pos   = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 3);
}

void dataobject_findOrderedKeyPosition_after6_of6()
{
    PRINT_FUNC_NAME();
    string const key = "v";
    DataObject data(DataType::Object);
    data["d"]  = "1";
    data["g"]  = "2";
    data["gs"] = "3";
    data["n"]  = "4";
    data["s"]  = "5";
    data["t"]  = "6";
    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 6);
}

void dataobject_findOrderedKeyPosition_empty()
{
    PRINT_FUNC_NAME();
    string const key = "adb4";
    DataObject data(DataType::Object);
    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 0);
}

void dataobject_findOrderedKeyPosition_before1_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa0";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 0);
}

void dataobject_findOrderedKeyPosition_before2_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa1b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 1);
}

void dataobject_findOrderedKeyPosition_before3_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa2b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 2);
}

void dataobject_findOrderedKeyPosition_before4_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa3b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 3);
}

void dataobject_findOrderedKeyPosition_before5_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa4b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 4);
}

void dataobject_findOrderedKeyPosition_before6_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa5b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 5);
}

void dataobject_findOrderedKeyPosition_before7_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa6b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 6);
}

void dataobject_findOrderedKeyPosition_before8_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa7b";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 7);
}

void dataobject_findOrderedKeyPosition_after8_of8()
{
    PRINT_FUNC_NAME();
    string const key = "aa9";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";
    data["aa8"] = "8";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 8);
}

void dataobject_findOrderedKeyPosition_after7_of7()
{
    PRINT_FUNC_NAME();
    string const key = "aa8";
    DataObject data(DataType::Object);
    data["aa1"] = "1";
    data["aa2"] = "2";
    data["aa3"] = "3";
    data["aa4"] = "4";
    data["aa5"] = "5";
    data["aa6"] = "6";
    data["aa7"] = "7";

    size_t pos = dataobject::findOrderedKeyPosition(key, data.getSubObjects());
    assert(pos == 7);
}

void dataobject_jsonOrder2()
{
    PRINT_FUNC_NAME();
    string const data = R"(
    {
        "balance" : "1000000000000000000",
        "nonce" : "0",
        "//comment1" : "contract Killer {",
        "//comment2" : "  function homicide {",
        "//comment3" : "      suicide(msg.sender;",
        "//comment4" : "    }",
        "//comment5" : "",
        "//comment6" : "    function multipleHomocide {",
        "//comment7" : "        Killer k  = this;",
        "//comment8" : "        k.homicide;",
        "//comment9" : "        k.homicide;",
        "//comment10" : "    }",
        "//comment11" : "",
        "//comment12" : "    function run returns ",
        "//comment13" : "    {",
        "//comment14" : "  multipleHomocide;",
        "//comment15" : "  return true;",
        "//comment16" : "    }",
        "//comment17" : "}",
        "code" : "0x606060405260e060020a600035046309e587a58114610031578063c04062261461004d578063dd4f1f2a1461005a575b005b61002f3373ffffffffffffffffffffffffffffffffffffffff16ff5b6100f5600061010961005e565b61002f5b60003090508073ffffffffffffffffffffffffffffffffffffffff166309e587a56040518160e060020a0281526004018090506000604051808303816000876161da5a03f1156100025750604080517f09e587a500000000000000000000000000000000000000000000000000000000815290516004828101926000929190829003018183876161da5a03f1156100025750505050565b604080519115158252519081900360200190f35b5060019056",
        "storage" : {
        }
    })";
    CJOptions opt{.autosort = true};
    spDataObject dObj = ConvertJsoncppStringToData(data, opt);
    assert(dObj->at(dObj->getSubObjects().size() - 2).getKey() == "nonce");
}

void dataobject_jsonOrder()
{
    PRINT_FUNC_NAME();
    DataObject data(DataType::Object);
    data.setAutosort(true);
    data["aa1"]  = "1";
    data["aa31"] = "3";
    data["aa3"]  = "3";
    data["aa7"]  = "7";
    data["aa5"]  = "5";
    data["aa2"]  = "2";
    data["aa70"] = "7";
    data["aa8"]  = "8";
    assert(data.asJson(0, false) ==
        "{\"aa1\":\"1\",\"aa2\":\"2\",\"aa3\":\"3\",\"aa31\":\"3\",\"aa5\":\"5\",\"aa7\":"
        "\"7\",\"aa70\":\"7\",\"aa8\":\"8\"}");
}

void dataobject_replace()
{
    PRINT_FUNC_NAME();
    spDataObject data;
    spDataObject data2;
    spDataObject data3;
    (*data2).setKey("key2");
    (*data2).setString("value2");
    (*data3).setKey("key3");
    (*data3).setString("value3");

    (*data).atKeyPointer("field1") = data3; // null object with key "field1" keep the key "field1"
    assert(data->asJson(0,false) == "{\"field1\":\"value3\"}");

    (*data).atKeyPointer("field1") = data2;
    // not null object with key "field1" replaces the key "field1" to data2's key
    assert(data->asJson(0,false) == "{\"key2\":\"value2\"}");
}

void dataobject_arrayhell()
{
    PRINT_FUNC_NAME();
    string const data = R"(
    {
        "array" : [
                    [
                        {
                            "address" : "0x0000000000000000000000000000000000001337",
                            "storageKeys" : [
                                "0x0000000000000000000000000000000000000000000000000000000000000001",
                                "0x0000000000000000000000000000000000000000000000000000000000000002"
                            ]
                        }
                    ]
                ]
    })";
    CJOptions opt{.autosort = true};
    spDataObject dObj = ConvertJsoncppStringToData(data, opt);
}

void dataobject_besuresponse()
{
    PRINT_FUNC_NAME();
    string const data          = R"(
        {
          "result" : {
            "transactions" : [ {
              "blockHash" : "0xac7b82af234ef01bf4d24a3b9c22c2de091c6f71ec04d51ff23bd780533d999f",
              "blockNumber" : "0x1",
              "chainId" : null,
              "from" : "0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b",
              "gas" : "0x7a120",
              "gasPrice" : "0xa",
              "hash" : "0x225117089dee26945644798e2c64d3117f55c95c7cf5509f7176de4b3af5202d",
              "input" : "0x604b80600c6000396000f3007c01000000000000000000000000000000000000000000000000000000006000350463cbf0b0c08114602d57005b60006004358073ffffffffffffffffffffffffffffffffffffffff16ff",
              "nonce" : "0x0",
              "publicKey" : "0x3a514176466fa815ed481ffad09110a2d344f6c9b78c1d14afc351c3a51be33d8072e77939dc03ba44790779b7a1025baf3003f6732430e20cd9b76d953391b3",
              "raw" : "0xf8a5800a8307a1208081ffb857604b80600c6000396000f3007c01000000000000000000000000000000000000000000000000000000006000350463cbf0b0c08114602d57005b60006004358073ffffffffffffffffffffffffffffffffffffffff16ff1ca00e7d3c664c49aa9f5ce4eb76c8547450466262a78bd093160f492ea0853c68e9a03f843e72210ff1da4fd9e375339872bcf0fad05c014e280ffc755e173700dd62",
              "to" : null,
              "transactionIndex" : "0x0",
              "value" : "0xff",
              "v" : "0x1c",
              "r" : "0xe7d3c664c49aa9f5ce4eb76c8547450466262a78bd093160f492ea0853c68e9",
              "s" : "0x3f843e72210ff1da4fd9e375339872bcf0fad05c014e280ffc755e173700dd62"
            } ]
          }
        })";
    CJOptions opt              = {.autosort = true};
    spDataObject dObj          = ConvertJsoncppStringToData(data, opt);
    string const expectedParse =
            R"({"result":{"transactions":[{"blockHash":"0xac7b82af234ef01bf4d24a3b9c22c2de091c6f71ec04d51ff23bd780533d999f","blockNumber":"0x1","chainId":null,"from":"0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b","gas":"0x7a120","gasPrice":"0xa","hash":"0x225117089dee26945644798e2c64d3117f55c95c7cf5509f7176de4b3af5202d","input":"0x604b80600c6000396000f3007c01000000000000000000000000000000000000000000000000000000006000350463cbf0b0c08114602d57005b60006004358073ffffffffffffffffffffffffffffffffffffffff16ff","nonce":"0x0","publicKey":"0x3a514176466fa815ed481ffad09110a2d344f6c9b78c1d14afc351c3a51be33d8072e77939dc03ba44790779b7a1025baf3003f6732430e20cd9b76d953391b3","r":"0xe7d3c664c49aa9f5ce4eb76c8547450466262a78bd093160f492ea0853c68e9","raw":"0xf8a5800a8307a1208081ffb857604b80600c6000396000f3007c01000000000000000000000000000000000000000000000000000000006000350463cbf0b0c08114602d57005b60006004358073ffffffffffffffffffffffffffffffffffffffff16ff1ca00e7d3c664c49aa9f5ce4eb76c8547450466262a78bd093160f492ea0853c68e9a03f843e72210ff1da4fd9e375339872bcf0fad05c014e280ffc755e173700dd62","s":"0x3f843e72210ff1da4fd9e375339872bcf0fad05c014e280ffc755e173700dd62","to":null,"transactionIndex":"0x0","v":"0x1c","value":"0xff"}]}})";

    assert(dObj->asJson(0, false) == expectedParse);
}

void dataobject_doublefields_comments_allowed()
{
    PRINT_FUNC_NAME();
    string const data = R"(
    {
        "balance" : "1000000000000000000",
        "//nonce" : "0",
        "//nonce" : "1",
        "code" : "0x606060405260e060020a600035046309e587a58114610031578063c04062261461004d578063dd4f1f2a1461005a575b005b61002f3373ffffffffffffffffffffffffffffffffffffffff16ff5b6100f5600061010961005e565b61002f5b60003090508073ffffffffffffffffffffffffffffffffffffffff166309e587a56040518160e060020a0281526004018090506000604051808303816000876161da5a03f1156100025750604080517f09e587a500000000000000000000000000000000000000000000000000000000815290516004828101926000929190829003018183876161da5a03f1156100025750505050565b604080519115158252519081900360200190f35b5060019056",
        "storage" : {
        }
    })";
    CJOptions opt     = {.jsonParse = CJOptions::JsonParse::ALLOW_COMMENTS, .autosort = true};
    spDataObject dObj = ConvertJsoncppStringToData(data, opt);
    assert(dObj->atKey("//nonce").asString() == "1");
}

void dataobject_doublefields_comments_disabled()
{
    PRINT_FUNC_NAME();
    string const data = R"(
    {
        "balance" : "1000000000000000000",
        "//nonce" : "0",
        "//nonce" : "1",
        "code" : "0x606060405260e060020a600035046309e587a58114610031578063c04062261461004d578063dd4f1f2a1461005a575b005b61002f3373ffffffffffffffffffffffffffffffffffffffff16ff5b6100f5600061010961005e565b61002f5b60003090508073ffffffffffffffffffffffffffffffffffffffff166309e587a56040518160e060020a0281526004018090506000604051808303816000876161da5a03f1156100025750604080517f09e587a500000000000000000000000000000000000000000000000000000000815290516004828101926000929190829003018183876161da5a03f1156100025750505050565b604080519115158252519081900360200190f35b5060019056",
        "storage" : {
        }
    })";

    try
    {
        CJOptions opt     = {.autosort = true};
        spDataObject dObj = ConvertJsoncppStringToData(data, opt);
        assert(dObj->atKey("//nonce").asString() == "1");
    } catch (std::exception const& _ex)
    {
        assert(string(_ex.what()).find("Reading json with dublicate fields") != string::npos);
    }
}

void dataobject_accessArray()
{
    PRINT_FUNC_NAME();
    // Access not an array
    spDataObject obj;
    auto const& vec = obj->getSubObjects();
    assert(vec.size() == 0);
    auto const& map = obj->getSubObjectKeys();
    assert(map.size() == 0);

    auto& vecU = (*obj).getSubObjectsUnsafe();
    assert(vecU.size() == 0);
}

void dataobject_copy()
{
    PRINT_FUNC_NAME();
    string const data = R"(
    {
        "intfield" : 123,
        "string" : "somstring",
        "bool" : true,
        "null" : null,
        "array" : [
            "element1",
            "element2"
        ],
        "object" : {
            "intfield" : -123,
            "string" : "somstring",
            "bool" : true
        },
        "array2" : [
            1,
            -2
        ]
    })";
    CJOptions opt     = {.jsonParse = CJOptions::JsonParse::ALLOW_COMMENTS, .autosort = true};
    spDataObject dObj = ConvertJsoncppStringToData(data, opt);
    //assert(
    //    dev::toHexPrefixed(dev::sha3(dObj->asJson(0, false))) ==
    //    "0xfea9954b38d3afe471538b8854d63628570c1b68cf1c403e3b75a3263f7d4e3a");

    spDataObject dObj2 = dObj->copy();
    assert(dObj->asJson(0, false) == dObj2->asJson(0, false));
    (*dObj).atKeyUnsafe("string") = "newString";
    assert(dObj->asJson(0, false) != dObj2->asJson(0, false));

    (*dObj).atKeyUnsafe("string") = "somstring";
    (*dObj2).copyFrom(dObj);
    assert(dObj->asJson(0, false) == dObj2->asJson(0, false));
    (*dObj).atKeyUnsafe("string") = "newString";
    assert(dObj->asJson(0, false) != dObj2->asJson(0, false));
}

void dataobject_defaultType()
{
    PRINT_FUNC_NAME();
    spDataObject obj;
    assert(DataObject::dataTypeAsString(obj->type()) == "notinit");
}

void dataobject_removeLeadingZerosFromHexValueEVEN()
{
    PRINT_FUNC_NAME();
    spDataObject obj = sDataObject("0x0000112233");
    (*obj).setKey("0x0000112233");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x112233");
    assert(obj->getKey() == "0x112233");

    (*obj).setString("0x000112234");
    (*obj).setKey("0x000112234");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x112234");
    assert(obj->getKey() == "0x112234");

    (*obj).setString("0x00012235");
    (*obj).setKey("0x00012235");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x012235");
    assert(obj->getKey() == "0x012235");

    (*obj).setString("0x01");
    (*obj).setKey("0x01");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x01");
    assert(obj->getKey() == "0x01");

    (*obj).setString("0x11");
    (*obj).setKey("0x11");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x11");
    assert(obj->getKey() == "0x11");

    (*obj).setString("0x1");
    (*obj).setKey("0x1");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x01");
    assert(obj->getKey() == "0x01");

    (*obj).setString("0x");
    (*obj).setKey("0x");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0x");
    assert(obj->getKey() == "0x");

    (*obj).setString("0");
    (*obj).setKey("0");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "0");
    assert(obj->getKey() == "0");

    (*obj).setString("");
    (*obj).setKey("");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "");
    assert(obj->getKey() == "");

    (*obj).setString("00012235");
    (*obj).setKey("00012235");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValueEVEN);
    (*obj).performModifier(mod_removeLeadingZerosFromHexKeyEVEN);
    assert(obj->asString() == "00012235");
    assert(obj->getKey() == "00012235");
}

void dataobject_removeLeadingZerosFromHexValues()
{
    PRINT_FUNC_NAME();
    spDataObject obj = sDataObject("0x0000112233");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x112233");

    (*obj).setString("0x000112234");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x112234");

    (*obj).setString("0x00012235");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x12235");

    (*obj).setString("0x01");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x1");

    (*obj).setString("0x11");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x11");

    (*obj).setString("0x1");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x1");

    (*obj).setString("0x");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0x");

    (*obj).setString("0");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "0");

    (*obj).setString("");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "");

    (*obj).setString("00012235");
    (*obj).performModifier(mod_removeLeadingZerosFromHexValues);
    assert(obj->asString() == "00012235");
}

void dataobject_mod_valueToFH32()
{
    PRINT_FUNC_NAME();
    spDataObject obj = sDataObject("0x01");
    (*obj).performModifier(mod_valueToFH32);
    assert(obj->asString() == "0x0000000000000000000000000000000000000000000000000000000000000001");

    (*obj).setString("0x");
    (*obj).performModifier(mod_valueToFH32);
    assert(obj->asString() == "0x0000000000000000000000000000000000000000000000000000000000000000");

    (*obj).setString("0x0000000000000000000000000000000000000000000000000000000000000000");
    (*obj).performModifier(mod_valueToFH32);
    assert(obj->asString() == "0x0000000000000000000000000000000000000000000000000000000000000000");

    (*obj).setString("0x1000000000000000000000000000000000000000000000000000000000000000");
    (*obj).performModifier(mod_valueToFH32);
    assert(obj->asString() == "0x1000000000000000000000000000000000000000000000000000000000000000");

    (*obj).setString("0x0000010000000000");
    (*obj).performModifier(mod_valueToFH32);
    assert(obj->asString() == "0x0000000000000000000000000000000000000000000000000000010000000000");

    (*obj).setString("0x1000010000000000");
    (*obj).performModifier(mod_valueToFH32);
    assert(obj->asString() == "0x0000000000000000000000000000000000000000000000001000010000000000");
}

void dataobject_copyFrom_emptyArray()
{
    PRINT_FUNC_NAME(); {
        DataObject orig(DataType::Array);
        assert(orig.type() == DataType::Array);
        DataObject copy;
        copy.copyFrom(orig);
        assert(copy.type() == DataType::Array);
    } {
        DataObject orig(DataType::Object);
        assert(orig.type() == DataType::Object);
        DataObject copy;
        copy.copyFrom(orig);
        assert(copy.type() == DataType::Object);
    }
}

int main()
{
    dataobject_sort();
    dataobject_bracers();
    dataobject_EscapeChars();

    dataobject_invalidJson1();
    dataobject_invalidJson2();
    dataobject_invalidJson3();
    dataobject_invalidJson4();
    dataobject_invalidJson5();
    dataobject_invalidJson6();
    dataobject_invalidJson7();
    dataobject_invalidJson8();
    dataobject_readJson1();
    dataobject_readJson2();
    dataobject_readJson3();
    dataobject_readJson4();
    dataobject_readJson5();
    dataobject_readJson5b();
    dataobject_readJson6();
    dataobject_readJson7();
    dataobject_readJson8();
    dataobject_readJson8a();
    dataobject_readJson9();
    dataobject_readJson10();
    dataobject_readJson11();
    dataobject_readJson12();
    dataobject_readJson13();
    dataobject_readJson14();
    dataobject_readJson15();
    dataobject_readJson16();

    dataobject_readJson_doublefields();
    dataobject_readJson_doublefields_allowComments();
    dataobject_readJson_doublefields_allowComments_actualComments();

    dataobject_findOrderedKeyPosition_before1_of3();
    dataobject_findOrderedKeyPosition_before1_of8();
    dataobject_findOrderedKeyPosition_before2_of8();
    dataobject_findOrderedKeyPosition_before3_of8();
    dataobject_findOrderedKeyPosition_before4_of8();
    dataobject_findOrderedKeyPosition_before5_of8();
    dataobject_findOrderedKeyPosition_before6_of8();
    dataobject_findOrderedKeyPosition_before7_of8();
    dataobject_findOrderedKeyPosition_before8_of8();

    dataobject_findOrderedKeyPosition_before2_of3();
    dataobject_findOrderedKeyPosition_before3_of3();
    dataobject_findOrderedKeyPosition_after3_of3();
    dataobject_findOrderedKeyPosition_after6_of6();
    dataobject_findOrderedKeyPosition_after7_of7();
    dataobject_findOrderedKeyPosition_after8_of8();

    dataobject_jsonOrder();
    dataobject_jsonOrder2();
    dataobject_replace();
    dataobject_arrayhell();

    dataobject_besuresponse();
    dataobject_doublefields_comments_allowed();
    dataobject_doublefields_comments_disabled();
    dataobject_accessArray();
    dataobject_copy();
    dataobject_defaultType();

    dataobject_findOrderedKeyPosition_empty();

    dataobject_removeLeadingZerosFromHexValueEVEN();
    dataobject_removeLeadingZerosFromHexValues();
    //dataobject_valueToCompactEvenHexPrefixed();
    //dataobject_keyToCompactEvenHexPrefixed();
    dataobject_mod_valueToFH32();

    dataobject_copyFrom_emptyArray();
    return 0;
}

