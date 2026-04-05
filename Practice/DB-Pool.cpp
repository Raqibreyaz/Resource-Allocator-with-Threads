#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include <memory>


// create DB connection + executes query
class DBConnection {
private:
   int id;
public:
   DBConnection(int id) : id(id) {}
  
   void executeQuery(const std::string& query) {
       std::cout << "Connection [" << id << "] executing: " << query << std::endl;
       
       std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
       // Simulate DB latency
   }
  
   int getId() const { return id; }
  
   std::string toString() const {
       return "DBConnection [id=" + std::to_string(id) + "]";
   }
};


class ConnectionPool {
private:
   std::vector<DBConnection*> available_connections;
public:
   ConnectionPool(int poolSize) {
       std::cout << "Initializing Connection Pool with " << poolSize << " connections." << std::endl;
       for (int i = 0; i < poolSize; ++i) {
           available_connections.push_back(new DBConnection(i + 1));
       }
   }


   DBConnection* acquire() {
       while (available_connections.empty()) {
           // Wait for a connection to become available
       }
       DBConnection* conn = available_connections.back();
       available_connections.pop_back();
       std::cout << "Acquired: " << conn->toString() << " | Pool size: " << available_connections.size() << std::endl;
       return conn;
   }


   void release(DBConnection* conn) {
       available_connections.push_back(conn);
       std::cout << "Released: " << conn->toString() << " | Pool size: " << available_connections.size() << std::endl;
   }
};


class Worker {
private:
   int workerId;
   ConnectionPool& pool;
   std::mt19937 rng;
   std::uniform_int_distribution<int> dist;
public:
   Worker(int id, ConnectionPool& pool)
       : workerId(id), pool(pool), rng(std::random_device()()), dist(50, 200) {}


   void run() {
       try {
           while (true) {
               DBConnection* conn = pool.acquire();
              
               std::cout << "Worker " << workerId << " processing with Connection " << conn->getId() << std::endl;
               conn->executeQuery("SELECT * FROM users WHERE active = 1");
              
               pool.release(conn);
              
               std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng))); // Time between requests
           }
       } catch (const std::exception& e) {
           std::cerr << "Worker " << workerId << " encountered an error: " << e.what() << std::endl;
       }
       std::cout << "Worker " << workerId << " stopping." << std::endl;
   }
};


int main() {
   ConnectionPool pool(3);
   int numWorkers = 5;


   std::vector<std::unique_ptr<Worker>> workerObjects;
   std::vector<std::thread> workerThreads;


   for (int i = 0; i < numWorkers; ++i) {
       workerObjects.push_back(std::make_unique<Worker>(i, pool));
       workerThreads.push_back(std::thread(&Worker::run, std::ref(*workerObjects.back())));
   }


   std::cout << "Starting Application..." << std::endl;
   std::this_thread::sleep_for(std::chrono::seconds(2));
   std::cout << "\nAttempting to shut down Application..." << std::endl;


   for (std::thread& t : workerThreads) {
       if (t.joinable()) t.join();
   }


   std::cout << "Application shut down complete." << std::endl;
   return 0;
}


