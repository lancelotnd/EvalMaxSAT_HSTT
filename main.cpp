#include "pugixml-1.12/src/pugixml.hpp"
#include <iostream>
#include <string.h>

int main()
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("xml-files/ArtificialSudoku4x4.xml");
    if (!result)
        return -1;

    for (pugi::xml_node instance: doc.child("HighSchoolTimetableArchive").child("Instances").children("Instance"))
    {
        std::string instance_name = instance.child("MetaData").child("Name").child_value();
        std::string instance_description = instance.child("MetaData").child("Description").child_value();
        std::cout << "The instance name is : " << instance_name << std::endl;
        std::cout << "The description is : " << instance_description << std::endl;
    }
}