# EvalMaxSAT_HSTT



This program is a proof of concept for solving the High School Timetabling problem using [MAX-SAT](https://en.wikipedia.org/wiki/Maximum_satisfiability_problem) solvers. 
It is developped around the uniform benchmarks format [XHSTT](https://www.utwente.nl/en/eemcs/dmmp/hstt/).
We take an xml file as input ([see here](https://www.utwente.nl/en/eemcs/dmmp/hstt/tutorial/) 
for the general structure of this file), then we output a solution schedule for each teacher.


To use this program :
```
make
./main xml-files/USAWestside2009.xml
```