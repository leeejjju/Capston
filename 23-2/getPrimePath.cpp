#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>
using namespace std;
#define BUF_SIZE 512
// #define DEBUG

/*
*   CapstonLab assignment
*   program to get Prime path
*   22000216 BRNa, 22100579 JJLee
*/


void printNodeEdges(vector<int> adj);
void addEdges();
void loadData(); //need to customize
void printAllNode();
void findPP(); //find prime path
void find_cycle(int root);
void find_paths(vector<int> st, vector<int> ed);

void printPPs();

vector<vector<int>> adj_list; //edges per node(index)
vector<int> is_visited; //visit state per node(index)
vector<vector<int>> start_point;
vector<vector<int>> end_point;
vector<vector<int>> PP; //TODO Prime paths -> is it needed?
int num_node = 0;

int main(){
    
    cout << "[enter number of node]\n>> ";
    cin >> num_node;
    adj_list.push_back(is_visited); //for the index matching...
    is_visited.push_back(0);

    addEdges(); 
    printAllNode();

    findPP();
    printPPs();
}

void printPP(vector<int> tmp){
    printf("(");
    for(int i = 0; i < tmp.size(); i++) printf("%d ", tmp.at(i));
    printf(")\n");
}

void printNodeEdges(vector<int> adj){
    for(int i = 0; i < adj.size(); i++) printf(" -> %d ", adj.at(i));
    printf(" -> NIL\n");
}

void printAllNode(){
    cout << "=================Node List=================\n";
    for(int i = 1; i <= num_node; i++){
        printf("[%2d]", i);
        printNodeEdges(adj_list.at(i));
    }
    cout << "===========================================\n";
}

void addEdges(){
    char cmd[BUF_SIZE];
    int nd = 0, adj_nd = 0;
    vector<int> start(num_node+1);
    vector<int> end(num_node+1);

    printf("[ Add edges ] (type \"quit\" to finish)\n>> ");
    fflush(stdin);
    fgets(cmd, BUF_SIZE, stdin);

    for(int i = 1; i <= num_node; i++){
        vector<int> node;
        adj_list.push_back(node);
        is_visited.push_back(0);
        start.at(i) = 0;
        end.at(i) = 0;
    }

    while(strcmp(cmd,"quit\n")){
        sscanf(cmd, "%d %d\n", &nd, &adj_nd);
        if(nd > num_node || nd < 1 || adj_nd > num_node || adj_nd < 1){
            printf("invalid node id: %d %d\n", nd, adj_nd);
        }else{
            adj_list.at(nd).push_back(adj_nd);
            start.at(nd)++;
            end.at(adj_nd)++;
            #ifdef DEBUG
            printf("new edge (%d,%d) added\n", nd, adj_nd);
            #endif
        }
        printf(">> ");
        fgets(cmd, BUF_SIZE, stdin);
    }

    for(int i = 1; i <= num_node; i++){
        if(start.at(i) == 0) {
            vector<int> ed = {i};
            end_point.push_back(ed);
        }
        if(end.at(i) == 0) {
            vector<int> st = {i};
            start_point.push_back(st);
        }
    }

    printf("done adding edges.\n\n");
    printf("start point: ");
    for(int i = 0; i < start_point.size(); i++) printf("%d ", start_point.at(i).back());
    printf("\nend point: ");
    for(int i = 0; i < end_point.size(); i++) printf("%d ", end_point.at(i).front());
    printf("\n");

}

// void findPP(){
    
//     PP.clear();
//     //1'st path
//     for(int i = 1; i <= num_node; i++){
//         vector<int> tmp = {i};
//         PP.push_back(tmp);
//         #ifdef DEBUG
//         printf("1'st new PP added... (%d)\n",i);
//         #endif
//     }
    
//     for(int k = 0; k < PP.size(); k++){
//         //find all ((j-1)'th path that tail's adj is not NIL) among current PPs and
//         //add all adj added path and remove origin from PPs
//         if(adj_list[PP.at(k).back()].size() > 0){
//             #ifdef DEBUG
//             printf("    start from path ");
//             printPP(PP.at(k));
//             #endif

//             bool added = false; //init flag

//             vector<int> origin; 
//             vector<int> adjs = adj_list[PP.at(k).back()]; //get adj ids
//             #ifdef DEBUG
//             printf("        adj list: ");
//             printPP(adjs);
//             #endif

//             origin = PP.at(k); //copy origin
//             for(int q = 0; q < adjs.size(); q++){ //add (origin + each adj) paths to PPS
//                 //only if there's no cycle
//                 if(find(origin.begin()+1, origin.end(), adjs[q]) == origin.end()){ 
//                     if((origin.size() > 1 && origin.front() != origin.back()) || origin.size() == 1){
//                         vector<int> newPP = origin;
//                         newPP.push_back(adjs[q]);
//                         PP.push_back(newPP);
//                         #ifdef DEBUG
//                         printf("        new PP added: ");
//                         printPP(newPP);
//                         #endif
//                         added = true;
//                     }
//                 }
//             }
            
//             if(added) {
//                 #ifdef DEBUG
//                 printf("        removed: ");
//                 printPP(PP.at(k));
//                 #endif
//                 PP.erase(PP.begin() + k); //remove origin
//                 k--; //go back once because current index is removed
//             }
            
//         }else if(PP.at(k).size() == 1) {
//             #ifdef DEBUG
//             printf("    removed: ");
//             printPP(PP.at(k));
//             #endif
//             PP.erase(PP.begin() + k); //remove origin
//             k--; //go back once because current index is removed
//         }
//     }
// }

/*How To??

각 노드에서 DFS를 해서 cycle을 찾아내고 cycle path를 저장해
    이 때 cycle의 in/out node를 판단해서 시작점 뭉탱이와 끝점 뭉탱이를 준비해... (visit표시 어케하지)

각 시작점들에서 모든 끝점들로의 path를 DFS로 찾아서 출력해

*/
void findPP(){

    for(int i = 1; i <= num_node; i++){
        find_cycle(i);
    }

    for(int i = 0; i < start_point.size(); i++){
        for(int j = 0; j < end_point.size(); j++){
            find_paths(start_point.at(i), end_point.at(j));
        }
    }

}

void DFS(int root){
    is_visited[root] = 1;

}

//root부터 DFS하다가 cycle 발견하면 출력하고 start, end point 저장
void find_cycle(int root){
    is_visited.clear();
    DFS(root);

}

//st.back()부터 en.front()까지 가는 모든 경로 DFS로 찾고 st, ed의 풀버전과 함께 출력
void find_paths(vector<int> st, vector<int> ed){

    for(int i = 0; i < st.size(); i++) printf("%d ", st.at(i));
    //DFS path print
    for(int i = 0; i < ed.size(); i++) printf("%d ", ed.at(i));

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

