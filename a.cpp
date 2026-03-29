#include <bits/stdc++.h>
using namespace std;

/*
    Bathroom Scheduling Problem
    - Capacity = 3
    - No mixing (only D or R at a time)
    - No preemption
    - Minimize waiting time + avoid starvation
*/

enum Party { DEMOCRAT, REPUBLICAN };
enum EventType { ARRIVAL, DEPARTURE };
enum State { EMPTY, D_OCCUPIED, R_OCCUPIED };

struct Person {
    string name;
    Party party;
    int arrival_time;
    int service_time;
    int id; // unique id for debugging
};

struct Event {
    int time;
    EventType type;
    Person person;

    bool operator>(const Event &other) const {
        if (time != other.time) return time > other.time;
        return type > other.type; // ARRIVAL before DEPARTURE if same time
    }
};

class BathroomScheduler {
private:
    queue<Person> QD, QR;

    priority_queue<Event, vector<Event>, greater<Event>> eventPQ;

    State current_state = EMPTY;
    int occupants = 0;
    int current_time = 0;

    const int CAPACITY = 3;
    const int AGING_THRESHOLD = 5; // tunable

public:
    void add_person(const Person &p) {
        eventPQ.push({p.arrival_time, ARRIVAL, p});
    }

    void run() {
        while (!eventPQ.empty()) {
            Event e = eventPQ.top();
            eventPQ.pop();

            current_time = e.time;

            if (e.type == ARRIVAL) {
                handle_arrival(e.person);
            } else {
                handle_departure(e.person);
            }
        }
    }

private:

    void handle_arrival(const Person &p) {
        if (p.party == DEMOCRAT) {
            QD.push(p);
        } else {
            QR.push(p);
        }

        try_admit();
    }

    void handle_departure(const Person &p) {
        occupants--;

        if (occupants == 0) {
            current_state = EMPTY;
        }

        try_admit();
    }

    void try_admit() {
        if (occupants == CAPACITY) return;

        if (current_state == EMPTY) {
            current_state = choose_party();
            if (current_state == EMPTY) return;
        }

        queue<Person> &Q = (current_state == D_OCCUPIED) ? QD : QR;

        while (occupants < CAPACITY && !Q.empty()) {
            Person p = Q.front();
            Q.pop();

            admit_person(p);
        }
    }

    void admit_person(const Person &p) {
        occupants++;

        int departure_time = current_time + p.service_time;

        eventPQ.push({departure_time, DEPARTURE, p});

        // Debug / Trace
        cout << "Time " << current_time
             << ": Admit " << p.name
             << " (" << (p.party == DEMOCRAT ? "D" : "R") << ")"
             << " -> leaves at " << departure_time << "\n";
    }

    State choose_party() {
        if (QD.empty() && QR.empty()) return EMPTY;
        if (QD.empty()) return R_OCCUPIED;
        if (QR.empty()) return D_OCCUPIED;

        int wait_D = current_time - QD.front().arrival_time;
        int wait_R = current_time - QR.front().arrival_time;

        if (wait_D > wait_R + AGING_THRESHOLD) {
            return D_OCCUPIED;
        } else if (wait_R > wait_D + AGING_THRESHOLD) {
            return R_OCCUPIED;
        } else {
            // Tie-breaker: larger queue
            if (QD.size() >= QR.size()) return D_OCCUPIED;
            else return R_OCCUPIED;
        }
    }
};


// Example deterministic service time function
int service_time_fn(const string &name) {
    int sum = 0;
    for (char c : name) sum += c;
    return (sum % 5) + 1; // 1 to 5 seconds
}


// -------------------- DRIVER --------------------

int main() {
    BathroomScheduler scheduler;

    vector<Person> people = {
        {"Alice", DEMOCRAT, 0, service_time_fn("Alice"), 1},
        {"Bob", REPUBLICAN, 1, service_time_fn("Bob"), 2},
        {"Charlie", DEMOCRAT, 2, service_time_fn("Charlie"), 3},
        {"David", REPUBLICAN, 3, service_time_fn("David"), 4},
        {"Eve", DEMOCRAT, 4, service_time_fn("Eve"), 5},
        {"Frank", REPUBLICAN, 5, service_time_fn("Frank"), 6},
        {"Grace", DEMOCRAT, 6, service_time_fn("Grace"), 7}
    };

    for (auto &p : people) {
        scheduler.add_person(p);
    }

    scheduler.run();

    return 0;
}