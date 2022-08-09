#include  "../../ipamir.h"
#include "../pysat/clset.hh"
#include "../pysat/cardenc/mto.hh"
#pragma once
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
        if(result == 30){
            std::cout << "Cost " << ipamir_val_obj(solver) << std::endl;
        } else {
            std::cout <<"UNSAT"<< std::endl;
        }

        return is_sat;
    }

    int get_val_lit(int lit){
        return ipamir_val_lit(solver, lit);
    }

    void encode_hard_at_least_n(std::vector<int> constraint, int n){
        kmto_encode_atleastN(toplit, clauses, constraint, n);
    }

    void encode_hard_at_most_n(std::vector<int> constraint, int n){
        kmto_encode_atmostN(toplit, clauses, constraint, n);
    }


    ClauseSet& getClauseSet(){
    return clauses;
}

};