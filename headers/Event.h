//PID #, Arrival t, CPU t, IO t, CPU t, IO t, CPU t
class Event
{
    public:
        int eventType;
        int CPUTime;
        int arrivalTime;
        int IOTime;
        int iteration;
        int type;
        Job job;
        int age;
        
    Event(int event, int CPU, int arrival, int IO, Job j)
    {
        eventType = event;
        CPUTime = CPU;
        arrivalTime = arrival;
        IOTime = IO;
        iteration =0;
        job = j;
        age = 0;
    };
        Event(int event, int CPU, int arrival, int IO, Job j, int it)
    {
        eventType = event;
        CPUTime = CPU;
        arrivalTime = arrival;
        IOTime = IO;
        iteration =  it;
        job = j;
        age = 0;
        
    };
    bool operator()(const Event &e1, const Event &e2)
    {
        return e1.CPUTime > e2.CPUTime;
    };

    void setAge(int ag)
    {
        age = ag;
    };
    void printEvent()
    {
        cout<<"CPU Time: "<<CPUTime<<endl;
        cout<<"Arrival Time: "<<arrivalTime<<endl;
        cout<<"Iteration: "<<iteration<<endl;
        cout<<"Job"<<job.JobId<<endl;
    }

    void setJobWaitTime(int a) {
        job.setWaitTime(a);
    }
};
