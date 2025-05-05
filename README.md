# Intelligent Traffic Simulation System

This project is an intelligent traffic simulation system developed in C language that can analyze and manage traffic flow to find appropriate methods for reducing urban traffic congestion.

## Problem to Solve
Heavy traffic in large cities is a problem that affects people's quality of life, wastes energy, and increases air pollution. Traditional traffic systems cannot adapt to changing traffic conditions in a timely manner. This project aims to develop an intelligent traffic simulation system that can analyze and manage traffic flow to find appropriate methods for reducing urban traffic congestion.

## Functions
1. Road Network Simulation
   * Create and manage graphs representing city roads and intersections
   * Define road characteristics such as capacity, speed limits, and number of lanes

2. Traffic Signal Management
   * Intelligent queue system for controlling traffic lights based on vehicle density
   * Adjust traffic light timing according to real-time traffic conditions

3. Optimal Route Finding
   * Use Dijkstra's algorithm to calculate the shortest path
   * Consider multiple factors such as distance, time, and traffic density

## Technologies Used
1. Development Language
   * C language for the entire system development
   * Use Structs and Pointers for efficient data structure management

2. Data Structures
   * Graph (Adjacency List) for representing the road network
   * Queue (Priority Queue) for traffic signal management
   * Heap for Dijkstra's algorithm in finding the shortest path

## File Structure
* **graph.h / graph.c**: Graph data structure for representing the road network
* **queue.h / queue.c**: Priority queue data structure
* **traffic_signal.h / traffic_signal.c**: Traffic light signal management
* **route.h / route.c**: Finding optimal routes
* **simulation.h / simulation.c**: Traffic system simulation
* **main.c**: Program entry point
