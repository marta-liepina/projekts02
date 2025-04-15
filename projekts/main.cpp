#include <iostream>
#include <bits/stdc++.h>
#define pb push_back
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

enum dir {TOP=0, RIGHT=1, BOTTOM=2, LEFT=3};
int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};

Cell maze[HEIGHT][WIDTH];

bool inBounds(int x, int y) {
    return x>=0 && x<WIDTH && y>=0 && y<HEIGHT;
}

void removeWall(int x1, int y1, int x2, int y2) {
    if(x1 == x2){
        if(y1 > y2) {
            maze[y1][x1].walls[TOP] = false;
            maze[y2][x2].walls[BOTTOM] = false;
        }
        else{
            maze[y1][x1].walls[BOTTOM] = false;
            maze[y2][x2].walls[TOP] = false;
        }
    }
    else if(y1 == y2){
        if(x1 > x2){
            maze[y1][x1].walls[LEFT] = false;
            maze[y2][x2].walls[RIGHT] = false;
        }
        else{
            maze[y1][x1].walls[RIGHT] = false;
            maze[y2][x2].walls[LEFT] = false;
        }
    }
}

void generateMaze(int startX, int startY) {
    maze[startY][startX].vis = true;
    vector<pair<int, int>> frontier;
    for (int i = 0; i < 4; i++){
        int nx = startX + dx[i];
        int ny = startY + dy[i];
        if(inBounds(nx, ny))
            frontier.push_back({nx, ny});
    }
    while(!frontier.empty()) {
        int randIndex = rand() % frontier.size();
        int cx = frontier[randIndex].first;
        int cy = frontier[randIndex].second;

        vector<pair<int, int>> neighbors;
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (inBounds(nx, ny) && maze[ny][nx].vis)
                neighbors.push_back({nx, ny});
        }

        if (!neighbors.empty() && rand() % 3 != 0) {
            int randNeighbor = rand() % neighbors.size();
            int nx = neighbors[randNeighbor].first;
            int ny = neighbors[randNeighbor].second;

            removeWall(cx, cy, nx, ny);
            maze[cy][cx].vis = true;

            for (int i = 0; i < 4; i++) {
                int nx = cx + dx[i];
                int ny = cy + dy[i];
                if (inBounds(nx, ny) && !maze[ny][nx].vis)
                    frontier.pb({nx, ny});
            }
        }

        frontier.erase(frontier.begin() + randIndex);
    }
}

bool isSolvable() {
    vector<vector<bool>> visited(HEIGHT, vector<bool>(WIDTH, false));
    queue<pair<int, int>> q;
    q.push({0, 0});
    visited[0][0] = true;

    while(!q.empty()){
        auto [cx, cy] = q.front();
        q.pop();
        if(cx == WIDTH - 1 && cy == HEIGHT - 1)
            return true;
        for(int d = 0; d < 4; d++){
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if(inBounds(nx, ny) && !visited[ny][nx]){
                if(!maze[cy][cx].walls[d]){
                    visited[ny][nx] = true;
                    q.push({nx, ny});
                }
            }
        }
    }
    return false;
}

void addExtraWalls(double probability) {
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            if((x == 0 && y == 0) || (x == WIDTH - 1 && y == HEIGHT - 1))
                continue;
            if(x < WIDTH - 1 && !maze[y][x].walls[RIGHT]) {
                if(!((x == 0 && y == 0) || (x+1 == WIDTH - 1 && y == HEIGHT - 1))){
                    if((double)rand()/RAND_MAX < probability) {
                        maze[y][x].walls[RIGHT] = true;
                        maze[y][x+1].walls[LEFT] = true;
                        if(!isSolvable()){
                            maze[y][x].walls[RIGHT] = false;
                            maze[y][x+1].walls[LEFT] = false;
                        }
                    }
                }
            }
            if(y < HEIGHT - 1 && !maze[y][x].walls[BOTTOM]) {
                if(!((x == 0 && y == 0) || (x == WIDTH - 1 && y+1 == HEIGHT - 1))){
                    if((double)rand()/RAND_MAX < probability) {
                        maze[y][x].walls[BOTTOM] = true;
                        maze[y+1][x].walls[TOP] = true;
                        if(!isSolvable()){
                            maze[y][x].walls[BOTTOM] = false;
                            maze[y+1][x].walls[TOP] = false;
                        }
                    }
                }
            }
        }
    }
}

void displayMaze() {
    ofstream fout("maze.out");
    for(int x=0; x<WIDTH; x++)
        fout << "+---";
    fout << "+\n";

    for (int y = 0; y < HEIGHT; y++){
        fout << "|";
        for (int x = 0; x < WIDTH; x++){
            if(y == 0 && x == 0)
                fout << " S ";
            else if(y == HEIGHT-1 && x == WIDTH-1)
                fout << " E ";
            else
                fout << "   ";
            fout << (maze[y][x].walls[RIGHT] ? "|" : " ");
        }
        fout << "\n";
        for (int x = 0; x < WIDTH; x++){
            fout << (maze[y][x].walls[BOTTOM] ? "+---" : "+   ");
        }
        fout << "+\n";
    }
}

int main() {
    srand(time(0));
    generateMaze(0, 0);
    addExtraWalls(0.3);
    displayMaze();
    return 0;
}
