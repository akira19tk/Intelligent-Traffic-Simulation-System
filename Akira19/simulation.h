#ifndef SIMULATION_H
#define SIMULATION_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <time.h>
 #include "graph.h"
 #include "queue.h"
 #include "traffic_signal.h"
 #include "route.h"
 
 // โครงสร้างข้อมูลของยานพาหนะ
 typedef struct {
     int id;              // ID ของยานพาหนะ
     int origin;          // จุดต้นทาง
     int destination;     // จุดปลายทาง
     int current_road;    // ถนนที่กำลังเดินทาง (ดัชนีของเส้นเชื่อมในกราฟ)
     int current_pos;     // ตำแหน่งปัจจุบัน (ระยะทางจากจุดเริ่มต้นของถนน)
     float speed;         // ความเร็วปัจจุบัน
     Route* route;        // เส้นทางที่วางแผนไว้
     int route_index;     // ดัชนีปัจจุบันในเส้นทาง
     bool completed;      // เดินทางถึงจุดหมายแล้วหรือไม่
 } Vehicle;
 
 // โครงสร้างข้อมูลของการจำลองระบบการจราจร
 typedef struct {
     Graph* graph;                // เครือข่ายถนน
     SignalSystem* signal_system; // ระบบสัญญาณไฟจราจร
     Vehicle* vehicles;           // อาเรย์ของยานพาหนะ
     int num_vehicles;            // จำนวนยานพาหนะทั้งหมด
     int max_vehicles;            // จำนวนยานพาหนะสูงสุดที่รองรับ
     int time_step;               // ขั้นตอนเวลาปัจจุบัน (วินาที)
     bool is_running;             // การจำลองกำลังทำงานหรือไม่
 } TrafficSimulation;
 
 // ฟังก์ชันสำหรับสร้างการจำลองระบบการจราจรใหม่
 TrafficSimulation* create_simulation(Graph* graph, SignalSystem* signal_system, int max_vehicles);
 
 // ฟังก์ชันสำหรับสร้างยานพาหนะใหม่ในการจำลอง
 int add_vehicle(TrafficSimulation* sim, int origin, int destination);
 
 // ฟังก์ชันสำหรับอัปเดตตำแหน่งของยานพาหนะ
 void update_vehicle(TrafficSimulation* sim, int vehicle_id);
 
 // ฟังก์ชันสำหรับอัปเดตสถานะของการจำลองในแต่ละขั้นตอนเวลา
 void update_simulation(TrafficSimulation* sim);
 
 // ฟังก์ชันสำหรับเริ่มการจำลอง
 void start_simulation(TrafficSimulation* sim);
 
 // ฟังก์ชันสำหรับหยุดการจำลอง
 void stop_simulation(TrafficSimulation* sim);
 
 // ฟังก์ชันสำหรับสร้างการจราจรแบบสุ่ม
 void generate_random_traffic(TrafficSimulation* sim, int num_vehicles);
 
 // ฟังก์ชันสำหรับวิเคราะห์ผลการจำลอง
 void analyze_simulation_results(TrafficSimulation* sim);
 
 // ฟังก์ชันสำหรับแสดงข้อมูลของการจำลอง
 void print_simulation_status(TrafficSimulation* sim);
 
 // ฟังก์ชันสำหรับลบการจำลองและคืนหน่วยความจำ
 void free_simulation(TrafficSimulation* sim);

 void simulate_morning_traffic(TrafficSimulation* sim);
 
 #endif
