/**
 *    author:  Yash Joshi
 *    created: 04.04.2023 10:10:33       
**/

#include <bits/stdc++.h>

using namespace std;


struct Node {
    int x;
    int y;

    Node() {
        x = 0;
        y = 0;
    }

    Node(int xx, int yy) {
        x = xx;
        y = yy;
    }
};

int leaderOfNode(Node node);

int anchorOfNode(Node node);

void broadcastGlobalLeader(int leader);

int layers = 0;

class Grid {
    int rows;
    int cols;

public:
    vector<vector<int>> cells;
    vector<vector<int>> robotCount;

    Grid(int rows, int cols) {
        this->rows = rows;
        this->cols = cols;

        cells.resize(rows);
        for (int i = 0; i < rows; i++)
            cells[i].resize(cols, 0);

        robotCount.resize(rows);
        for (int i = 0; i < rows; i++)
            robotCount[i].resize(cols, 0);
    }

    /* Checking the cordinates are valid or not */
    bool isValid(Node node) {
        int i = node.x, j = node.y;
        if (i < 0 || j < 0 || i >= rows || j >= cols)
            return false;
        else return true;
    }

    /* Number of Ports for the given Node */
    int portCount(Node node) {
        vector<Node> portMap = getPortMap(node);
        return portMap.size();
    }

    vector<Node> getPortMap(Node node) {
        vector<Node> portMap;

        /* 6 possible directions for node */
        vector<vector<int>> dirs = {
                {-2, 0},
                {-1, 1},
                {1,  1},
                {2,  0},
                {1,  -1},
                {-1, -1},
        };

        for (auto dir: dirs) {
            Node tempNode(node.x + dir[0], node.y + dir[1]);
            if (isValid(tempNode)) {
                portMap.push_back(tempNode);
            }
        }

        return portMap;
    }

    /* Checking node is corner node or not*/
    bool isCorner(Node node) {
        if (portCount(node) == 2) return true;
        else if (portCount(node) == 3) {
            vector<Node> portMap = getPortMap(node);
            for (Node i: portMap) {
                if (portCount(i) == 2 || portCount(i) == 4)
                    return true;
            }
        }
        return false;
    }

    void setRobotCount(Node node, int val) {
        robotCount[node.x][node.y] = val;
    }


    int getAnchor(Node node) {


        return 0;
    }

    int getLeader(Node node) {

        return leaderOfNode(node);
    }

    int getRobotCount(Node node) {
        return robotCount[node.x][node.y];
    }

    ~Grid() {}
};

class Robot {
private:
    int robotID;

    int portEntered = 0;
    Grid *grid;
    bool leader = false;
    bool anchor = false;


public:
    int globalLeader = -1;
    Node currNode;
    Node prevNode;

    Robot() {}

    void setPosition(int x, int y) {
        currNode.x = x;
        currNode.y = y;
    }

    /* Set pointer to grid to use Grid methods */
    void setGrid(Grid *pGrid) {
        this->grid = pGrid;
    }

    void setId(int id) {
        this->robotID = id;
    }

    bool isSameNode(Node a, Node b) {
        return (a.x == b.x) && (a.y == b.y);
    }

    void setPortEntered(vector<Node> &portMap, Node node) {
        for (int index = 0; index < portMap.size(); index++) {
            if (isSameNode(prevNode, portMap[index])) {
                portEntered = index;
                return;
            }
        }
    }

    void printPos(Node node) {
        cout << node.x << " : " << node.y << endl;
    }

    void exitToPort(vector<Node> &portMap, Node node) {
        prevNode = currNode;
        currNode = node;
        portMap = grid->getPortMap(currNode);

        //printPos(currNode);

        grid->setRobotCount(prevNode, grid->getRobotCount(prevNode) - 1);
        grid->setRobotCount(currNode, grid->getRobotCount(currNode) + 1);

    }

    /* Stage 1*/
    void moveToBoundary() {
//          cout << robotID << " : stage 1 starts..." << endl;
        /* Choose random port and exit through that port*/
        int random = 1;
        vector<Node> portMap = grid->getPortMap(currNode);

        exitToPort(portMap, portMap[(random) % portMap.size()]);

        setPortEntered(portMap, prevNode);
//        cout << "First Pos: ";
//        printPos(currNode);
        /* Exit through 3rd Node from portEnterd */
        while (grid->portCount(currNode) == 6) {
            exitToPort(portMap, portMap[(portEntered + 3) % portMap.size()]);
            setPortEntered(portMap, prevNode);
//            printPos(currNode);
        }

        /* Print Position */

    }

    /* Stage 2*/
    void moveToAnyCorner() {
        while (!grid->isCorner(currNode)) {
            vector<Node> portMap = grid->getPortMap(currNode);
            if (grid->portCount(currNode) == 3) {
                for (Node node: portMap) {
                    if (((grid->portCount(node) == 3) || (grid->portCount(node) == 5))
                        && (!isSameNode(node, portMap[portEntered]))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
            }
            if (grid->portCount(currNode) == 4) {
                for (Node node: portMap) {
                    if (((grid->portCount(node) == 2) || (grid->portCount(node) == 3) || (grid->portCount(node) == 4))
                        && (!isSameNode(node, portMap[portEntered]))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
            }
            if (grid->portCount(currNode) == 5) {
                for (Node node: portMap) {
                    if (((grid->portCount(node) == 2) || (grid->portCount(node) == 3))
                        && (!isSameNode(node, portMap[portEntered]))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
            }
        }
    }

    /* Stage 3*/
    void moveToOneCorner() {
        int myLeader = leaderOfNode(currNode);

        if (myLeader == robotID)
            leader = true;

        int myAnchor = anchorOfNode(currNode);

        int currAnchor = -1;
        int currLeader = myLeader;


        if (myAnchor == robotID)
            anchor = true;

        if (anchor && leader)
            leader = false;


        if (leader) {
            int tempLeader = myLeader;
            while (currAnchor != myAnchor) {
                //go to intial first node near to current corner
                vector<Node> portMap = grid->getPortMap(currNode);
                for (Node node: portMap) {
                    if (((grid->portCount(node) == 3) || (grid->portCount(node) == 5) || (grid->portCount(node) == 4))
                        && (!isSameNode(node, portMap[portEntered]))) {
                        // cout<<"Initial change : ";
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
                moveToAnyCorner();
                // cout<<"-------------"<<endl;
                // collect information
//                cout<<"Corner: " ; printPos(currNode);
//                printPos(prevNode);
                tempLeader = min(tempLeader, leaderOfNode(currNode));
                currAnchor = anchorOfNode(currNode);
            }
            broadcastGlobalLeader(tempLeader);
        }


        currLeader = leaderOfNode(currNode);

        while (currLeader != globalLeader) {
            //go to intial first node near to current corner
            vector<Node> portMap = grid->getPortMap(currNode);
            for (Node node: portMap) {
                if (((grid->portCount(node) == 3) || (grid->portCount(node) == 5) || (grid->portCount(node) == 4))
                    && (!isSameNode(node, portMap[portEntered]))) {
                    // cout<<"Initial change : ";
                    exitToPort(portMap, node);
                    setPortEntered(portMap, prevNode);
                    break;
                }
            }

            moveToAnyCorner();
            currLeader = leaderOfNode(currNode);
        }

    }

    /* Stage 4*/
    void distributeOverBoundary() {
        int p = ceil(layers / 2);
        int q = layers - p;

        if (grid->portCount(currNode) == 3)
            swap(p, q);

        bool turn = 1;

        int robotCount = grid->getRobotCount(currNode);

        int currentLot = p;
        vector<Node> portMap = grid->getPortMap(currNode);

        if (robotID >= currentLot) {
            for (Node node: portMap) {
                if (((grid->portCount(node) == 3) || (grid->portCount(node) == 5))) {
                    exitToPort(portMap, node);
                    setPortEntered(portMap, prevNode);
                    break;
                }
            }
        }
        currentLot += q;

        while (robotID >= currentLot) {
//            printPos(currNode);
            if (turn) {
                for (Node node: portMap) {
                    if (((grid->portCount(node) == 3) || (grid->portCount(node) == 5))
                        && (!isSameNode(node, portMap[portEntered]))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
                currentLot += p;
            } else {
                for (Node node: portMap) {
                    if (((grid->portCount(node) == 3) || (grid->portCount(node) == 5))
                        && (!isSameNode(node, portMap[portEntered]))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
                currentLot += q;
            }
            turn = !turn;
        }
    }

    /* Stage 5*/
    void distributeOverGrid() {
        while (grid->getRobotCount(currNode) != 0) {
           // cout<<"curr count : "<< grid->getRobotCount(currNode)<<endl;
            vector<Node> portMap = grid->getPortMap(currNode);

            if (leaderOfNode(currNode) == robotID) {
                cout << robotID << " settled on : ";
                printPos(currNode);
                return;
            }
            if ((grid->portCount(currNode) == 2) ||
                ((grid->portCount(currNode) == 3) && (grid->isCorner(currNode))) ||
                (grid->portCount(currNode) == 4)) {
                for (Node node: portMap) {
                    //cout<<grid->portCount(node)<<endl;
                    if (((grid->portCount(node) == 2) || (grid->portCount(node) == 3) || (grid->portCount(node) == 4))
                        && ((!isSameNode(node, portMap[portEntered])) || (grid->isCorner(currNode)))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
                continue;
            } else if (grid->portCount(currNode) == 3) {
                for (Node node: portMap) {
                    if ((grid->portCount(node) == 6)
                        && (!isSameNode(node, portMap[portEntered]))) {
                        exitToPort(portMap, node);
                        setPortEntered(portMap, prevNode);
                        break;
                    }
                }
                continue;
            } else if (grid->portCount(currNode) == 5) {
                bool hasCorner = false;
                for (Node node: portMap) {
                    if (grid->isCorner(node)) {
                        hasCorner = true;
                        break;
                    }
                }

                if (hasCorner) {
                    // 6 degree node after 4 degree node
                    for (int i = 0; i < portMap.size(); i++) {
                        if (grid->portCount(portMap[i]) == 4) {
                            Node prev, next;
                            if (i == 0) {
                                prev = portMap[portMap.size() - 1];
                            } else prev = portMap[i - 1];

                            if (i == portMap.size() - 1) {
                                next = portMap[0];
                            } else next = portMap[i + 1];

                            if (grid->portCount(prev) == 6) {
                                exitToPort(portMap, prev);
                                setPortEntered(portMap, prevNode);
                            } else if (grid->portCount(next) == 6) {
                                exitToPort(portMap, next);
                                setPortEntered(portMap, prevNode);
                            }
                        }
                    }

                } else {
                    //2nd 6 degree node
                    bool firstTime = true;
                    Node prev = portMap[portMap.size() - 1], next;
                    for (int i = 0; i < portMap.size(); i++) {
                        if (i == portMap.size() - 1) {
                            next = portMap[0];
                        } else next = portMap[i + 1];

                        if ((grid->portCount(portMap[i]) == 6) &&
                            (grid->portCount(prev) == 6) &&
                            (grid->portCount(next) == 6)
                                ) {
                            exitToPort(portMap, portMap[i]);
                            setPortEntered(portMap, prevNode);
                        }
                        prev = portMap[i];
                    }
                }


                continue;
            } else if (grid->portCount(currNode) == 6) {
                exitToPort(portMap, portMap[(portEntered + 3) % portMap.size()]);
                setPortEntered(portMap, prevNode);
                continue;
            }
        }

    }

    int getId() {
        return robotID;
    }


    void setGlobalLeader(int x) {
        this->globalLeader = x;
    }

    ~Robot() {}
};


void printRobotsPos();

void initIO() {
    char inputFile[] = {"input.txt"}, outputFile[] = {"output.txt"};
    freopen(inputFile, "r", stdin);
    freopen(outputFile, "w", stdout);
}

Robot robots[10];
int noOfRobots;

int main() {
    initIO();

    /* Take Configuration */
    int rows, cols;
    cin >> rows >> cols;
    layers = rows;

    Grid grid(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cin >> grid.cells[i][j];
        }
    }


    cin >> noOfRobots;


    int xx, yy;

    for (int i = 0; i < noOfRobots; i++) {
        cin >> xx >> yy;
        robots[i].setPosition(xx, yy);
        robots[i].setGrid(&grid);
        robots[i].setId(i);
    }

    /* Stage 1 */
    for (int i = 0; i < noOfRobots; i++) robots[i].moveToBoundary();

    /* Stage 2 */
    for (int i = 0; i < noOfRobots; i++) robots[i].moveToAnyCorner();

    /* Stage 3 */
    for (int i = 0; i < noOfRobots; i++) robots[i].moveToOneCorner();

    /* Stage 4 */
    for (int i = 0; i < noOfRobots; i++) robots[i].distributeOverBoundary();

    /* Stage 5 */
    for (int i = 0; i < noOfRobots; i++) robots[i].distributeOverGrid();

    /* Final Positions of Robots */
    return 0;
}


int leaderOfNode(Node node) {
    int leaderId = INT_MAX;
    for (int i = 0; i < noOfRobots; i++) {
        if ((robots[i].currNode.x == node.x) && (robots[i].currNode.y == node.y)) {
            leaderId = min(robots[i].getId(), leaderId);
        }
    }
    return leaderId;
}

int anchorOfNode(Node node) {
    int leaderId = INT_MIN;
    for (int i = 0; i < noOfRobots; i++) {
        if ((robots[i].currNode.x == node.x) && (robots[i].currNode.y == node.y)) {
            leaderId = max(robots[i].getId(), leaderId);
        }
    }
    return leaderId;
}

void broadcastGlobalLeader(int leader) {
    for (int i = 0; i < noOfRobots; i++) {
        if (robots[i].globalLeader == -1)
            robots[i].globalLeader = leader;
        else break;
    }
}