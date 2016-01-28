#include <iostream>
#include <cfgreader.h>

// The executable reads the command line argument and prints out the corresponding data in the json file

using namespace std;
int main(int argc, char **argv){
    if (argc == 1){
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <tag>" << std::endl;
        exit(1);
    }
    string tag(argv[1]);
    cfgreader cfg("analysis.cfg");
    if (cfg.root[tag].isArray()){
      for (int i = 0; i < cfg.root[tag].size(); i++){
        cout << cfg.root[tag][i].asString() << " ";
      }
      cout << endl;
    }else{
      cout << cfg.root[tag].asString() << endl;
    }
    
}