#include "../pysat/seqcounter.hh"
#include <vector>
#include "../../ipamir.h"

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
    std::set<std::vector<int>> clashes;
    int lit = 1;


public: Encoder(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){

        std::cout << "There are " << e.size() << " events " << std::endl;
        std::cout << "There are " << t.size() << " time slots " << std::endl;
        std::cout << "There are " << r.resources_types_size() << " resources types " << std::endl;
    }

    void encode(){
        std::vector<int> soft_lits;
        std::vector<std::vector<int>> allClauses;
        ClauseSet clauses;
        std::map<std::string,std::vector<int>> all_lit_for_event;
        for(int i_t = 0; i_t < t.size(); i_t++){
            std::map<std::string, std::vector<int>> wildcard_clashes;
            std::map<std::string, std::vector<int>> lits_of_event_at_time;

            for (int i_e = 0; i_e < e.size(); i_e++) {
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
                        //We store literal of this wildcards for that time.
                        wildcard_clashes[candidate].push_back(lit);
                        //we store literals of this event for that time
                        lits_of_event_at_time[s.event].push_back(lit);
                        lit++;
                    }
                }
                //We handle clash constraints
                std::vector<std::set<std::string>> clashing_events = r.getAllClashingEvents();
                for(auto & set_events : clashing_events){
                    std::vector<int> v;
                    for(auto & event : set_events) {
                        v.insert(v.end(), lits_of_event_at_time[event].begin(), lits_of_event_at_time[event].end());
                    }
                    clashes.insert(v);

                }
                //Pushing the wildcard_clashes
                for(auto & c : wildcard_clashes){
                    clashes.insert(c.second);
                }
            }
            for (auto & ev: lits_of_event_at_time) {
                all_lit_for_event[ev.first].insert(all_lit_for_event[ev.first].end(), ev.second.begin(), ev.second.end());
            }
        }
        int nvar = lit;

        //At most one for each vector
        for(auto c: clashes){
            if(!c.empty() && c.size() > 1){
                seqcounter_encode_atmostN(lit,clauses,c,1);
                lit++;
                soft_lits.push_back(lit);
                for(auto cl: clauses.get_clauses()){
                    std::vector<int> v = cl;
                    v.push_back(-lit);
                    allClauses.push_back(v);
                }
                clauses.clear();
            }
        }
        //exactly one for each event
        for(auto e: all_lit_for_event) {
            if(!e.second.empty()) {
                seqcounter_encode_atleastN(lit,clauses,e.second,1);
                lit++;
                soft_lits.push_back(lit);
                for(auto cl: clauses.get_clauses()){
                    std::vector<int> v = cl;
                    v.push_back(-lit);
                    allClauses.push_back(v);
                }
                clauses.clear();
                seqcounter_encode_atmostN(lit,clauses,e.second,1);
                lit++;
                soft_lits.push_back(lit);
                for(auto cl: clauses.get_clauses()){
                    std::vector<int> v = cl;
                    v.push_back(-lit);
                    allClauses.push_back(v);
                }
                clauses.clear();
            }
        }

    void *solver = ipamir_init();

    for(auto &vec:allClauses){
        for(auto &lit:vec) {
            ipamir_add_hard(solver,lit);
        }
        ipamir_add_hard(solver,0);
    }
    for(auto l : soft_lits){
        ipamir_add_soft_lit(solver,-l,1);
    }
    std::cout << "================================================" << std::endl;
    std::cout << "There are " << nvar << " pure literals" << std::endl;
    std::cout << "Whith constraints, there are " <<  lit << " literals, " << soft_lits.size() << " of which are soft. "  << " NB CLAUSES : " << allClauses.size()<< std::endl;
    int return_code = ipamir_solve(solver);
    if(return_code == 30){
        std::cout << "Satisfiable" << std::endl;
        std::string previous_time = "";
        for(int i = 0; i< lit; i++){
            if(ipamir_val_lit(solver, i+1) > 0){
                int literal = ipamir_val_lit(solver, i+1);
                if(literal <= nvar){
                    Solution_event s = literal_map[literal];
                    if(previous_time != s.time){
                        previous_time = s.time;
                        std::cout << "=================================================" << std::endl;
                    }
                    std::cout << s.time << " ";
                    e.getEvent(s.event).printEvent();
                    std::cout << s.assigned_resources[0]->getId() << std::endl;
                }
            }

        }
        std::cout << std::endl;
        std::cout << "Cost is " << ipamir_val_obj(solver) << std::endl;
    } else {
        std::cout << "UNSAT" << std::endl;
    }
    }

    void encode_requirements(){
    }
};