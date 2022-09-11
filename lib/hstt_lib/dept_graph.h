#include <set>
#include <vector>
#include <map>

class DeptGraph {
    int block_id = 0;
    int block_number = 0;
    std::map<std::string, int> dept_block;
    std::map<std::string, int> teacher_block;
    std::map<int, std::set<std::string>> block_teachers;
    std::map<int, std::set<string>> block_dept;

public:

    DeptGraph(){}

    void connectNeighbors(std::string teacher, std::set<std::string> connexes){
        int id = block_id;
        for(auto name : connexes){
            if(dept_block.count(name) == 0)
                dept_block[name] = 0;
        }
        bool is_first = true;
        std::string first = "";

        for(auto name: connexes){
            if(is_first){
                first = name;
                is_first = false;
        }
            if(dept_block[first] == 0 && dept_block[name] == 0){
                block_id++;
                id++;
                block_number++;
                dept_block[name] = block_id;
            } else if(dept_block[first] != dept_block[name]){
                if(dept_block[name] == 0){
                    dept_block[name] = dept_block[first];
                    id = dept_block[first];
                } else {
                    /**
                     * One block is excessive and should be merged with the oldest.
                     */
                     int block_1 = dept_block[first];
                     int block_2 = dept_block[name];
                     int oldest_block = std::min(block_1, block_2);
                     int block_to_merge;
                     if(block_1 == oldest_block){
                         block_to_merge = block_2;
                     } else {
                         block_to_merge = block_1;
                     }
                     id = oldest_block;
                    merge_block(oldest_block, block_to_merge);
                };
            }
       }
        std::cout << "There are " << block_number << " blocks" <<  std::endl;
        block_teachers[id].insert(teacher);
        reverse_then_print_map();
    }

    void merge_block(int oldest_block, int block_to_merge){
        for(auto &pair: dept_block){
            if(pair.second == block_to_merge){
                pair.second = oldest_block;
            }
        }
        block_teachers[oldest_block].insert(
                block_teachers[block_to_merge].begin(),
                block_teachers[block_to_merge].end());
        block_teachers.erase(block_to_merge);
        block_number--;
    }

    void reverse_then_print_map(){
        block_dept.clear();
        for(auto p : dept_block){
            block_dept[p.second].insert(p.first);
        }
        for(auto p: block_dept){
            std::cout << p.first << " ";
            for(auto d:p.second){
                std::cout << d <<  " ";
            }
            std::cout << std::endl;
        }
        std::cout << "===================================================" << std::endl;
        for(auto  p: block_teachers){
            if(!p.second.empty()){
                std::cout << p.first << " (" << p.second.size()<<") " ;
                for(auto d:p.second){
                    std::cout << d <<  " ";
                }
                std::cout << std::endl;
            }
        }
    }


};

