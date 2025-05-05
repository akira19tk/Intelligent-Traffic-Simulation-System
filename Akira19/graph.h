#ifndef GRAPH_H
#define GRAPH_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 
 // โครงสร้างข้อมูลสำหรับเก็บข้อมูลของถนน
 typedef struct {
     int lanes;          // จำนวนช่องทาง
     float length;       // ความยาวของถนน (กิโลเมตร)
     float speed_limit;  // ความเร็วจำกัด (กม./ชม.)
     int capacity;       // ความจุของถนน (จำนวนรถสูงสุด)
     int current_load;   // จำนวนรถปัจจุบันบนถนน
 } Road;
 
 // โครงสร้างข้อมูลสำหรับเก็บข้อมูลของเส้นเชื่อมในกราฟ
 typedef struct Edge {
     int dest;           // ปลายทางของเส้นเชื่อม (ทางแยก)
     Road* road;         // ข้อมูลของถนน
     float weight;       // น้ำหนักของเส้นเชื่อม (เวลาในการเดินทาง)
     struct Edge* next;  // ชี้ไปยังเส้นเชื่อมถัดไป
 } Edge;
 
 // โครงสร้างข้อมูลสำหรับเก็บข้อมูลของจุดยอดในกราฟ (ทางแยก)
 typedef struct {
     int id;             // รหัสของทางแยก
     char* name;         // ชื่อของทางแยก
     bool has_signal;    // มีสัญญาณไฟจราจรหรือไม่
     Edge* head;         // ชี้ไปยังเส้นเชื่อมแรก
 } Vertex;
 
 // โครงสร้างข้อมูลสำหรับเก็บข้อมูลของกราฟ
 typedef struct {
     int num_vertices;   // จำนวนจุดยอด (ทางแยก)
     Vertex* vertices;   // อาเรย์ของจุดยอด
 } Graph;
 
 // ฟังก์ชันสำหรับสร้างกราฟใหม่
 Graph* create_graph(int num_vertices);
 
 // ฟังก์ชันสำหรับเพิ่มเส้นเชื่อมลงในกราฟ (เพิ่มถนนระหว่างทางแยก)
 void add_edge(Graph* graph, int src, int dest, Road* road);
 
 // ฟังก์ชันสำหรับเพิ่มทางแยกใหม่
 void add_vertex(Graph* graph, int id, const char* name, bool has_signal);
 
 // ฟังก์ชันสำหรับสร้างถนนใหม่
 Road* create_road(int lanes, float length, float speed_limit, int capacity);
 
 // ฟังก์ชันสำหรับคำนวณเวลาการเดินทางบนถนน
 float calculate_travel_time(Road* road);
 
 // ฟังก์ชันสำหรับอัปเดตน้ำหนักของเส้นเชื่อมตามสภาพการจราจร
 void update_edge_weight(Graph* graph);
 
 // ฟังก์ชันสำหรับแสดงข้อมูลของกราฟ
 void print_graph(Graph* graph);
 
 // ฟังก์ชันสำหรับลบกราฟและคืนหน่วยความจำ
 void free_graph(Graph* graph);

 Graph* create_sample_network();

 
 #endif
