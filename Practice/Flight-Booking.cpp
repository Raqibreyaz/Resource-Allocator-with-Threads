#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>
#include <map>
#include <memory>


class Flight {
public:
   int flightNumber;
   atomic<int> availableSeats;
   std::mutex mtx;


   Flight(int num, int seats) : flightNumber(num), availableSeats(seats) {}
};


class ReservationSystem {
private:
   std::map<int, std::shared_ptr<Flight>> flights;


public:
   void addFlight(int num, int seats) {
       flights[num] = std::make_shared<Flight>(num, seats);
   }


   bool bookSingleFlight(int flightNum) {
       // Find the flight
       //locked mutex
       auto flight = flights[flightNum];
      
       if (flight->availableSeats > 0) {
           std::this_thread::sleep_for(std::chrono::milliseconds(2)); // Simulate DB latency
           flight->availableSeats--;
           std::cout << "Booked single flight " << flightNum << ". Seats left: " << flight->availableSeats << std::endl;
           return true;
       }
       return false;
   }


   bool bookConnectingFlight(int firstFlightNum, int secondFlightNum) {
       auto flight1 = flights[firstFlightNum];
       auto flight2 = flights[secondFlightNum];


       // Lock both flights to ensure we can book both or neither DEADlock
       std::lock_guard<std::mutex> lock1(flight1->mtx);
       std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate DB latency
       std::lock_guard<std::mutex> lock2(flight2->mtx);


       if (flight1->availableSeats > 0 && flight2->availableSeats > 0) {
           flight1->availableSeats--;
           flight2->availableSeats--;
          
           // Log success
           std::cout << "Successfully booked connecting flights "
                     << firstFlightNum << " and " << secondFlightNum << std::endl;
           return true;
       }
       return false;
   }
};


void userThreadA(ReservationSystem& sys) {
   for (int i = 0; i < 5; ++i) {
       sys.bookConnectingFlight(101, 102);
   }
}


void userThreadB(ReservationSystem& sys) {
   for (int i = 0; i < 5; ++i) {
       sys.bookConnectingFlight(102, 101);
   }
}


void userThreadC(ReservationSystem sys) {
   for (int i = 0; i < 10; ++i) {
       sys.bookSingleFlight(101);
   }
}


int main() {
   ReservationSystem sys;
   sys.addFlight(101, 20);
   sys.addFlight(102, 20);


   std::cout << "Starting Flash Sale..." << std::endl;


   std::thread t1(userThreadA, std::ref(sys));
   std::thread t2(userThreadB, std::ref(sys));
   std::thread t3(userThreadC, sys);


   t1.join();
   t2.join();
   t3.join();


   std::cout << "Flash Sale Ended." << std::endl;
   return 0;
}
    