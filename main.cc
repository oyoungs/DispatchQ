#include "DispatchQueue/DispatchQueue.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Testing Serial Worker ===" << std::endl;
    {
        dispatch::DispatchQueue queue(dispatch::DispatchStrategy::Serial);
        
        for (int i = 0; i < 3; ++i) {
            queue.Push([i]() {
                std::cout << "Serial Task " << i << " executed\n";
            });
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "\n=== Testing Dynamic Thread Pool ===" << std::endl;
    {
        dispatch::DispatchQueue queue(dispatch::DispatchStrategy::Concurrent);
        
        for (int i = 0; i < 10; ++i) {
            queue.Push([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::cout << "Task " << i << " executed\n";
            });
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "\n=== Testing Stealing Thread Pool ===" << std::endl;
    {
        dispatch::DispatchQueue queue(dispatch::DispatchStrategy::Stealing);
        
        for (int i = 0; i < 10; ++i) {
            queue.Push([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::cout << "Stealing Task " << i << " executed\n";
            });
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "\n=== Testing Bounded Thread Pool ===" << std::endl;
    {
        dispatch::DispatchQueue queue(dispatch::DispatchStrategy::Bounded);
        
        for (int i = 0; i < 10; ++i) {
            queue.Push([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                std::cout << "Bounded Task " << i << " executed\n";
            });
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "\nAll tasks completed\n";
    
    return 0;
}