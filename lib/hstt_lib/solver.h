#include  "../../ipamir.h"
#include "../pysat/clset.hh"
class Solver {

public: static inline int toplit{0};
void * solver;
ClauseSet clauses;
bool is_sat = false;

public:   Solver(){
        solver = ipamir_init();
    }


    bool solve(){
        for (auto cl : clauses.get_clauses()){
            for (auto l :cl){
                ipamir_add_hard(solver, l);
            }
            ipamir_add_hard(solver,0);
        }
        int result = ipamir_solve(solver);
        is_sat = result == 30;
        std::cout << result << std::endl;
        return is_sat;
    }

    int get_val_lit(int lit){
        return ipamir_val_lit(solver, lit);
    }


    ClauseSet& getClauseSet(){
    return clauses;
}

};