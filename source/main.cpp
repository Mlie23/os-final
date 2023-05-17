// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lab 7: CPU Scheduler Dispatch Simulation
// Name: Nirjal Shakya, Deeegi, Michael Lie
// Date: 5/3/2023
// Copyright: 2023 (Nirjal, Deegi, Michael Lie)
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <queue>
#include <iomanip>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "../headers/Processor.h"
#include "../headers/Job.h"
#include "../headers/Event.h"
#include "fstream"

using namespace std;

// Sort the processes based on Arrival Time
struct sortProcessTime
{
    bool operator()(const Event &t1, const Event &t2)
    {
        return t1.arrivalTime > t2.arrivalTime;
    }
};

// Sort this for the SPN queues prioritizing the shortest CPU time left.
struct sortSPN
{
    bool operator()(const Event &t1, const Event &t2)
    {
        return t1.CPUTime > t2.CPUTime;
    }
};

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global vvariables for the simulation

float totalWaitTime = 0;
float totalResponseTime = 0;
float totalTurnaroundTime = 0;
int currentTime = 0;
const int totalTasks = 100;
priority_queue<Event, vector<Event>, sortProcessTime> FEL_main;
priority_queue<Event, vector<Event>, sortProcessTime> FEL;
vector<Event> Readyqueue;
vector<Event> RR1;
vector<Event> RR2;
priority_queue<Event, vector<Event>, sortSPN> SPN;
vector<Event> FCFS;

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////

void incrementAge();
void movePriority();
bool checkProcessorsAvailability(int totalProcessors, Processor p[]);
int findAvailableProcessor(Processor p[], int totalProcessors);
void arrivalEvent(Event event, Processor p[], int totalProcessors, int simId);
void departureEvent(Event event, Processor p[], int totalProcessors, int simId, int &totalJob, int sim);
void changeProcessorAvailability(Event e, Processor p[], int totalProcessors);
void moveQueues(vector<Event> &algo2, int ageVal);

int main()
{
    int simTime8 = 800;
    int simTime4 = 1600;
    int simulationList[14] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6};
    int processorList[14] = {4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8};
    string configList[14] = {"[4] RR16 RR32 SPN ", "[8] RR16 RR32 SPN", "[4] RR16 RR32 FCFS", "[8] RR16 RR32 FCFS",
                             "[4] RR16", "[8] RR16", "[4] FCFS", "[8] FCFS", "[4] SPN", "[8] SPN", "[4] RR32", "[8] RR32", "[4] Random Iteration", "[8] Random Iteration"};
    srand(time(NULL));
    // Randomize the total tasks
    for (int i = 0; i < totalTasks; i++)
    {
        int random = (rand() % 10);
        int randomIO = (rand() % 5 + 1);
        // cout << randomIO;
        FEL_main.push(Event(0, rand() % 100 + 30, random, randomIO, Job(i)));
    }

    ofstream file;
    file.open("Result.csv");
    file << "Configuration,"
         << "Processors,"
         << "Jobs,"
         << "Total Simulation Time,"
         << "Avg. Throughput,"
         << "Avg. Wait Time,"
         << "Avg Response Time,"
         << "Avg. Turnaround Time,"
         << "Avg. Utilization for Processors,"
         << "\n";

    // While future event list is not empty, continue the simulation.
    for (int j = 0; j < 14; j++)
    {
        totalWaitTime = 0;
        totalResponseTime = 0;
        totalTurnaroundTime = 0;
        currentTime = 0;
        FEL = FEL_main;
        int totalProcessors = processorList[j];
        int totalJobs = 0;
        Processor p[totalProcessors];
        int sim;
        if (j%2==0)
        {
            sim = simTime4;
        }
        else{
            sim = simTime8;
        }
        for (int i = 0; i < totalProcessors; i++)
        {
            p[i] = Processor(i + 1);
        };
        while (FEL.size() > 0)
        {
            // Pop the event from the very top of the future event list
            // Set current time to arrival time
            // Switch based on event type (Arrival, Departure)
            Event topEvent = FEL.top();
            currentTime = topEvent.arrivalTime;
            FEL.pop();
            switch (topEvent.eventType)
            {
            case 0:
                arrivalEvent(topEvent, p, totalProcessors, simulationList[j]);
                break;
            case 1:
                departureEvent(topEvent, p, totalProcessors, simulationList[j], totalJobs, sim);
                break;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Implement the aging mechanism
            // Increment the age of everything in the queue
            // Move an event from RR2, SPN, FCFS based on the age tresshold
            // The lower the queue, the smaller the
            incrementAge();
            switch (j)
            {
            case 0:
                moveQueues(RR1, 20);
                moveQueues(RR2, 15);
                movePriority();
                break;

            case 1:
                moveQueues(RR1, 20);
                moveQueues(RR2, 15);
                moveQueues(FCFS, 5);
                break;

            default:
                break;
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Output Simulation Statistics
        // cout << "Total Simulation time: " << currentTime << endl;
        // cout << "Total Tasks: " << totalTasks << endl;
        // cout << "Average Throughput: " << float(float(totalTasks) / float(currentTime)) << endl;
        // cout << "Average Wait Time: " << float(totalWaitTime / totalTasks) << endl;
        // cout << "Average Response Time: " << float(totalResponseTime / totalTasks) << endl;
        // cout << "Average Turnaround Time: " << float(totalTurnaroundTime / totalTasks) << endl;
        float totalAverageUtilization = 0.0;
        for (int i = 0; i < totalProcessors; i++)
        {
            totalAverageUtilization += float(float(p[i].utilizationTime) / float(currentTime) * 100);
            // cout << "Utilization time for proccessor " << i + 1 << " is " << p[i].utilizationTime << endl;
            // cout << "Utilization percentage for processor " << i + 1 << " is " << float(float(p[i].utilizationTime) / float(currentTime) * 100) << "%" << endl;
        }
        // cout << "Average Utilization time for all processors: " << (totalAverageUtilization / totalProcessors) << "%" << endl;
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Write simulation to a csv file
        file << configList[j] << "," << totalProcessors << ","
             << "100," << currentTime << "," << float(float(totalTasks) / float(currentTime)) << ","
             << float(totalWaitTime / totalTasks) << "," << float(totalResponseTime / totalTasks) << "," << float(totalTurnaroundTime / totalTasks) << ","
             << (totalAverageUtilization / totalProcessors) <<"%,"<<totalJobs<< "\n";
    }

    file.close();
    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    return 0;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arrival Event
// Parameter: event that we are pasing
void arrivalEvent(Event event, Processor p[], int totalProcessors, int simId)
{
    // If we can find available processor (!=-1), add job to the processor
    int available = findAvailableProcessor(p, totalProcessors);
    if (available != -1)
    {
        p[available].addJob(event.job.JobId);
        if (event.job.responseTime == 0)
        {
            event.job.responseTime = currentTime;
        }
        int processTime;

        switch (simId)
        {
        case 0:
            // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Implement 4 different queues algorithm (RR1, RR2, SPN, FCFS)
            switch (event.iteration)
            {
            // RR1
            case 0:
                // cout<<event.IOTime<<endl;
                if (event.IOTime > 0)
                {
                    FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, --event.iteration));
                }
                else
                {
                    processTime = min(16, event.CPUTime);
                    p[available].utilizationTime += processTime;
                    event.CPUTime -= processTime;
                    FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
                    break;
                }
            break;

            // RR2
            case 1:
                processTime = min(32, event.CPUTime);
                p[available].utilizationTime += processTime;
                event.CPUTime -= processTime;
                FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
                break;

            // SPN
            case 2:
                p[available].utilizationTime += event.CPUTime;
                FEL.push(Event(1, 0, currentTime + event.CPUTime, 0, event.job, event.iteration));
                break;

            default:
                cout << "Something is not right" << endl;
                break;
            }
            break;

        case 1:
            // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Implement 4 different queues algorithm (RR1, RR2, SPN, FCFS)
            switch (event.iteration)
            {
            // RR1
            case 0:
                if (event.IOTime > 0)
                {
                    FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, --event.iteration));
                }
                else
                {
                    processTime = min(16, event.CPUTime);
                    p[available].utilizationTime += processTime;
                    event.CPUTime -= processTime;
                    FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
                }
                break;

            // RR2
            case 1:
                processTime = min(32, event.CPUTime);
                p[available].utilizationTime += processTime;
                event.CPUTime -= processTime;
                FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
                break;

            // FCFS
            case 2:
                p[available].utilizationTime += event.CPUTime;
                FEL.push(Event(1, 0, currentTime + event.CPUTime, 0, event.job, event.iteration));
                break;

            default:
                cout << "Something is not right" << endl;
                break;
            }
            break;

        case 2:
            // For just RR16
            if (event.IOTime > 0)
            {
                FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, event.iteration));
            }
            else
            {
                processTime = min(16, event.CPUTime);
                p[available].utilizationTime += processTime;
                event.CPUTime -= processTime;
                FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
            }
            break;

        case 3:
            // For just FCFS
            if (event.IOTime > 0)
            {
                FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, event.iteration));
            }
            else
            {
                p[available].utilizationTime += event.CPUTime;
                FEL.push(Event(1, 0, currentTime + event.CPUTime, 0, event.job, event.iteration));
            }
            break;

        case 4:
            // For just SPN
            if (event.IOTime > 0)
            {
                FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, event.iteration));
            }
            else
            {
                p[available].utilizationTime += event.CPUTime;
                FEL.push(Event(1, 0, currentTime + event.CPUTime, 0, event.job, event.iteration));
            }
            break;

        case 5:
            // For just RR32
            if (event.IOTime > 0)
            {
                FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, event.iteration));
            }
            else
            {
                processTime = min(32, event.CPUTime);
                p[available].utilizationTime += processTime;
                event.CPUTime -= processTime;
                FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
            }
            break;
        case 6:
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Implement 4 different queues algorithm (RR1, RR2, SPN, FCFS)
            switch (event.iteration)
            {
            // RR1
            case 0:
                if (event.IOTime > 0)
                {
                    FEL.push(Event(1, event.CPUTime, currentTime + event.IOTime, 0, event.job, event.iteration));
                }
                else
                {
                    processTime = min(16, event.CPUTime);
                    p[available].utilizationTime += processTime;
                    event.CPUTime -= processTime;
                    FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
                }
                break;

            // RR2
            case 1:
                processTime = min(32, event.CPUTime);
                p[available].utilizationTime += processTime;
                event.CPUTime -= processTime;
                FEL.push(Event(1, event.CPUTime, currentTime + processTime, 0, event.job, event.iteration));
                break;

            // SPN
            case 2:
                p[available].utilizationTime += event.CPUTime;
                FEL.push(Event(1, 0, currentTime + event.CPUTime, 0, event.job, event.iteration));
                break;
            // FCFS
            case 3:
                p[available].utilizationTime += event.CPUTime;
                FEL.push(Event(1, 0, currentTime + event.CPUTime, 0, event.job, event.iteration));
                break;
            default:
                cout << "Something is not right" << endl;
                break;
            }
            break;
        }
        default:
            break;
        }
    }
    // Else if all processors are busy
    // Add to designated queues!
    else
    {
        switch (simId)
        {
        case 0:
            // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Implement 3 different queues algorithm (RR1, RR2, SPN)
            switch (event.iteration)
            {
            case 0:
                RR1.push_back(event);
                break;
            case 1:
                RR2.push_back(event);
                break;
            case 2:
                SPN.push(event);
                break;
            default:
                break;
            }
            break;

        case 1:
            // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Implement 3 different queues algorithm (RR1, RR2, FCFS)
            switch (event.iteration)
            {
            case 0:
                RR1.push_back(event);
                break;
            case 1:
                RR2.push_back(event);
                break;
            case 2:
                FCFS.push_back(event);
                break;
            default:
                break;
            }
            break;

        case 2:
            // For just RR16
            RR1.push_back(event);
            break;

        case 3:
            // For just FCFS
            FCFS.push_back(event);
            break;

        case 4:
            // For just SPN
            SPN.push(event);
            break;

        case 5:
            // For just RR32
            RR2.push_back(event);
            break;
        case 6:
            switch (event.iteration)
            {
            case 0:
                RR1.push_back(event);
                break;
            case 1:
                RR2.push_back(event);
                break;
            case 2:
                SPN.push(event);
                break;
            // case 3:
            //     FCFS.push_back(event);
            default:
                break;
            }
            break;
        default:
            break;
        }

        event.job.waitTime += currentTime + event.IOTime;
    }
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Departure event
// Parameneter: current event
// We always prioritize RR1 -> RR2 -> SPN -> FCFS
// Just like waterfall method
// One queue cannot be performed if higher queue is not empty
void departureEvent(Event event, Processor p[], int totalProcessors, int simId, int &totalJob, int sim)
{
    changeProcessorAvailability(event, p, totalProcessors);
    if (!RR1.empty())
    {
        RR1.front().job.waitTime = currentTime - RR1.front().job.waitTime;
        arrivalEvent(RR1.front(), p, totalProcessors, simId);
        RR1.erase(RR1.begin());
    }
    else if (!RR2.empty())
    {
        RR2.front().job.waitTime = currentTime - RR2.front().job.waitTime;
        arrivalEvent(RR2.front(), p, totalProcessors, simId);
        RR2.erase(RR2.begin());
    }
    else if (!SPN.empty())
    {
        Event test = SPN.top();
        test.job.waitTime = currentTime - SPN.top().job.waitTime;
        arrivalEvent(test, p, totalProcessors, simId);
        SPN.pop();
    }
    else if (!FCFS.empty())
    {
        // cout << "FCFS is called" << endl;
        FCFS.front().job.waitTime = currentTime - FCFS.front().job.waitTime;
        arrivalEvent(FCFS.front(), p, totalProcessors, simId);
        FCFS.erase(FCFS.begin());
    }

    if (event.CPUTime == 0)
    {
        event.job.turnAroundtime = currentTime;
        totalWaitTime += event.job.waitTime;
        totalResponseTime += event.job.responseTime;
        totalTurnaroundTime += event.job.turnAroundtime;
        if (currentTime <= sim) {
            totalJob +=1;
        }
    }
    else if (event.CPUTime > 0)
    {
        if (simId == 6)
        {
            int rand_iteration = rand() % 3;
            FEL.push(Event(0, event.CPUTime, currentTime + 5, event.IOTime, event.job, rand_iteration));
        }
        else
        {
            FEL.push(Event(0, event.CPUTime, currentTime + 5, event.IOTime, event.job, ++event.iteration));
        }

    }
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check Processor availability
// If there is at least one idle, return true
// else return false
bool checkProcessorsAvailability(int totalProcessors, Processor p[])
{
    for (int i = 0; i < totalProcessors; i++)
    {
        if (p[i].available == true)
        {
            return true;
        }
    }
    return false;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Find index of procesor that is currently idle
// If nothing is available, return -1
int findAvailableProcessor(Processor p[], int totalProcessors)
{
    for (int i = 0; i < totalProcessors; i++)
    {
        if (p[i].available == true)
        {
            return i;
        }
    }
    return -1;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Release the job from current processor once finished
// Mark processor as available
void changeProcessorAvailability(Event e, Processor p[], int totalProcessors)
{
    for (int i = 0; i < totalProcessors; i++)
    {
        if (p[i].JID == e.job.JobId)
        {
            p[i].removeJob();
        }
    }
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Move an event from one queue to RR1 queue
// Event will be moved if a certain age tresshold is met
// Age tresshold varies between queues
void moveQueues(vector<Event> &algo2, int ageVal)
{
    // vector <Event> copy_algo2 = algo2;
    int index = 0;
    while (index < algo2.size())
    {
        if (algo2[index].age >= ageVal)
        {
            algo2[index].iteration = 0;
            algo2[index].age = 0;
            RR1.push_back(algo2[index]);
            algo2.erase(algo2.begin() + index);
            index--;
        }

        index++;
    }
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Move priority queue to RR1 queue
// This function is specifically build to move events from SPN priority queue to RR1 queue
void movePriority()
{
    priority_queue<Event, vector<Event>, sortSPN> copy_SPN;
    priority_queue<Event, vector<Event>, sortSPN> final_SPN;
    copy_SPN = SPN;

    while (!copy_SPN.empty())
    {
        Event test = copy_SPN.top();
        if (test.age > 15)
        {
            test.iteration = 0;
            test.age = 0;
            RR1.push_back(test);
            copy_SPN.pop();
        }
        else
        {
            final_SPN.push(test);
            copy_SPN.pop();
        }
    }
    SPN = final_SPN;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Incremenet the age of all the events in the ready queues
void incrementAge()
{
    for (int i = 0; i < RR1.size(); i++)
    {
        RR1[i].age += 1;
    }
    for (int i = 0; i < RR2.size(); i++)
    {
        RR2[i].age += 1;
    }
    for (int i = 0; i < FCFS.size(); i++)
    {
        FCFS[i].age += 1;
    }

    priority_queue<Event, vector<Event>, sortSPN> copy_SPN;

    while (!SPN.empty())
    {
        Event test = SPN.top();
        test.age += 1;
        copy_SPN.push(test);
        SPN.pop();
    }
    SPN = copy_SPN;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////