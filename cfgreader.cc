#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <json/json.h>

// Helper class to access the global analysis configuration.
// Reads the json configuration file and puts the values into public variables

using namespace std;

class cfgreader {
  public:
    cfgreader (string);
    std::vector<string> datasets;
    std::vector<string> categories;
    string json_path;
    Json::Value root;
};

cfgreader::cfgreader (string cfgfile){
    bool debug=false;
       // will contains the root value after parsing
    Json::Reader reader;
    std::string line, inputConfig;
    
    // read global configuration
    if (debug) cout << "Reading global configuration " << cfgfile << " : ";
    std::ifstream myfile (cfgfile);
    if (myfile.is_open()){
        while ( getline (myfile,line) ){ inputConfig += line; }
        myfile.close();
    } else {
      std::cerr << "Unable to open file test.json";
      exit(1);
    }
    
    if (debug) cout << "parsing json : ";
    bool parsingSuccessful = reader.parse( inputConfig, root );
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        std::cerr  << "Failed to parse configuration\n"
                   << reader.getFormattedErrorMessages();
        exit(1);
    }
    if (debug) cout << "success.\n";
    
    // read the configuration
    //vector<string> datasets;
    //vector<string> categories;
    json_path=root["directories"]["json"].asString();
    for (int i = 0; i<root["datasets"].size(); i++) {
        datasets.push_back(root["datasets"][i].asString());
    }
    for (int i = 0; i<root["categories"].size(); i++) {
        categories.push_back(root["categories"][i].asString());
    }
    if (debug) std::cout << "number of Datasets: " << datasets.size() << endl;
    if (debug) std::cout << "number of categories: " << categories.size() << endl;
    if (debug) std::cout << "json path: " << json_path << endl;    
}
