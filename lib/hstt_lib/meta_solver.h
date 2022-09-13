/**
* This class shall combine all solvers and keep track of
 * conflicts and overall cost of the solutions found.
 * It is also responsible to optimise the cost of the overall
 * solution.
*/

#include "solver.h"
#include "Events.h"
#include "Resources.h"


class MetaSolver {
    Resources& r;
    Events& e;

    std::map<std::string, std::map<int, std::set<std::string>>> SameTimeSameDeptRes;
    std::map<std::string, int> groups_capacity;
    int number_of_conflicts = 0;
    std::map<std::string, std::vector<int>> conflicts;
    std::vector<std::shared_ptr<Solver>> all_solvers;
    std::map<std::string, std::shared_ptr<Solver>> map_solvers;
    int global_objective = 0;


public:


    MetaSolver(Events& e, Resources& r): e(e), r(r){

    }

    MetaSolver& operator=(const MetaSolver& other)
    {
        if(this == &other) return *this;
        r = other.r;
        e = other.e;
        SameTimeSameDeptRes = other.SameTimeSameDeptRes;
        groups_capacity = other.groups_capacity;
        number_of_conflicts = other.number_of_conflicts;
        conflicts = other.conflicts;
        all_solvers = other.all_solvers;
        map_solvers = other.map_solvers;
        global_objective = other.global_objective;

        return *this;
    }

    void add_solver(std::string solver_id){
        map_solvers[solver_id] = std::make_shared<Solver>();
    }

    bool solve(std::string solver){
        bool is_sat = map_solvers[solver]->solve();
        if(is_sat){
            global_objective += map_solvers[solver]->get_objective();
            std::cout << "Global objective " << global_objective << std::endl;
        }

        return is_sat;
    }

    void output_stats(){
        for(auto group : SameTimeSameDeptRes){
                int capacity = groups_capacity[group.first];
                for(auto period: group.second){
                    if(period.second.size() > capacity){
                        number_of_conflicts += period.second.size() - capacity;
                        conflicts[group.first].emplace_back(period.first);
                    }
                }
        }
        std::cout << "Number of conflicts : " << number_of_conflicts << std::endl;
    }

    void add_group_capacity(std::string group, int capacity){
        groups_capacity[group] = capacity;
    }

    std::shared_ptr<Solver> operator [](std::string s) {return map_solvers[s];}

    std::map<std::string, std::map<int, std::set<std::string>>> & stsdr(){
        return SameTimeSameDeptRes;
    }


    void clearGroupsOfSolver(std::string solver){

    }

};