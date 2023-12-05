#include <iostream>
#include <vector>
using namespace std;

char cmd;
int index;
vector<string> records;
string buf;

void saveCsv(){
    
}


int main(){

    cout << "Welcome to myRecorder ;)\n";
    while(cmd != 'q'){
        printf("\n------------------------------\nc: add new record\nr: read all records\nu: update exist record\nd: delete exist records\ns: save records as csv\npress 'q' to quit!\n>>");
        cin >> cmd;
        switch (cmd){
        case 'c':
        case 'C':
        cout << "type the record: ";
        cin >> buf;
        records.push_back(buf);
        cout << "new data inserted! : " << records.back() << endl;
        break;

        case 'r':
        case 'R':
        if(records.empty()) {
            cout << "no record exist!\n";
            break;
        }
        else cout << records.size() << " records found!\n";
        for(int i = 0; i < records.size(); i++){
            cout << i+1 << " : " << records.at(i) << endl;
        }
        break;

        case 'u':
        case 'U':
        cout << "select number of record to update: ";
        cin >> index;
        if(index < 1 || index > records.size()){
            cout << "the records does not exist :(\n";
            break;
        }
        index --;
        cout << "insert the new record: ";
        cin >> buf;
        records.at(index) = buf;
        cout << "succesfuly updated :)\n";
        break;

        case 'd':
        case 'D':
        cout << "select number of record to delete: ";
        cin >> index;
        if(index < 1 || index > records.size()){
            cout << "the records does not exist :(\n";
            break;
        }
        index --;
        records.erase(records.begin() + index);
        cout << "succesfuly deleted :)\n";
        break;

        case 's':
        case 'S':
        saveCsv();

        case 'q':
        case 'Q':
        cout << "See you :D\n\n";

        default:
            cout << "Invalid cmd: " << cmd << endl;
            break;
        }
    }
    return 0;
    
}