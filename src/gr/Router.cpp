#include "Router.h"

using namespace gr;

Router::Router(db::Database* database_) :   
    database(database_)
{
    grGrid = database->grGrid;
    logger = database->logger;

    // Grgrid info
    gridX = grGrid.nx;
    gridY = grGrid.ny;
    capH = grGrid.Hcap;
    capV = grGrid.Vcap;

    // Queue of nets to route
    for (db::Net* net: database->nets)
        net_queue.push_back(net);

    // Gcells | shape of a map
    gcells.resize(gridX, vector<db::GCell*>(gridY));
    for(int i = 0; i < gridX; i++){
        for(int j = 0; j < gridY; j++){
            gcells[i][j] = new db::GCell();
            gcells[i][j]->X_cor = i;
            gcells[i][j]->Y_cor = j;
            gcells[i][j]->supplyH = capH;
            gcells[i][j]->supplyV = capV;
        }
    }

    // Demand map | two directions
    // TODO: 
    demH.resize(gridY, vector<int>(gridX));     // H [Y][X]
    demV.resize(gridX, vector<int>(gridY));     // V [X][Y]

    // Route paths | net-wise
    rpaths.resize(database->nNets);
    rpoints.resize(database->nNets);
    net_rflag.resize(database->nNets);

} //END MODULE

//---------------------------------------------------------------------

bool Router::single_net_pattern(db::Net* net){
    int x_1 = net->Pins[0]->pos_x;int y_1 = net->Pins[0]->pos_y;
    int x_2 = net->Pins[1]->pos_x;int y_2 = net->Pins[1]->pos_y;

    int x_l = min(x_1, x_2);int y_l = min(y_1, y_2);
    int x_h = max(x_1, x_2);int y_h = max(y_1, y_2);

    int x_s_1; int x_s_2;
    int y_s_1; int y_s_2;
    if (((x_2 - x_1) * (y_2 - y_1)) < 0){
        x_s_1 = x_h; x_s_2 = x_l;
        y_s_1 = y_h; y_s_2 = y_l;
    } else{
        x_s_1 = x_l; x_s_2 = x_h;
        y_s_1 = y_l; y_s_2 = y_h;
    }

    int min_cost = INT_MAX;
    int cost;
    bool direction;     // true vertival | false horizontal
    int bend;       // Bending point

    // Vertical line
    for (int i = x_l; i <= x_h; i++){
        cost = 0;
        cost += accumulate(demH[y_s_1].begin() + x_l, demH[y_s_1].begin() + i, 0);
        cost += accumulate(demH[y_s_2].begin() + i, demH[y_s_2].begin() + x_h, 0);
        cost += accumulate(demV[i].begin() + y_l, demV[i].begin() + y_h, 0);
        if (cost < min_cost){
            min_cost = cost;
            direction = true;
            bend = i;
        }
    }
    
    // Horizontal line
    for (int j = y_l; j <= y_h; j++){
        cost = 0;
        cost += accumulate(demV[x_s_1].begin() + y_l, demV[x_s_1].begin() + j, 0);
        cost += accumulate(demV[x_s_2].begin() + j, demV[x_s_2].begin() + y_h, 0);
        cost += accumulate(demH[j].begin() + x_l, demH[j].begin() + x_h, 0);
        if (cost < min_cost){
            min_cost = cost;
            direction = false;
            bend = j;
        }
    }

    // Update demand matrix | Update route path
    int idx = net->id();
    if (direction == true){
        for (int i = x_l; i != bend; i++){
            demH[y_s_1][i] += 1;
            db::GCell* gcell = gcells[i][y_s_1];
            rpaths[idx].path.push_back(gcell);
            rpaths[idx].direction.push_back(false);       // true vertival | false horizontal
            gcell->demandH += 1;
            gcell->netsX.push_back(idx);                  // X = H | Y = V
        }
        for (int j = y_l; j != y_h; j++){
            demV[bend][j] += 1;
            db::GCell* gcell = gcells[bend][j];
            rpaths[idx].path.push_back(gcell);
            rpaths[idx].direction.push_back(true);
            gcell->demandV += 1;
            gcell->netsY.push_back(idx);
        }
        for (int i = bend; i != x_h; i++){
            demH[y_s_2][i] += 1;
            db::GCell* gcell = gcells[i][y_s_2];
            rpaths[idx].path.push_back(gcell);
            rpaths[idx].direction.push_back(false);
            gcell->demandH += 1;
            gcell->netsX.push_back(idx);
        }
    } else{
        for (int j = y_l; j != bend; j++){
            demV[x_s_1][j] += 1;
            db::GCell* gcell = gcells[x_s_1][j];
            rpaths[idx].path.push_back(gcell);
            rpaths[idx].direction.push_back(true);
            gcell->demandV += 1;
            gcell->netsY.push_back(idx);
        }
        for (int i = x_l; i != x_h; i++){
            demH[bend][i] += 1;
            db::GCell* gcell = gcells[i][bend];
            rpaths[idx].path.push_back(gcell);
            rpaths[idx].direction.push_back(false);
            gcell->demandH += 1;
            gcell->netsX.push_back(idx);
        }
        for (int j = bend; j != y_h; j++){
            demV[x_s_2][j] += 1;
            db::GCell* gcell = gcells[x_s_2][j];
            rpaths[idx].path.push_back(gcell);
            rpaths[idx].direction.push_back(true);
            gcell->demandV += 1;
            gcell->netsY.push_back(idx);
        }
    }

    // Writer format
    if (direction == true){
        rpoints[idx].push_back(Point(x_l, y_s_1));
        rpoints[idx].push_back(Point(bend, y_s_1));
        rpoints[idx].push_back(Point(bend, y_s_2));
        rpoints[idx].push_back(Point(x_h, y_s_2));
    } else{
        rpoints[idx].push_back(Point(x_s_1, y_l));
        rpoints[idx].push_back(Point(x_s_1, bend));
        rpoints[idx].push_back(Point(x_s_2, bend));
        rpoints[idx].push_back(Point(x_s_2, y_h));
    }

    // Update queues
    net_rflag[idx] = true;
    // net_queue.clear();

    return true;
} //END MODULE

//---------------------------------------------------------------------

bool Router::unroute_net(db::Net* net){
    int idx = net->id();
    net_rflag[idx] = false;

    for (int i = 0; i != rpaths[idx].direction.size(); i++){
        bool direction = rpaths[idx].direction[i];
        db::GCell* gcell = rpaths[idx].path[i];
        if(direction){
            demV[gcell->X_cor][gcell->Y_cor] -= 1;
            gcell->demandV -= 1;
            vector<int>::iterator pos = find(gcell->netsY.begin(), gcell->netsY.end(), idx);
            if (pos != gcell->netsY.end()) // == myVector.end() means the element was not found
                gcell->netsY.erase(pos);
            else{
                logger->info() << "Error! Path not found!\n";
                exit(1);
            }
        } else{
            demH[gcell->Y_cor][gcell->X_cor] -= 1;
            gcell->demandH -= 1;
            vector<int>::iterator pos = find(gcell->netsX.begin(), gcell->netsX.end(), idx);
            if (pos != gcell->netsX.end()) // == myVector.end() means the element was not found
                gcell->netsX.erase(pos);
            else{
                logger->info() << "Error! Path not found!\n";
                exit(1);
            }
        }
    }

    rpaths[idx].clear();
    rpoints[idx].clear();
    
    return true;
} //END MODULE

//---------------------------------------------------------------------

bool Router::break_ovfl(){
    net_queue.clear();
    for(int i = 0; i < gridX; i++){
        for(int j = 0; j < gridY; j++){
            while(demV[i][j] > capV){            // X = H | Y = V
                db::Net* net = database->nets[gcells[i][j]->netsY.back()];
                unroute_net(net);
                net_queue.push_back(net);
                net_rflag[net->id()] = false;
            }
        }
    }

    for(int i = 0; i < gridX; i++){
        for(int j = 0; j < gridY; j++){
            while(demH[j][i] > capH){           // X = H | Y = V
                db::Net* net = database->nets[gcells[i][j]->netsX.back()];
                unroute_net(net);
                net_queue.push_back(net);
                net_rflag[net->id()] = false;
            }
        }
    }

    return true;
} //END MODULE

//---------------------------------------------------------------------

bool Router::single_net_maze(db::Net* net){
    // Create maps
    vector<vector<int>> visited(gridX, vector<int>(gridY, false));
    vector<vector<int>> blkH(gridX, vector<int>(gridY, false));
    vector<vector<int>> blkV(gridX, vector<int>(gridY, false));
    for(int i = 0; i < gridX; i++){
        for(int j = 0; j < gridY; j++){
            blkH[i][j] = (demH[j][i] >= capH);
            blkV[i][j] = (demV[i][j] >= capV);
        }
    }

    int x_1 = net->Pins[0]->pos_x;int y_1 = net->Pins[0]->pos_y;
    int x_2 = net->Pins[1]->pos_x;int y_2 = net->Pins[1]->pos_y;
    cout << x_1 << " | " << y_1 << endl;
    cout << x_2 << " | " << y_2 << "\n\n\n";

    // Prepare for queues
    auto solComp = [](const std::shared_ptr<Vertex> &lhs, const std::shared_ptr<Vertex> &rhs) {
        return rhs->cost < lhs->cost;
    };
    priority_queue<std::shared_ptr<Vertex>, vector<std::shared_ptr<Vertex>>, 
                        decltype(solComp)> solQueue(solComp);

    // Init vertex
    solQueue.push(std::make_shared<Vertex>(0, Point(x_1, y_1), nullptr));
    Point dstPin(x_2, y_2);

    // Hadlock's 
    while (!solQueue.empty()) {
        auto curVer = solQueue.top();
        solQueue.pop();
        int x_v = curVer->pos.x_;
        int y_v = curVer->pos.y_;
        int cost = curVer->cost;
        visited[x_v][y_v] = true;

        // if (x_v == 0 && y_v == 58) cout << curVer->prev->pos.x_ << " | " << curVer->prev->pos.y_ << endl;

        // reach a pin?
        if (curVer->pos == dstPin) {
            shared_ptr<Vertex> cur = curVer;
            // break;
            while (cur->prev != nullptr){
                shared_ptr<Vertex> prev = cur->prev;
                logger->info() << prev->pos.x_ << ", " << prev->pos.y_ << endl;
                cur = prev;
            }
            break;
        }

        // 4 directions
        for(int d = 0; d < 4; d++){
            // Next vertex to visit | remember to calc the bound
            int x_off = Direction.x_off[d];
            int y_off = Direction.y_off[d];
            int x_new = x_v + x_off;
            int y_new = y_v + y_off;
            if (x_new < 0 || y_new < 0 || x_new >= gridX || y_new >= gridY) continue;
            // logger->info() << x_new << " | " << y_new << endl;

            // This direction has block
            if (d == 0 && blkV[x_v][y_v]) continue;
            if (d == 1 && blkV[x_v][y_v - 1]) continue;
            if (d == 2 && blkH[x_v - 1][y_v]) continue;
            if (d == 3 && blkH[x_v][y_v]) continue;

            // If not visited
            if (!visited[x_new][y_new]) {
                Point newPin(x_new, y_new);
                int new_cost = cost;

                if ((newPin - dstPin) > (curVer->pos - dstPin)) new_cost++;
                solQueue.push(std::make_shared<Vertex>(new_cost, newPin, curVer));
                visited[x_new][y_new] = true;
            }
        }
    }

    return true;
} //END MODULE

//---------------------------------------------------------------------

void Router::print_demand(){
    for(int j = 0; j < gridY; j++){
        for(int i = 0; i < gridX; i++){
            logger->info() << setw(3) << demV[i][j];
        }
        logger->info() << endl;
    }
    logger->info() << endl;
    logger->info() << endl;
    for(int j = 0; j < gridY; j++){
        for(int i = 0; i < gridX; i++){
            logger->info() << setw(3) << demH[j][i];
        }
        logger->info() << endl;
    }
    logger->info() << endl;
    logger->info() << endl;
    logger->info() << endl;
    logger->info() << endl;
} //END MODULE

//---------------------------------------------------------------------

void Router::write(const string& output_path) {
    ofstream outfile;
    outfile.open(output_path, ios::out);
    
    for(int idx = 0; idx < database->nNets; idx++){
        outfile << database->nets[idx]->name() << ' ' 
                    << idx << "\n";
        for(int j = 0; j < rpoints[idx].size() - 1; j++){
            outfile << '(' << rpoints[idx][j].x_ << ", "
                            << rpoints[idx][j].y_ << ", 1)-("
                            << rpoints[idx][j+1].x_ << ", "
                            << rpoints[idx][j+1].y_ << ", 1)\n";
        }
        outfile << "!\n";
    }
    outfile.close();
} //END MODULE

//---------------------------------------------------------------------

