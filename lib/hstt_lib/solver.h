#include  "../../ipamir.h"
#include "../pysat/clset.hh"
class Solver {

public: static inline int toplit{0};
void * solver;
ClauseSet clauses;

public:   Solver(){
        solver = ipamir_init();
    }


    int solve(ClauseSet c){
    clauses = c;
        for (auto cl : c.get_clauses()){
            for (auto l :cl){
                ipamir_add_hard(solver, l);
            }
            ipamir_add_hard(solver,0);
        }
        int result = ipamir_solve(solver);
        std::cout << result << std::endl;

        return 0;

    }

};