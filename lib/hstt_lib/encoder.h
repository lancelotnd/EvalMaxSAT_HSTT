#include "../pysat/seqcounter.hh"
#include <vector>
#include "../../ipamir.h"

void test_encode(){
    int top_id = 4;
    int nvar = 4;
    std::vector<int> v = {1,2,3,4};
    ClauseSet c;
    seqcounter_encode_atmostN(top_id, c, v,3);
    seqcounter_encode_atleastN(top_id, c , v,2);
    c.dump(std::cout);
    void *solver = ipamir_init();

    for(auto &vec: c.get_clauses()){
        for(auto &lit:vec) {
            ipamir_add_hard(solver,lit);
        }
        ipamir_add_hard(solver,0);
    }
    for(int i = nvar+1; i<= top_id; i++){
        ipamir_add_soft_lit(solver,i,1);
    }
    int return_code = ipamir_solve(solver);
    if(return_code == 10){
        std::cout << "Satisfiable" << std::endl;
        for(int i =0; i< top_id; i++){
            std::cout << ipamir_val_lit(solver, i+1) << " ";

        }
        std::cout << std::endl;
        std::cout << "Cost is " << ipamir_val_obj(solver) << std::endl;
    } else {
        std::cout << "UNSAT" << std::endl;
    }
}

class   Encoder {
    Times& t;
    Resources& r;
    Events& e;
    Constraints& c;

public: Encoder(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){

        std::cout << "There are " << e.size() << " events " << std::endl;
        std::cout << "There are " << t.size() << " time sluts " << std::endl;
        std::cout << "There are " << r.resources_types_size() << " resources types " << std::endl;
    }



};