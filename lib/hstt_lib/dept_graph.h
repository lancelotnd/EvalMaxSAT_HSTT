#include <set>
#include <vector>
#include <map>

class DeptGraph {
    int block_id = 0;
    int block_number = 0;
    std::map<std::string, int> dept_block;

public:
    void connectNeighbors(std::set<std::string> connexes){
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
                block_number++;
                dept_block[name] = block_id;
            } else if(dept_block[first] != dept_block[name]){
                if(dept_block[name] == 0){
                    dept_block[name] = dept_block[first];
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
                    merge_block(oldest_block, block_to_merge);
                };
            }
       }
    }

    void merge_block(int oldest_block, int block_to_merge){
        for(auto &pair: dept_block){
            if(pair.second == block_to_merge){
                pair.second = oldest_block;
            }
        }
        block_number--;
    }


};

