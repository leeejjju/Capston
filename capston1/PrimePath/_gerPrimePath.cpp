#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>
using namespace std;
#define BUF_SIZE 512
#define DEBUG

// #define OUR
#define CASE1
// #define CASE2

/*
*   CapstonLab assignment
*   program to get Prime path
*   22000216 BRNa, 22100579 JJLee
*/

class node{
    public:
    int id; //node id
    node* adj; //adjacen
    node(int init_id);
    void add_edge(int id);
    void printNodeEdges();
};

void addEdges();
void loadData(); //need to customize
void printAllNode();
void findPP(); //find prime path
void printPPs();
vector<int> getAdjs(vector<int> vec, node* nd);

vector<node*> adj_list;
vector<vector<int>> PP; //Prime paths
int num_node = 0;

int main(){
    
    cout << "[enter number of node]\n>> ";
    cin >> num_node;
    adj_list.push_back(NULL); //for the index matching...

    addEdges(); 
    printAllNode();

    findPP();
    printPPs();
}

void printPP(vector<int> tmp){
    printf("(");
    for(int i = 0; i < tmp.size(); i++){
        printf("%d ", tmp.at(i));
    }
    printf(")\n");
}

node::node(int init_id){
    id = init_id;
    adj = NULL;
}

void node::add_edge(int id){
    node* adj_node = new node(id);
    if(adj == NULL) adj = adj_node;
    else adj->add_edge(id);
}

void node::printNodeEdges(){
    if(adj == NULL) cout << " -> NIL\n";
    else {
        cout << " -> " << adj->id;
        adj->printNodeEdges();
    }
}

void printAllNode(){
    cout << "=================Node List=================\n";
    for(int i = 1; i <= num_node; i++){
        printf("[%2d]", adj_list.at(i)->id);
        adj_list.at(i)->printNodeEdges();
    }
    cout << "===========================================\n";
}

void addEdges(){
    char cmd[BUF_SIZE];
    int nd = 0, adj_nd = 0;
    
    printf("[ Add edges ] (type \"quit\" to finish, type \"load\" to load data)\n>> ");
    fflush(stdin);
    fgets(cmd, BUF_SIZE, stdin);

    if(!strcmp(cmd,"load\n")){
        loadData();
    }else{
        for(int i = 1; i <= num_node; i++){
            node* newNode = new node(i);
            adj_list.push_back(newNode);
        }
        while(strcmp(cmd,"quit\n")){
            sscanf(cmd, "%d %d\n", &nd, &adj_nd);
            if(nd > num_node || nd < 1 || adj_nd > num_node || adj_nd < 1){
                printf("invalid node id\n");
            }else{
                adj_list.at(nd)->add_edge(adj_nd);
                printf("new edge (%d,%d) added\n", nd, adj_nd);
            }
            printf(">> ");
            fgets(cmd, BUF_SIZE, stdin);
        }
    }printf("done adding edges\n\n");
}

//get adj node id listb
vector<int> getAdjs(vector<int> vec, node* nd){
    
    if(nd == NULL) return vec;
    else{
        vec.push_back(nd->id);
        return getAdjs(vec, nd->adj);
    }
}

void findPP(){
    vector<vector<int>> PPs; //vector of Prime pathes starting each node i
    for(int i = 1; i <= num_node; i++){
        #ifdef DEBUG
        printf("start to find PPs starting with node %d...\n", i);
        #endif
        PPs.clear();

        //1'st path
        vector<int> tmp = {i};
        PPs.push_back(tmp);
        printf("        node %d's 1'st new PP added... (%d)\n", i, i);
        for(int j = 2; j <= num_node; j++){ //find j'th simple pathes starting with node i
            for(int k = 0; k < PPs.size(); k++){
                //find all ((j-1)'th path that tail's adj is not NIL) among current PPs and
                //add all adj added path and remove origin from PPs
                if((PPs.at(k).size() == (j-1)) && (adj_list[PPs.at(k).back()]->adj != NULL)){
                    #ifdef DEBUG
                    printf("    [%d PPs] start with %d'th path ", i, j-1);
                    printPP(PPs.at(k));
                    #endif
                    vector<int> origin; 
                    vector<int> adjs = getAdjs(origin, adj_list[PPs.at(k).back()]->adj); //get adj ids
                    origin = PPs.at(k); //copy origin (j-1)'th path
                    printf("    adj list: ");
                    printPP(adjs);
                    bool added = false;
                    for(int q = 0; q < adjs.size(); q++){ //add (origin + each adj) paths to PPS
                        //only if there's no cycle
                        if(find(origin.begin()+1, origin.end(), adjs[q]) == origin.end()){ 
                            if((origin.size() > 1 && origin.front() != origin.back()) || origin.size() == 1){
                                vector<int> newPP = origin;
                                newPP.push_back(adjs[q]);
                                PPs.push_back(newPP);
                                printf("        node %d's %d'th new PP added... ", i, j);
                                printPP(newPP);
                                added = true;
                            }
                        }
                    }
                    
                    if(added) {
                        printf("        now dont wanna using ");
                        printPP(PPs.at(k));
                        PPs.erase(PPs.begin() + k); //remove origin
                        k--; //go back once because current index is removed
                    }
                    
                }
            }
        }
        PP.insert(PP.end(), PPs.begin(), PPs.end()); //add PPs of node i to PP
    }
}

void printPPs(){
    cout << "=================Prime Path================\n";
    for(int i = 0; i < PP.size(); i++){
        printf("[");
        for(int j = 0; j < PP.at(i).size(); j++) printf("%d ", PP.at(i).at(j));
        printf("]\n");
    }
    cout << "==========================================\n";
}

#ifdef OUR
void loadData(){
    num_node = 37;
    for(int i = 1; i <= num_node; i++){
        node* newNode = new node(i);
        adj_list.push_back(newNode);
    }
    adj_list.at(1)->add_edge(2);
    adj_list.at(2)->add_edge(3);
    adj_list.at(2)->add_edge(4);
    adj_list.at(2)->add_edge(5);
    adj_list.at(3)->add_edge(6);
    adj_list.at(3)->add_edge(7);
    adj_list.at(4)->add_edge(8);
    adj_list.at(5)->add_edge(9);
    adj_list.at(5)->add_edge(10);
    adj_list.at(6)->add_edge(11);
    adj_list.at(7)->add_edge(12);
    adj_list.at(7)->add_edge(13);
    adj_list.at(8)->add_edge(14);
    adj_list.at(8)->add_edge(15);
    adj_list.at(9)->add_edge(8);
    adj_list.at(10)->add_edge(9);
    adj_list.at(10)->add_edge(16);
    adj_list.at(12)->add_edge(11);
    adj_list.at(13)->add_edge(17);
    adj_list.at(14)->add_edge(3);
    adj_list.at(15)->add_edge(3);
    adj_list.at(16)->add_edge(9);
    adj_list.at(16)->add_edge(18);
    adj_list.at(17)->add_edge(19);
    adj_list.at(17)->add_edge(20);
    adj_list.at(18)->add_edge(8);
    adj_list.at(19)->add_edge(21);
    adj_list.at(19)->add_edge(22);
    adj_list.at(20)->add_edge(23);
    adj_list.at(20)->add_edge(24);
    adj_list.at(21)->add_edge(25);
    adj_list.at(22)->add_edge(26);
    adj_list.at(23)->add_edge(27);
    adj_list.at(24)->add_edge(17);
    adj_list.at(25)->add_edge(11);
    adj_list.at(26)->add_edge(28);
    adj_list.at(26)->add_edge(29);
    adj_list.at(27)->add_edge(30);
    adj_list.at(27)->add_edge(31);
    adj_list.at(28)->add_edge(32);
    adj_list.at(29)->add_edge(26);
    adj_list.at(30)->add_edge(24);
    adj_list.at(31)->add_edge(33);
    adj_list.at(31)->add_edge(34);
    adj_list.at(32)->add_edge(35);
    adj_list.at(32)->add_edge(36);
    adj_list.at(33)->add_edge(37);
    adj_list.at(34)->add_edge(37);
    adj_list.at(35)->add_edge(25);
    adj_list.at(36)->add_edge(32);
    adj_list.at(37)->add_edge(27);
}
#endif

#ifdef CASE1
void loadData(){
    num_node = 5;
    for(int i = 1; i <= num_node; i++){
        node* newNode = new node(i);
        adj_list.push_back(newNode);
    }
    adj_list.at(1)->add_edge(2);
    adj_list.at(2)->add_edge(3);
    adj_list.at(2)->add_edge(4);
    adj_list.at(4)->add_edge(5);
    adj_list.at(5)->add_edge(2);
}
#endif

#ifdef CASE2
void loadData(){
    num_node = 7;
    for(int i = 1; i <= num_node; i++){
        node* newNode = new node(i);
        adj_list.push_back(newNode);
    }
    adj_list.at(1)->add_edge(2);
    adj_list.at(1)->add_edge(5);
    adj_list.at(2)->add_edge(3);
    adj_list.at(2)->add_edge(6);
    adj_list.at(3)->add_edge(4);
    adj_list.at(4)->add_edge(2);
    adj_list.at(5)->add_edge(5);
    adj_list.at(5)->add_edge(7);
    adj_list.at(6)->add_edge(7);
}
#endif
