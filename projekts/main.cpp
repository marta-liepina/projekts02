#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <utility>
using namespace std;

const int WIDTH = 30;
const int HEIGHT = 30;

struct Cell {
    bool vis = false;
    bool walls[4] = {true, true, true, true};
};

enum dir { TOP = 0, RIGHT = 1, BOTTOM = 2, LEFT = 3 };
int dx[] = { 0, 1, 0, -1 };
int dy[] = { -1, 0, 1, 0 };

Cell maze[HEIGHT][WIDTH];
bool isTreeEdge[HEIGHT][WIDTH][4]={};

bool inBounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

void removeWall(int x1, int y1, int x2, int y2) {
    if(x1==x2){
        if(y1>y2){
            maze[y1][x1].walls[TOP] = false;
            maze[y2][x2].walls[BOTTOM] = false;
            isTreeEdge[y1][x1][TOP] = true;
            isTreeEdge[y2][x2][BOTTOM] = true;
        }
        else{
            maze[y1][x1].walls[BOTTOM] = false;
            maze[y2][x2].walls[TOP] = false;
            isTreeEdge[y1][x1][BOTTOM] = true;
            isTreeEdge[y2][x2][TOP] = true;
        }
    } else if (y1 == y2) {
        if (x1 > x2) {
            maze[y1][x1].walls[LEFT] = false;
            maze[y2][x2].walls[RIGHT] = false;
            isTreeEdge[y1][x1][LEFT] = true;
            isTreeEdge[y2][x2][RIGHT] = true;
        } else {
            maze[y1][x1].walls[RIGHT] = false;
            maze[y2][x2].walls[LEFT] = false;
            isTreeEdge[y1][x1][RIGHT] = true;
            isTreeEdge[y2][x2][LEFT] = true;
        }
    }
}

void generateMaze(int startX, int startY){
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++) {
            maze[y][x].vis = false;
            for(int d=0;d<4;d++)
                maze[y][x].walls[d] = true;
        }
    }
    memset(isTreeEdge, 0, sizeof(isTreeEdge));

    maze[startY][startX].vis = true;
    vector<pair<int,int>> frontier;
    frontier.reserve(WIDTH * HEIGHT);
    for(int i=0; i<4; i++){
        int nx = startX + dx[i], ny = startY + dy[i];
        if(inBounds(nx, ny))
            frontier.emplace_back(nx, ny);
    }

    while(!frontier.empty()){
        int randIndex = rand() % frontier.size();
        auto[cx, cy] = frontier[randIndex];

        vector<pair<int,int>> neighbors;
        neighbors.reserve(4);
        for(int i=0; i<4; i++){
            int nx = cx + dx[i], ny = cy + dy[i];
            if(inBounds(nx, ny) && maze[ny][nx].vis) {
                neighbors.emplace_back(nx, ny);
            }
        }

        if(!neighbors.empty() && rand() % 3 != 0) {
            auto [nx, ny] = neighbors[rand() % neighbors.size()];
            removeWall(cx, cy, nx, ny);
            maze[cy][cx].vis = true;
            for(int i=0; i<4; i++){
                int nnx = cx + dx[i], nny = cy + dy[i];
                if(inBounds(nnx, nny) && !maze[nny][nnx].vis)
                    frontier.emplace_back(nnx, nny);
            }
        }

        frontier[randIndex] = frontier.back();
        frontier.pop_back();
    }
}

void addExtraWalls(double probability){
    for(int y=0; y<HEIGHT; y++){
        for(int x=0; x<WIDTH; x++){
            if((x==0 && y==0) || (x==WIDTH-1 && y==HEIGHT-1))
                continue;
            if(x<WIDTH-1 && !maze[y][x].walls[RIGHT] && !isTreeEdge[y][x][RIGHT]) {
                if((double)rand()/RAND_MAX < probability) {
                    maze[y][x].walls[RIGHT] = true;
                    maze[y][x+1].walls[LEFT] = true;
                }
            }
            if (y<HEIGHT-1 && !maze[y][x].walls[BOTTOM] && !isTreeEdge[y][x][BOTTOM]) {
                if((double)rand()/RAND_MAX < probability) {
                    maze[y][x].walls[BOTTOM] = true;
                    maze[y+1][x].walls[TOP] = true;
                }
            }
        }
    }
}

bool isSolvable() {
    vector<vector<bool>>visited(HEIGHT, vector<bool>(WIDTH, false));
    queue<pair<int,int>> q;
    q.push({0,0});
    visited[0][0] = true;
    while(!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();
        if(cx==WIDTH-1 && cy==HEIGHT-1)
            return true;
        for(int d=0; d<4; d++){
            int nx = cx + dx[d], ny = cy + dy[d];
            if(inBounds(nx, ny) && !visited[ny][nx] && !maze[cy][cx].walls[d]) {
                visited[ny][nx] = true;
                q.push({nx, ny});
            }
        }
    }
    return false;
}

void writeMaze() {
    ofstream fout("maze.out");
    for(int x=0; x<WIDTH; x++)
        fout << "+---";
    fout << "+\n";
    for(int y=0; y<HEIGHT; y++){
        fout<<"|";
        for(int x=0; x<WIDTH; x++){
            if(y==0 && x==0)
                fout << " S ";
            else if(y==HEIGHT-1 && x==WIDTH-1)
                fout << " B ";
            else
                fout << "   ";
            fout << (maze[y][x].walls[RIGHT] ? "|" : " ");

        }
        fout << "\n";
        for(int x=0; x<WIDTH; x++)
            fout << (maze[y][x].walls[BOTTOM]? "+---" : "+   ");
        fout << "+\n";
    }
    fout.close();
}

struct Parent{
    int px, py;
    Parent(int _px=-1,int _py=-1) : px(_px),py(_py){}
};

vector<pair<int,int>> solveMaze() {
    vector<vector<bool>> visited(HEIGHT, vector<bool>(WIDTH,false));
    vector<vector<Parent>> parent(HEIGHT, vector<Parent>(WIDTH));
    queue<pair<int,int>> q;
    q.emplace(0,0);
    visited[0][0]=true;
    bool found=false;
    while(!q.empty()){
        auto [cx,cy]=q.front();
        q.pop();
        if(cx==WIDTH-1 && cy==HEIGHT-1){
            found=true;
            break;
        }
        for(int d=0; d<4; d++){
            int nx=cx+dx[d], ny=cy+dy[d];
            if(inBounds(nx,ny)&&!visited[ny][nx]&&!maze[cy][cx].walls[d]){
                visited[ny][nx]=true;
                parent[ny][nx]=Parent(cx,cy);
                q.emplace(nx,ny);
            }
        }
    }
    vector<pair<int,int>> path;
    if(found){
        int cx=WIDTH-1, cy=HEIGHT-1;
        while(!(cx==0&&cy==0)){
            path.emplace_back(cx,cy);
            auto par=parent[cy][cx]; cx=par.px; cy=par.py;
        }
        path.emplace_back(0,0);
        reverse(path.begin(), path.end());
    }
    return path;
}

void writeMazeSolution(const vector<pair<int,int>>& sol){
    vector<vector<bool>> onPath(HEIGHT, vector<bool>(WIDTH,false));
    for(auto &p: sol)
        onPath[p.second][p.first]=true;

    ofstream fout("maze_solution.out");
    for(int x=0; x<WIDTH; x++)
        fout << "+---";
    fout << "+\n";
    for(int y=0; y<HEIGHT; y++){
        fout<<"|";
        for(int x=0;x<WIDTH;x++){
            if(y==0 && x==0)
                fout << " S ";
            else if(y==HEIGHT-1 && x==WIDTH-1)
                fout << " B ";
            else if(onPath[y][x])
                fout << " * ";
            else
                fout << "   ";
            fout << (maze[y][x].walls[RIGHT] ? "|" : " ");

        }
        fout << "\n";
        for(int x=0; x<WIDTH; x++)
            fout<<(maze[y][x].walls[BOTTOM]? "+---" : "+   ");
        fout<<"+\n";
    }
    fout.close();
}

int main(){
    srand(time(0));
    generateMaze(0,0);
    addExtraWalls(0.3);
    writeMaze();
    auto sol = solveMaze();
    writeMazeSolution(sol);
    return 0;
}
