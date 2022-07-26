/**
* The goal of this version is to reduce drastically the number
 * of variables of the problem by essentially solving smaller sections
 * of the problem
 *
*/


/**
 * Here we consider viewing the problem with the perspective of a single resource type.
 * We take each resource of this type and count the total duration it is sollicited
 * accross all events. Then we try and allocate a schedule for the resource taking into
 * account constraints PrefferedTimes and the likes. If there are wildcards, such as a room
 * we will try and assign the same wildcard for the same resource as much as possible. Finally
 * we will put the events into the Resource's schedule.
 */

#include "../pysat/mto.hh"
#include <vector>
#include "../../ipamir.h"

class EncoderV2 {
    Times& t;
    Resources& r;
    Events& e;
    Constraints& c;
    int nb_clauses = 0;


public: EncoderV2(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){
        std::cout << "c Constructed Encoder V2" << std::endl;
    }

    void encode() {
        propagate_constraints();
        std::map<std::string, std::string> allTypes = r.get_resources_types();
        for(auto &i : allTypes){
            for(auto res: r.getResourcesOfType(i.first)){
                void * solver = ipamir_init();
                Resource * tmp = r.getPrt(res);
                t.size();
                std::vector<int> allTimes;
                int top_lit = 0;
                for(int i = 0; i <t.size(); i++){
                    ipamir_add_hard(solver, i+1);
                    allTimes.push_back(i+1);
                    top_lit = i+1;
                }
                ipamir_add_hard(solver,0);
                ClauseSet c;
                int nvar = top_lit;
                std::vector<int> durations = tmp->getDurations();
                int dur = tmp->getTotalDuration();
                if(dur > 0){
                    kmto_encode_equalsN(top_lit, c, allTimes, dur);
                    push_clause_to_solver(solver, c,top_lit);
                    tmp->printResource();
                    std::cout << "Durations : ";
                    for(auto l : durations){
                        std::cout << l << " ";
                    }
                    int return_code = ipamir_solve(solver);
                    std::cout << "return code is " << return_code<< std::endl;
                    if(return_code == 30) {
                        std::vector<int> allPeriods;
                        std::cout << "Satisfiable" << std::endl;
                        for(int i = 0; i < nvar; i++){
                            int literal = ipamir_val_lit(solver, i+1);
                            if(literal > 0)
                            {
                                allPeriods.push_back(literal);
                                std::cout << literal << " ";
                            }
                        }
                        std::cout << std::endl;

                        std::cout << std::endl;
                        std::cout << "Cost " << ipamir_val_obj(solver)<< std::endl;
                    }

                }
            }
            }
        }


        void propagate_constraints(){
            for(int i =0; i < c.size(); i++){
                if(c[i]->getClassName() == "SplitEventConstraint") {
                    std::set<Event*> se = c[i]->getApplied(e);
                    std::cout << "Propagating split for those events";
                    for(auto e:se){
                        Constraint * cunt = c[i].get();
                        SplitEventsConstraint *s = static_cast<SplitEventsConstraint*>(cunt);
                        auto v = s->getMinMax();
                        e->addSplitConstraint(v[0], v[1], v[2], v[3]);
                        std::cout << e->getId() << " ";
                    }
                    std::cout << std::endl;
                }
            }

        }



    void push_clause_to_solver(void* solver, ClauseSet &c, int & top_lit){
        top_lit++;
        for(auto &v: c.get_clauses()){
            //std::cout << "h ";
            for(auto &l :v){
               //std::cout << l << " ";
                ipamir_add_hard(solver, l);
            }
            ipamir_add_hard(solver, -top_lit);
            //std::cout << -top_lit << " 0" << std::endl;

            ipamir_add_hard(solver, 0);
            ipamir_add_soft_lit(solver,-top_lit, 100);
            nb_clauses++;
        }
        //std::cout  << "1 " << -top_lit << " 0" << std::endl;
        c.clear();
    }

    };




