#include <iostream>
using namespace std;
class Processor
{
public:
    int PID;
    int JID;
    bool available;
    int utilizationTime;
        Processor()
    {

    }
    Processor(int id)
    {
        PID = id;
        available = true;
        utilizationTime = 0;
    }
    void getProcess()
    {
        cout << "PID: " << PID << endl;
        cout << "Available: " << available << endl;
    }
    void addJob(int jobId) {
        available = false;
        JID = jobId;
    }
    void removeJob() {
        available = true;
        JID = -1;
    }
};
