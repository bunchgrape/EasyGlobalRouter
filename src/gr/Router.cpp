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
    demH.resize(gridY, vector<int>(gridX));     // X * Y
    demV.resize(gridX, vector<int>(gridY));     // Y * X

    // Route paths | net-wise
    rpaths.resize(database->nNets);



    // for(int i = 0; i < grGrid.nx; i++){
    //     for(int j = 0; j <  grGrid.ny; j++){
    //         database->logger->info() << grGrid.gcells[i][j].X_cor;
    //         database->logger->info() << grGrid.gcells[i][j].Y_cor;
    //         database->logger->info() << grGrid.gcells[i][j].supplyH;
    //         database->logger->info() << grGrid.gcells[i][j].supplyV << endl;
    //     }
    // }

    // database->logger->info() << gridX << endl;
    // database->logger->info() << gridY << endl;
    // database->logger->info() << capH << endl;
    // database->logger->info() << capV << endl;

    // for(int i = 0; i < grGrid.nx; i++){
    //     for(int j = 0; j <  grGrid.ny; j++){
    //         cout << demH[i][j];
    //     }
    //     cout << endl;
    // }

    // sum_of_elems = std::accumulate(vector.begin(), vector.end(),
    //                                 decltype(vector)::value_type(0));
    // cout << "---------------------------------\n";
    // vector<int> a = {1,2,3,4};
    // // cout << accumulate(a.begin()+0,a.begin()+3,0) << endl;
    // a[2]+=1;
    // cout<<a[2]<<endl;
}

bool Router::single_net_pattern(db::Net* net){
    int x_1 = net->Pins[0]->pos_x;
    int y_1 = net->Pins[0]->pos_y;
    int x_2 = net->Pins[1]->pos_x;
    int y_2 = net->Pins[1]->pos_y;

    int x_l = min(x_1, x_2);
    int y_l = min(y_1, y_2);
    int x_h = max(x_1, x_2);
    int y_h = max(y_1, y_2);

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
    int solution;
    // vertical line
    for (int i = x_l; i <= x_h; i++){
        cost = 0;
        cost += accumulate(demH[y_s_1].begin() + x_l, demH[y_s_1].begin() + i, 0);
        cost += accumulate(demH[y_s_2].begin() + i, demH[y_s_2].begin() + x_h, 0);
        cost += accumulate(demV[i].begin() + y_l, demV[i].begin() + y_h, 0);
        if (cost < min_cost){
            min_cost = cost;
            direction = true;
            solution = i;
        }
    }
    
    // horizontal line
    for (int j = y_l; j <= y_h; j++){
        cost = 0;
        cost += accumulate(demV[x_s_1].begin() + y_l, demV[x_s_1].begin() + j, 0);
        cost += accumulate(demV[x_s_2].begin() + j, demV[x_s_2].begin() + y_h, 0);
        cost += accumulate(demH[j].begin() + x_l, demH[j].begin() + x_h, 0);
        if (cost < min_cost){
            min_cost = cost;
            direction = false;
            solution = j;
        }
    }

    // update demand matrix | update route path
    int idx = net->id();
    if (direction == true){
        for (int i = x_l; i != solution; i++){
            demH[y_s_1][i] += 1;
            db::GCell* gcell = gcells[i][y_s_1];
            rpaths[idx].path.push_back(gcell);
            gcell->demandH += 1;
            gcell->netsX.push_back(idx);
        }
        for (int j = y_l; j != y_h; j++){
            demV[solution][j] += 1;
            db::GCell* gcell = gcells[solution][j];
            rpaths[idx].path.push_back(gcell);
            gcell->demandV += 1;
            gcell->netsY.push_back(idx);
        }
        for (int i = solution; i != x_h; i++){
            demH[y_s_2][i] += 1;
            db::GCell* gcell = gcells[i][y_s_2];
            rpaths[idx].path.push_back(gcell);
            gcell->demandH += 1;
            gcell->netsX.push_back(idx);
        }
    } else{
        for (int j = y_l; j != solution; j++){
            demV[x_s_1][j] += 1;
            db::GCell* gcell = gcells[x_s_1][j];
            rpaths[idx].path.push_back(gcell);
            gcell->demandV += 1;
            gcell->netsX.push_back(idx);
        }
        for (int i = x_l; i != x_h; i++){
            demH[solution][i] += 1;
            db::GCell* gcell = gcells[i][solution];
            rpaths[idx].path.push_back(gcell);
            gcell->demandV += 1;
            gcell->netsY.push_back(idx);
        }
        for (int j = solution; j != x_h; j++){
            demV[x_s_2][j] += 1;
            db::GCell* gcell = gcells[x_s_2][j];
            rpaths[idx].path.push_back(gcell);
            gcell->demandH += 1;
            gcell->netsX.push_back(idx);
        }
    }

    return true;
}