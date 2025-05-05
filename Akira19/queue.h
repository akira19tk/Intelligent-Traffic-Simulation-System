#ifndef QUEUE_H
#define QUEUE_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 
 // โครงสร้างข้อมูลของสมาชิกในคิว
 typedef struct QueueNode {
     int junction_id;        // ID ของทางแยก
     float priority;         // ค่าความสำคัญ (ความหนาแน่นของการจราจร)
     struct QueueNode* next; // ชี้ไปยังสมาชิกถัดไป
 } QueueNode;
 
 // โครงสร้างข้อมูลของคิวที่มีการจัดลำดับความสำคัญ
 typedef struct {
     QueueNode* head;        // ชี้ไปยังสมาชิกแรกของคิว
     int size;              // จำนวนสมาชิกในคิว
 } PriorityQueue;
 
 // ฟังก์ชันสำหรับสร้างคิวใหม่
 PriorityQueue* create_priority_queue();
 
 // ฟังก์ชันสำหรับตรวจสอบว่าคิวว่างหรือไม่
 bool is_queue_empty(PriorityQueue* queue);
 
 // ฟังก์ชันสำหรับเพิ่มสมาชิกใหม่ลงในคิว
 void enqueue(PriorityQueue* queue, int junction_id, float priority);
 
 // ฟังก์ชันสำหรับลบสมาชิกที่มีความสำคัญสูงสุดออกจากคิว
 int dequeue(PriorityQueue* queue);
 
 // ฟังก์ชันสำหรับปรับค่าความสำคัญของสมาชิกในคิว
 void update_priority(PriorityQueue* queue, int junction_id, float new_priority);
 
 // ฟังก์ชันสำหรับแสดงข้อมูลของคิว
 void print_queue(PriorityQueue* queue);
 
 // ฟังก์ชันสำหรับลบคิวและคืนหน่วยความจำ
 void free_queue(PriorityQueue* queue);
 
 #endif
