#ifndef ROUTE_H
#define ROUTE_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <float.h>
 #include "graph.h"
 
 // โครงสร้างข้อมูลของเส้นทาง
 typedef struct {
     int* path;          // อาเรย์ของจุดยอดในเส้นทาง
     int length;         // จำนวนจุดยอดในเส้นทาง
     float total_time;   // เวลาการเดินทางทั้งหมด
     float total_distance; // ระยะทางทั้งหมด
 } Route;
 
 // ฟังก์ชันสำหรับสร้างเส้นทางใหม่
 Route* create_route(int capacity);
 
 // ฟังก์ชันสำหรับสร้างเส้นทางโดยใช้อัลกอริทึม Dijkstra
 Route* find_shortest_path(Graph* graph, int src, int dest);
 
 // ฟังก์ชันสำหรับค้นหาเส้นทางที่มีการจราจรน้อยที่สุด
 Route* find_least_congested_path(Graph* graph, int src, int dest);
 
 // ฟังก์ชันสำหรับค้นหาเส้นทางที่ใช้เวลาน้อยที่สุด
 Route* find_fastest_path(Graph* graph, int src, int dest);
 
 // ฟังก์ชันสำหรับค้นหาเส้นทางที่ดีที่สุดโดยพิจารณาหลายปัจจัย
 Route* find_optimal_path(Graph* graph, int src, int dest, float time_weight, float distance_weight, float congestion_weight);
 
 // ฟังก์ชันสำหรับคำนวณเวลาการเดินทางของเส้นทาง
 float calculate_route_time(Graph* graph, Route* route);
 
 // ฟังก์ชันสำหรับคำนวณระยะทางของเส้นทาง
 float calculate_route_distance(Graph* graph, Route* route);
 
 // ฟังก์ชันสำหรับแสดงข้อมูลของเส้นทาง
 void print_route(Graph* graph, Route* route);
 
 // ฟังก์ชันสำหรับเปรียบเทียบเส้นทาง
 void compare_routes(Graph* graph, Route* route1, Route* route2);
 
 // ฟังก์ชันสำหรับลบเส้นทางและคืนหน่วยความจำ
 void free_route(Route* route);
 
 #endif
