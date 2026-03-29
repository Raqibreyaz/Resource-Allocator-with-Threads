#include <algorithm>
#include <chrono>
#include <climits>
#include <cstddef>
#include <deque>
#include <iostream>
#include <ostream>
#include <thread>
#include <utility>
#include <vector>

/*

CPU Scheduling:
- round robin
- shortest job first
- first come first serve
- last come first serve

- 

*/

const int BATHROOM_CAPACITY = 3;
enum class Group { D, R, X };

using namespace std::chrono;

typedef struct {
  system_clock::time_point arrival_time;
  time_t burst_time;
} ArrivalAndBurstTime;

/*
f(name) -> time the person will take in the bathroom
so we need to remove that person when that time reaches
=> people_in_bathroom--, if(people_in_bathroom == 0) group_in_bathroom =
Group::X

1. we also have to check if people are done and then decrease the counter + if
possible update the current group

(Now - Arrival_Time) >= Burst_Time
=> remove that person
*/

const std::vector<std::pair<std::string, Group>> people = {{"A", Group::X},
                                                           {"R", Group::D}};

int main() {
  Group group_in_bathroom = Group::X;

  std::deque<ArrivalAndBurstTime> bathroom_people;
  std::deque<std::pair<std::size_t, Group>> queueD;
  std::deque<std::pair<std::size_t, Group>> queueR;

  const auto allocate_bathroom = [&](const std::string &person,
                                     const Group group) -> void {
    // if not people in bathroom then allocate bathroom
    if (bathroom_people.size() == 0) {
      group_in_bathroom = group;
    }

    // if people are there, allocate only when user group matches + there is
    // room in the bathroom
    if (group_in_bathroom == group &&
        bathroom_people.size() < BATHROOM_CAPACITY) {

      auto arrival_time = system_clock::now();
      auto burst_time = static_cast<long>(person.size());

      bathroom_people.push_back(
          {.arrival_time = arrival_time, .burst_time = burst_time});
    }

    // or just push the person in the queue
    else {
      if (group == Group::D) {
        queueD.push_back({person.size(), group});
      } else {
        queueR.push_back({person.size(), group});
      }
    }
  };

  const auto allocate_bathroom_with_time = [&](const time_t burst_time,
                                               const Group group) -> void {
    // if not people in bathroom then allocate bathroom
    if (bathroom_people.size() == 0) {
      group_in_bathroom = group;
    }

    // if people are there, allocate only when user group matches + there is
    // room in the bathroom
    if (group_in_bathroom == group &&
        bathroom_people.size() < BATHROOM_CAPACITY) {

      auto arrival_time = system_clock::now();

      bathroom_people.push_back(
          {.arrival_time = arrival_time, .burst_time = burst_time});
    }

    // or just push the person in the queue
    else {
      if (group == Group::D) {
        queueD.push_back({burst_time, group});
      } else {
        queueR.push_back({burst_time, group});
      }
    }
  };

  const auto add_people_to_bathroom = [&]() {
    if (group_in_bathroom == Group::D) {
      for (const auto &person : queueD) {
        allocate_bathroom_with_time(person.first, person.second);
      }
    } else {
      for (const auto &person : queueR) {
        allocate_bathroom_with_time(person.first, person.second);
      }
    }
  };

  const auto remove_people_from_bathroom = [&]() -> void {
    while (true) {

      long min_time = LONG_MAX;
      for (size_t i = 0; i < bathroom_people.size(); i++) {
        const auto now = system_clock::now();
        const auto arrival_time = bathroom_people[i].arrival_time;
        const auto burst_time = bathroom_people[i].burst_time;

        const long timediff =
            duration_cast<std::chrono::minutes>(now - arrival_time).count();

        min_time = std::min(min_time, burst_time - timediff);
        std::cout << timediff << std::endl;

        // if the person consumed its time then remove him
        if (timediff >= burst_time) {
          bathroom_people.erase(bathroom_people.begin() + i);
        }
      }

      std::cout << min_time << std::endl;
      if (min_time != LONG_MAX)
        std::this_thread::sleep_for(std::chrono::seconds(min_time));
    }
  };

  for (const auto &person : people) {
    allocate_bathroom(person.first, person.second);
  }

  std::thread t(remove_people_from_bathroom);
  t.join();

  // const auto now = system_clock::now();
  // const auto x_minutes_earlier = now - std::chrono::minutes(20);

  // const long now_in_minutes =
  //     duration_cast<std::chrono::minutes>(now.time_since_epoch()).count();

  // const auto x_minutes_earlier_in_minutes =
  //     duration_cast<std::chrono::minutes>(x_minutes_earlier.time_since_epoch())
  //         .count();

  // std::cout << now_in_minutes << std::endl
  //           << x_minutes_earlier_in_minutes << std::endl
  //           << now_in_minutes - x_minutes_earlier_in_minutes << std::endl;

  return 0;
}