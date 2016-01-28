// cheetsheet and test file for jsoncpp
// jsoncpp classs reference:
// http://jsoncpp.sourceforge.net/class_json_1_1_value.html

#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>

void test_write(){
    Json::Value root;   // will contain the root value after parsing
    root["Dataset"] ="ttbar";
    root["isData"] = false;
    root["Events"] = 10000;
    root["Selection"]["Cut 1"] = 2;
    root["Selection"]["Cut 2"] = 7;
    Json::StyledWriter writer;
    std::string outputConfig = writer.write( root );
    std::ofstream out("test.json");
    out << outputConfig << std::endl;
    out.close();
    std::cout << "wrote test.json\n";
}

void test_read(){
    Json::Value root;   // will contain the root value after parsing
    Json::Reader reader;
    std::string line, inputConfig;
    std::ifstream myfile ("test.json");
    if (myfile.is_open()){
        while ( getline (myfile,line) ){ inputConfig += line; }
        myfile.close();
    } else {
      std::cerr << "Unable to open file test.json";
      return;
    }
    
    bool parsingSuccessful = reader.parse( inputConfig, root );
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        std::cerr  << "Failed to parse configuration\n"
                   << reader.getFormattedErrorMessages();
        return;
    }
    std::cout << "Dataset: " << root["Dataset"]
              << "Cut 1  : " << root["Selection"]["Cut 1"] << std::endl;
    std::cout << root["Selection"]["Cut 1"].asInt() + 10 << "\n";
    
    /*if (debug) cout << "json file contains pointers to " << root["rootfiles"].size() << " ROOT files" << endl;
    for (int i = 0; i<root["rootfiles"].size(); i++) {
        stemp=root["path"].asString() + "/" + root["rootfiles"][i].asString();
        cout << "adding to chain: " << stemp << endl;
        mychain->Add(stemp);
    }*/
    
    
}

int main(){
    //test_write();
    test_read();
    return 0;
}