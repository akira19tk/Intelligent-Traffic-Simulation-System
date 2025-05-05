#include "queue.h"

// ฟังก์ชันสำหรับสร้างคิวใหม่
PriorityQueue* create_priority_queue(void) {
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (queue == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for queue\n");
        exit(1);
    }
    
    queue->head = NULL;
    queue->size = 0;
    
    return queue;
}

// ฟังก์ชันสำหรับตรวจสอบว่าคิวว่างหรือไม่
bool is_queue_empty(PriorityQueue* queue) {
    return (queue->head == NULL);
}

// ฟังก์ชันสำหรับเพิ่มสมาชิกใหม่ลงในคิว
void enqueue(PriorityQueue* queue, int junction_id, float priority) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for queue node\n");
        return;
    }
    
    new_node->junction_id = junction_id;
    new_node->priority = priority;
    
    if (is_queue_empty(queue) || priority > queue->head->priority) {
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        QueueNode* current = queue->head;
        while (current->next != NULL && current->next->priority >= priority) {
            current = current->next;
        }
        
        new_node->next = current->next;
        current->next = new_node;
    }
    
    queue->size++;
}

// ฟังก์ชันสำหรับลบสมาชิกที่มีความสำคัญสูงสุดออกจากคิว
int dequeue(PriorityQueue* queue) {
    if (is_queue_empty(queue)) {
        fprintf(stderr, "Error: Cannot remove node from empty queue\n");
        return -1;
    }
    
    QueueNode* temp = queue->head;
    int junction_id = temp->junction_id;
    
    queue->head = queue->head->next;
    free(temp);
    
    queue->size--;
    
    return junction_id;
}

// ฟังก์ชันสำหรับปรับค่าความสำคัญของสมาชิกในคิว
void update_priority(PriorityQueue* queue, int junction_id, float new_priority) {
    QueueNode* current = queue->head;
    bool found = false;
    
    while (current != NULL) {
        if (current->junction_id == junction_id) {
            found = true;
            break;
        }
        current = current->next;
    }
    
    if (found) {
        if (queue->head->junction_id == junction_id) {
            QueueNode* temp = queue->head;
            queue->head = queue->head->next;
            free(temp);
            queue->size--;
        } else {
            QueueNode* prev = queue->head;
            current = queue->head->next;
            
            while (current != NULL && current->junction_id != junction_id) {
                prev = current;
                current = current->next;
            }
            
            if (current != NULL) {
                prev->next = current->next;
                free(current);
                queue->size--;
            }
        }
        
        enqueue(queue, junction_id, new_priority);
    } else {
        enqueue(queue, junction_id, new_priority);
    }
}

// ฟังก์ชันสำหรับแสดงข้อมูลของคิว
void print_queue(PriorityQueue* queue) {
    if (is_queue_empty(queue)) {
        printf("Queue is empty\n");
        return;
    }
    
    printf("Priority Queue (Number of nodes: %d):\n", queue->size);
    
    QueueNode* current = queue->head;
    int position = 1;
    
    while (current != NULL) {
        printf("%d. Junction ID: %d, Priority: %.2f\n",
               position, current->junction_id, current->priority);
        
        current = current->next;
        position++;
    }
}

// ฟังก์ชันสำหรับลบคิวและคืนหน่วยความจำ
void free_queue(PriorityQueue* queue) {
    if (queue == NULL) return;
    
    QueueNode* current = queue->head;
    while (current != NULL) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    
    free(queue);
}
