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


    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("xml-files/DenmarkSmallSchool.xml");
    if (!result)
        return -1;

    for (pugi::xml_node instance: doc.child("HighSchoolTimetableArchive").child("Instances").children("Instance"))
    {
        Times t = Times(instance);
        Resources r = Resources(instance);
        Events e = Events(instance, r);
        Constraints c = Constraints(instance);
        Encoder encoder = Encoder(t,r,e,c);
        encoder.encode();
    }

}