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


struct Solution_event {
    int lit;
    std::string time;
    std::string event;
    std::vector<Resource*> assigned_resources;
} ;

class   Encoder {
    Times& t;
    Resources& r;
    Events& e;
    Constraints& c;
    std::map<int, Solution_event> literal_map;
    std::vector<std::vector<int>> clashes;

public: Encoder(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){

        std::cout << "There are " << e.size() << " events " << std::endl;
        std::cout << "There are " << t.size() << " time sluts " << std::endl;
        std::cout << "There are " << r.resources_types_size() << " resources types " << std::endl;
    }

    void encode(){
        int lit = 1;
        for(int i_t = 0; i_t < t.size(); i_t++){
            std::map<std::string,std::vector<int>> all_lit_for_event;
            for (int i_e = 0; i_e < e.size(); i_e++) {
                std::map<std::string, std::vector<int>> lits_of_event_at_time;
                std::map<std::string, std::vector<std::string>> potential_resources_for_type;
                for(auto w: e[i_e].getWildCards()){
                    potential_resources_for_type[w] = r.getResourcesOfType(w);
                    //We encode a new lit for
                    for(auto candidate : potential_resources_for_type[w]){
                        Solution_event s;
                        s.time = t[i_t].getId();
                        s.event = e[i_e].getId();
                        s.assigned_resources = {r.getPrt(candidate)};
                        s.lit = lit;
                        literal_map[lit] = s;
                        //we store literals of this event for that time
                        lits_of_event_at_time[s.event].push_back(lit);
                        lit++;
                        std::cout << "LIT " << lit << " for " << s.time << " " << s.event << " " << s.assigned_resources[0]->getId()  << std::endl;
                    }
                }
                //We handle clash constraints
                std::vector<std::set<std::string>> clashing_events = r.getAllClashingEvents();
                for(auto & set_events : clashing_events){
                    std::vector<int> v;
                    for(auto & event : set_events) {
                        v.insert(v.end(), lits_of_event_at_time[event].begin(), lits_of_event_at_time[event].end());
                    }
                    clashes.push_back(v);
                }
            }
        }
    }

    void encode_requirements(){

    }

};