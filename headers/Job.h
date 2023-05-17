// Throughput
// • Average turnaround times
// • Average wait times
// • Average response times
// • Processor utilization
#include <iostream>
using namespace std;
class Job
{
public:
    int turnAroundtime;
    int waitTime;
    int responseTime;
    int JobId;
    Job()
    {
        turnAroundtime = 0;
        waitTime = 0;
        responseTime = 0;
    }
    
    Job(int id)
    {
        turnAroundtime = 0;
        waitTime = 0;
        responseTime = 0;
        JobId = id;
    }


    void setWaitTime(int time) {
        waitTime = time;
    }
};