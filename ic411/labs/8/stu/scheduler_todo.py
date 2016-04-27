# scheduler_todo.py
# CDR Mike Bilzor, U.S. Naval Academy
# April 2016
# No warranty, no restrictions on use

# Class definition
# This is the data to track as each job runs
class job:

    def __init__ (self, job_id, arrival, service, priority):
        self.job_id = job_id
        self.arrival = arrival
        self.service = service
        self.service_init = service
        self.priority = priority
        self.start = 0
        self.end = 0
        self.wait = 0

    def __str__(self):
        return str(self.job_id) + " " + str(self.arrival) + " " + \
        str(self.service) + " " + str(self.priority)

# Constants
JOB_ID = 0
ARRIVAL = 1
SERVICE = 2
PRIORITY = 3
RR_QUANTUM = 3
LOW_PRIORITY = 11
MAX_SERVICE = 10

schedulers = ["RR","PRI","FCFS","SPN","SRT","HRRN"]
NUM_SCHEDULERS = len(schedulers)

# file_jobs is a 2-D list (array). There is one copy of the list for each scheduler.
#   Each list contains a number of jobs.
#   Each job has a job_id, an arrival_time, service time, and priority

# Read job data from file
file_jobs = []
for i in range(NUM_SCHEDULERS):
    file_jobs.append([])

f = open('jobdata.txt','r')
while True:

    # Read a line
    line = f.readline()
    line_array = line.split()
    if line_array == []:
        break

    # Create a job list array for each scheduler
    for i in range(NUM_SCHEDULERS):

        file_jobs[i].append(job(int(line_array[JOB_ID]), int(line_array[ARRIVAL]), \
        int(line_array[SERVICE]), int(line_array[PRIORITY])))

f.close()

# Run all schedulers
scheduler_index = 0
for scheduler in schedulers:

    # Load jobs data from file to array
    jobs = file_jobs[scheduler_index]

    # Initialize variables
    clock_time = 0
    last_completion = 0
    running_job = None
    job_queue = []        # A FIFO queue. When a job's time arrives, it queues here.
                        #  When pre-empted, a job goes to the end of the queue.
    completed_jobs = []    # A list of completed jobs, so we can calculate statistics.

    while ((jobs != []) or (job_queue != []) or (running_job != None)):

        # The currently executing job, if any, completes another tick of work.
        if running_job != None:
            running_job.service -= 1

        # All other jobs, if any, wait a tick
        for job in job_queue:
            job.wait += 1

        # The currently executing job may finish, in which case we update statistics.
        if running_job != None:
            if running_job.service < 1:
                running_job.end = clock_time
                completed_jobs.append(running_job)
                running_job = None
                last_completion = clock_time

        # A new job may arrive.
        if jobs != []:
            if jobs[0].arrival == clock_time:
                job_queue.append(jobs.pop(0))

        #######################################################################

        # Depending on the scheduler, the currently executing job may change.
        # In each of the following, at a minimum, running_job and the job_queue
        #  must be checked/updated.

        if scheduler == "RR":

            # See if there's a running job. If not, get one from the job queue
            if (running_job == None):
                if job_queue != []:
                    running_job = job_queue.pop(0)

            # If there is a running job, pre-empt it if the quantum has been exceeded
            elif (last_completion == clock_time - RR_QUANTUM): # Pre-emption
                job_queue.append(running_job) # Back to queue
                if job_queue != []:
                    running_job = job_queue.pop(0)

        elif scheduler == "PRI":

            # Find the running job's priority. If None, default to LOW_PRIORITY
            currentPriority = LOW_PRIORITY
            if running_job != None:
                if running_job.priority != None:
                    currentPriority = running_job.priority

            # Find highest pri job in queue
            pos = 0
            highestPos = pos
            if job_queue != []:
                highestPri = job_queue[0].priority
            for job in job_queue:
                if job.priority < highestPri:
                    highestPri = job.priority
                    highestPos = pos
                pos = pos+1

            # If there's a running job, compare priority against best from queue
            #   If the queue max pri job is higher, pre-empt the running job
            if running_job != None:
                if highestPri < running_job.priority:
                    job_queue.append(running_job)
                    running_job = job_queue.pop(highestPos)

            # If no running job, just pick highest-pri job from the queue
            else:
                if job_queue != []:
                    running_job = job_queue.pop(highestPos)

        elif scheduler == "FCFS":

            # If there's no running job, get the first one from the job queue
            if (running_job == None):
                if job_queue != []:
                    running_job = job_queue.pop(0)

        elif scheduler == "SRT":

            # Find running job's service time
            currentService = MAX_SERVICE
            if running_job != None:
                currentService = running_job.service

            # Find shortest service time in queue
            pos = 0
            lowestPos = pos
            lowestServ = MAX_SERVICE
            for job in job_queue:
                if job.service < lowestServ:
                    lowestServ = job.service
                    lowestPos = pos
                pos = pos+1

            # If a job is running, compare
            if running_job != None:
                if lowestServ < running_job.service:
                    job_queue.append(running_job)
                    running_job = job_queue.pop(lowestPos)

            # If not, just pick from queue
            else:
                if job_queue != []:
                    running_job = job_queue.pop(lowestPos)

        elif scheduler == "SPN":

            # If no running job, find shortest service time job in queue
            if running_job == None:
                pos = 0
                shortestPos = pos
                if job_queue != []:
                    shortestSer = job_queue[0].service
                for job in job_queue:
                    if job.service < shortestSer:
                        shortestSer = job.service
                        shortestPos = pos
                    pos = pos+1

                if job_queue != []:
                    running_job = job_queue.pop(shortestPos)

        elif scheduler == "HRRN":

            # Find highest response ratio job in queue
            if (running_job == None):

                queue_max_rr = 0.0
                max_index = 0
                index = 0
                for job in job_queue:
                    job_rr = float((job.wait + job.service_init) / job.service_init)
                    if job_rr > queue_max_rr:
                        max_index = index
                        queue_max_rr = job_rr
                    index += 1

                if job_queue != []:
                    running_job = job_queue.pop(max_index)

        #######################################################################

        # End of scheduler section. Administrative updates follow:

        # New job? If so, update its start time. Welcome aboard, shipmate.
        if running_job != None:
            if running_job.start == 0:
                running_job.start = clock_time

        # Increment the clock
        clock_time += 1

    # End of while loop for current scheduler

    # Print statistics for current scheduler
    job_count = 0.0
    response_total = 0.0
    turnaround_total = 0.0
    for job in completed_jobs:
        job_count += 1.0
        response_total += float(job.start - job.arrival)
        turnaround_total += float(job.end - job.arrival)

    print("\nScheduler: " + scheduler)
    if job_count != 0.0:
        print("Response Time Avg.:\t%1.2f ticks" % (response_total / job_count) )
        print("Turnaround Time Avg.:\t%1.2f ticks" % (turnaround_total / job_count) )
    else:
        print("No jobs completed")

    # Go to next scheduler
    scheduler_index += 1
