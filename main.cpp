#include "lib/pugixml-1.12/src/pugixml.hpp"
#include <iostream>
#include <string.h>
#include "lib/hstt_lib/Times.h"
#include "lib/hstt_lib/Resources.h"
#include "lib/hstt_lib/Events.h"
#include "lib/hstt_lib/Constraints.h"
#include "lib/hstt_lib/encoder.h"

int main()
{

    test_encode();
/*
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("xml-files/USAWestside2009.xml");
    if (!result)
        return -1;

    for (pugi::xml_node instance: doc.child("HighSchoolTimetableArchive").child("Instances").children("Instance"))
    {
        Times t = Times(instance);
        Resources r = Resources(instance);
        Events e = Events(instance);
        Constraints c = Constraints(instance);
    }
    */
}