/**
* This class shall combine all solvers and keep track of
 * conflicts and overall cost of the solutions found.
 * It is also responsible to optimise the cost of the overall
 * solution.
*/

#include "solver.h"

class MetaSolver {
    std::map<std::string, std::map<int, std::set<std::string>>> SameTimeSameDeptRes;
    std::vector<std::shared_ptr<Solver>> all_solvers;
    std::map<std::string, std::shared_ptr<Solver>> map_solvers;


public:

    MetaSolver(){}

    void add_solver(std::string solver_id){
        map_solvers[solver_id] = std::make_shared<Solver>();
    }

    std::shared_ptr<Solver> operator [](std::string s) {return map_solvers[s];}

    std::map<std::string, std::map<int, std::set<std::string>>> & stsdr(){
        return SameTimeSameDeptRes;
    }
};