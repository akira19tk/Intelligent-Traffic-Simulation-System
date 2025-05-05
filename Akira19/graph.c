#include "graph.h"
#include <string.h>

// ฟังก์ชันสำหรับสร้างกราฟใหม่
Graph* create_graph(int num_vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for graph\n");
        exit(1);
    }
    
    graph->num_vertices = num_vertices;
    graph->vertices = (Vertex*)malloc(num_vertices * sizeof(Vertex));
    if (graph->vertices == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for vertices\n");
        free(graph);
        exit(1);
    }
    
    // เริ่มต้นค่าสำหรับแต่ละจุดยอด
    for (int i = 0; i < num_vertices; i++) {
        graph->vertices[i].id = i;
        graph->vertices[i].name = NULL;
        graph->vertices[i].has_signal = false;
        graph->vertices[i].head = NULL;
    }
    
    return graph;
}

// ฟังก์ชันสำหรับเพิ่มทางแยกใหม่
void add_vertex(Graph* graph, int id, const char* name, bool has_signal) {
    if (id >= 0 && id < graph->num_vertices) {
        graph->vertices[id].id = id;
        
        // จัดสรรและคัดลอกชื่อ
        if (name != NULL) {
            int name_len = strlen(name) + 1;
            graph->vertices[id].name = (char*)malloc(name_len * sizeof(char));
            if (graph->vertices[id].name == NULL) {
                fprintf(stderr, "Error: Unable to allocate memory for vertex name\n");
                return;
            }
            strcpy(graph->vertices[id].name, name);
        }
        
        graph->vertices[id].has_signal = has_signal;
    } else {
        fprintf(stderr, "Error: Invalid vertex ID\n");
    }
}

// ฟังก์ชันสำหรับสร้างถนนใหม่
Road* create_road(int lanes, float length, float speed_limit, int capacity) {
    Road* road = (Road*)malloc(sizeof(Road));
    if (road == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for road\n");
        exit(1);
    }
    
    road->lanes = lanes;
    road->length = length;
    road->speed_limit = speed_limit;
    road->capacity = capacity;
    road->current_load = 0;  // เริ่มต้นไม่มีรถบนถนน
    
    return road;
}

// ฟังก์ชันสำหรับเพิ่มเส้นเชื่อมลงในกราฟ (เพิ่มถนนระหว่างทางแยก)
void add_edge(Graph* graph, int src, int dest, Road* road) {
    if (src < 0 || src >= graph->num_vertices ||
        dest < 0 || dest >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex ID\n");
        return;
    }
    
    // สร้าง Edge ใหม่
    Edge* new_edge = (Edge*)malloc(sizeof(Edge));
    if (new_edge == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for edge\n");
        return;
    }
    
    // คำนวณเวลาการเดินทางเริ่มต้น
    float travel_time = calculate_travel_time(road);
    
    new_edge->dest = dest;
    new_edge->road = road;
    new_edge->weight = travel_time;
    new_edge->next = graph->vertices[src].head;
    graph->vertices[src].head = new_edge;
}

// ฟังก์ชันสำหรับคำนวณเวลาการเดินทางบนถนน
float calculate_travel_time(Road* road) {
    // เวลาการเดินทางพื้นฐาน = ความยาว / ความเร็วจำกัด (ในชั่วโมง)
    float base_time = road->length / road->speed_limit;
    
    // คำนวณความหนาแน่นของการจราจร (0.0 - 1.0)
    float congestion = (float)road->current_load / road->capacity;
    if (congestion > 1.0) congestion = 1.0;  // ป้องกันค่าเกิน
    
    // ปรับเวลาการเดินทางตามความหนาแน่นของการจราจร
    // ใช้ฟังก์ชันแบบไม่เชิงเส้นเพื่อจำลองผลกระทบของความแออัด
    // เมื่อความหนาแน่นเพิ่มขึ้น เวลาการเดินทางจะเพิ่มขึ้นอย่างรวดเร็ว
    float adjusted_time = base_time * (1.0 + 4.0 * (congestion * congestion));
    
    return adjusted_time;
}

// ฟังก์ชันสำหรับอัปเดตน้ำหนักของเส้นเชื่อมตามสภาพการจราจร
void update_edge_weight(Graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        Edge* current = graph->vertices[i].head;
        while (current != NULL) {
            current->weight = calculate_travel_time(current->road);
            current = current->next;
        }
    }
}

// ฟังก์ชันสำหรับแสดงข้อมูลของกราฟ
void print_graph(Graph* graph) {
    printf("Road Network Information:\n");
    printf("Total number of intersections: %d\n", graph->num_vertices);
    
    for (int i = 0; i < graph->num_vertices; i++) {
        printf("\nIntersection ID: %d", i);
        if (graph->vertices[i].name != NULL) {
            printf(" (%s)", graph->vertices[i].name);
        }
        printf("\n");
        printf("  Traffic Signal: %s\n", graph->vertices[i].has_signal ? "Yes" : "No");
        
        Edge* current = graph->vertices[i].head;
        while (current != NULL) {
            printf("  Connected to Intersection ID: %d", current->dest);
            if (graph->vertices[current->dest].name != NULL) {
                printf(" (%s)", graph->vertices[current->dest].name);
            }
            printf("\n");
            printf("    Road: Lanes=%d, Length=%.2f km, Speed Limit=%.2f km/h, Capacity=%d, Current Load=%d\n",
                   current->road->lanes, current->road->length, current->road->speed_limit,
                   current->road->capacity, current->road->current_load);
            printf("    Current Travel Time: %.2f hours\n", current->weight);
            
            current = current->next;
        }
    }
}

// ฟังก์ชันสำหรับลบกราฟและคืนหน่วยความจำ
void free_graph(Graph* graph) {
    if (graph == NULL) return;
    
    // ลบข้อมูลของแต่ละจุดยอด
    for (int i = 0; i < graph->num_vertices; i++) {
        // ลบชื่อ
        if (graph->vertices[i].name != NULL) {
            free(graph->vertices[i].name);
        }
        
        // ลบเส้นเชื่อมทั้งหมด
        Edge* current = graph->vertices[i].head;
        while (current != NULL) {
            Edge* temp = current;
            current = current->next;
            
            // ลบข้อมูลถนน
            if (temp->road != NULL) {
                free(temp->road);
            }
            
            free(temp);
        }
    }
    
    // ลบอาเรย์ของจุดยอด
    free(graph->vertices);
    
    // ลบกราฟ
    free(graph);
}
