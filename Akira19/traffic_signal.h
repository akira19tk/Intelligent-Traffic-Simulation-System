#ifndef TRAFFIC_SIGNAL_H
#define TRAFFIC_SIGNAL_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include "graph.h"
 #include "queue.h"
 
 // สถานะของสัญญาณไฟจราจร
 typedef enum {
     RED,
     GREEN,
     YELLOW
 } SignalState;
 
 // โครงสร้างข้อมูลของเฟสสัญญาณไฟจราจร
 typedef struct {
     int direction;       // ทิศทาง (0 = เหนือ, 1 = ตะวันออก, 2 = ใต้, 3 = ตะวันตก)
     SignalState state;   // สถานะปัจจุบัน
     int duration;        // ระยะเวลา (วินาที)
     int remaining_time;  // เวลาที่เหลือ (วินาที)
 } SignalPhase;
 
 // โครงสร้างข้อมูลของสัญญาณไฟจราจรที่ทางแยก
 typedef struct {
     int junction_id;     // ID ของทางแยก
     int num_phases;      // จำนวนเฟส
     SignalPhase* phases; // อาเรย์ของเฟส
     int current_phase;   // เฟสปัจจุบัน
     bool is_adaptive;    // เป็นระบบปรับตัวหรือไม่
 } TrafficSignal;
 
 // โครงสร้างข้อมูลของระบบสัญญาณไฟจราจร
 typedef struct {
     int num_signals;         // จำนวนสัญญาณไฟจราจร
     TrafficSignal* signals;  // อาเรย์ของสัญญาณไฟจราจร
     PriorityQueue* queue;    // คิวสำหรับจัดลำดับความสำคัญ
 } SignalSystem;
 
 // ฟังก์ชันสำหรับสร้างระบบสัญญาณไฟจราจรใหม่
 SignalSystem* create_signal_system(Graph* graph);
 
 // ฟังก์ชันสำหรับสร้างสัญญาณไฟจราจรที่ทางแยก
 TrafficSignal* create_traffic_signal(int junction_id, int num_phases, bool is_adaptive);
 
 // ฟังก์ชันสำหรับกำหนดค่าเฟสของสัญญาณไฟจราจร
 void set_signal_phase(TrafficSignal* signal, int phase_index, int direction, SignalState state, int duration);
 
 // ฟังก์ชันสำหรับอัปเดตสัญญาณไฟจราจร (ลดเวลาที่เหลือและเปลี่ยนเฟส)
 void update_traffic_signal(TrafficSignal* signal);
 
 // ฟังก์ชันสำหรับคำนวณความหนาแน่นของการจราจรที่ทางแยก
 float calculate_junction_congestion(Graph* graph, int junction_id);
 
 // ฟังก์ชันสำหรับปรับระยะเวลาของเฟสตามความหนาแน่นของการจราจร
 void adjust_signal_timing(Graph* graph, TrafficSignal* signal);
 
 // ฟังก์ชันสำหรับจัดการคิวสัญญาณไฟจราจรอัจฉริยะ
 void manage_signal_queue(Graph* graph, SignalSystem* system);
 
 // ฟังก์ชันสำหรับอัปเดตระบบสัญญาณไฟจราจรทั้งหมด
 void update_signal_system(Graph* graph, SignalSystem* system);
 
 // ฟังก์ชันสำหรับแสดงข้อมูลของสัญญาณไฟจราจร
 void print_traffic_signal(TrafficSignal* signal);
 
 // ฟังก์ชันสำหรับแสดงข้อมูลของระบบสัญญาณไฟจราจร
 void print_signal_system(SignalSystem* system);
 
 // ฟังก์ชันสำหรับลบสัญญาณไฟจราจรและคืนหน่วยความจำ
 void free_traffic_signal(TrafficSignal* signal);
 
 // ฟังก์ชันสำหรับลบระบบสัญญาณไฟจราจรและคืนหน่วยความจำ
 void free_signal_system(SignalSystem* system);
 
 #endif
