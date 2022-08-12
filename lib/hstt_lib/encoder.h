#include "../pysat/seqcounter.hh"
#include <vector>
#include "../../ipamir.h"

struct Solution_event {
    int lit;
    std::string time;
    std::string event;
    bool has_wildcard = false;
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
    int nb_clauses = 0;


public: Encoder(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){

        std::cout << "c There are " << e.size() << " events " << std::endl;
        std::cout << "c There are " << t.size() << " time slots " << std::endl;
        std::cout << "c There are " << r.resources_types_size() << " resources types " << std::endl;
    }

    void encode(){
        std::cout << "c ENCODING ALL LITERALS" << std::endl;
        void *solver = ipamir_init();
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
                        s.has_wildcard = true;
                        literal_map[lit] = s;
                        //We store literal of this wildcards for that time.
                        wildcard_clashes[candidate].push_back(lit);
                        //we store literals of this event for that time
                        lits_of_event_at_time[s.event].push_back(lit);
                        //std::cout << "c Encoded LIT " << lit << std::endl;
                        lit++;
                    }
                }
                //In some cases there are no wildcard resources to fulfill
                //All we need is to place the events on the grid.
                if(potential_resources_for_type.empty()){
                    Solution_event s;
                    s.time = t[i_t].getId();
                    s.event = e[i_e].getId();
                    s.lit = lit;
                    s.has_wildcard = false;
                    literal_map[lit] =s;
                    lits_of_event_at_time[s.event].push_back(lit);
                    //std::cout << "c Encoded LIT " << lit << std::endl;
                    lit++;
                }

                //We handle clash constraints
                //std::cout << "c Encoding clash constraints" << std::endl;
                std::vector<std::set<std::string>> clashing_events = r.getAllClashingEvents();
                for(auto & set_events : clashing_events){
                    std::vector<int> v;
                    if(!set_events.empty()){
                        for(auto & event : set_events) {
                            std::cout << event << " ";
                            v.insert(v.end(), lits_of_event_at_time[event].begin(), lits_of_event_at_time[event].end());
                        }
                        std::cout << std::endl;
                        clashes.insert(v);
                    }
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
        std::cout << "c GENERATING and PUSHING AT MOST ONE CONSTRAINT to solver" << std::endl;

        //At most one for each vector
        for(auto c: clashes){
            if(!c.empty() && c.size() > 1){
                seqcounter_encode_atmostN(lit,clauses,c,1);
                push_clause_to_solver(solver, clauses,lit);
            }
        }
        std::cout << "c GENERATING and PUSHING EXACTLY ONE CONSTRAINT to solver" << std::endl;

        //exactly one for each event
        for(auto e: all_lit_for_event) {
            if(!e.second.empty()) {
                seqcounter_encode_atleastN(lit,clauses,e.second,1);
                push_clause_to_solver(solver,clauses,lit);
                seqcounter_encode_atmostN(lit,clauses,e.second,1);
                push_clause_to_solver(solver,clauses,lit);
            }
        }

    std::cout << "c ================================================" << std::endl;
    std::cout << "c There are " << nvar << " pure literals" << std::endl;
    std::cout << "c Whith constraints, there are " <<  lit << " literals, " << lit-nvar << " of which are soft. "  << " NB CLAUSES : " <<nb_clauses<< std::endl;
    std::cout << "c START SOLVING" << std::endl;
    int return_code = ipamir_solve(solver);
    if(return_code == 30){
        std::cout << "c Satisfiable" << std::endl;
        std::string previous_time = "";
        for(int i = 0; i < lit; i++){
            if(ipamir_val_lit(solver, i+1) > 0){
                int literal = ipamir_val_lit(solver, i+1);
                if(literal <= nvar){
                    Solution_event s = literal_map[literal];
                    if(previous_time != s.time){
                        previous_time = s.time;
                        std::cout << "c =================================================" << std::endl;
                    }
                    std::cout << "c "<< s.time << " ";
                    e.getEvent(s.event).printEvent();
                    if(s.has_wildcard){
                        std::cout << s.assigned_resources[0]->getId();
                    }
                    std::cout << std::endl;
                }
            }
        }
        std::cout << std::endl;
        std::cout << "c Cost is " << ipamir_val_obj(solver) << std::endl;
    } else {
        std::cout << "UNSAT" << std::endl;
    }

    }

    void push_clause_to_solver(void* solver, ClauseSet &c, int & top_lit){
        top_lit++;
        for(auto &v: c.get_clauses()){
            std::cout << "h ";
            for(auto &l :v){
                std::cout << l << " ";
                ipamir_add_hard(solver, l);
            }
            std::cout << -top_lit << " 0" << std::endl;
            ipamir_add_hard(solver, -top_lit);
            ipamir_add_hard(solver, 0);
            nb_clauses++;
        }
        std::cout  << "1 " << -top_lit << " 0" << std::endl;
        ipamir_add_soft_lit(solver, -top_lit, 1);
        c.clear();
    }


};