#include "route.h"

// ฟังก์ชันสำหรับสร้างเส้นทางใหม่
Route* create_route(int capacity) {
    Route* route = (Route*)malloc(sizeof(Route));
    if (route == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for route\n");
        exit(1);
    }
    
    route->path = (int*)malloc(capacity * sizeof(int));
    if (route->path == NULL && capacity > 0) {
        fprintf(stderr, "Error: Unable to allocate memory for route\n");
        free(route);
        exit(1);
    }
    
    route->length = 0;
    route->total_time = 0.0;
    route->total_distance = 0.0;
    
    return route;
}

// ฟังก์ชันสำหรับคำนวณเวลาการเดินทางของเส้นทาง
float calculate_route_time(Graph* graph, Route* route) {
    if (route->length <= 1) {
        return 0.0;
    }
    
    float total_time = 0.0;
    
    for (int i = 0; i < route->length - 1; i++) {
        int src = route->path[i];
        int dest = route->path[i + 1];
        
        Edge* current = graph->vertices[src].head;
        while (current != NULL) {
            if (current->dest == dest) {
                total_time += current->weight;
                break;
            }
            current = current->next;
        }
    }
    
    return total_time;
}

// ฟังก์ชันสำหรับคำนวณระยะทางของเส้นทาง
float calculate_route_distance(Graph* graph, Route* route) {
    if (route->length <= 1) {
        return 0.0;
    }
    
    float total_distance = 0.0;
    
    for (int i = 0; i < route->length - 1; i++) {
        int src = route->path[i];
        int dest = route->path[i + 1];
        
        Edge* current = graph->vertices[src].head;
        while (current != NULL) {
            if (current->dest == dest) {
                total_distance += current->road->length;
                break;
            }
            current = current->next;
        }
    }
    
    return total_distance;
}

// ฟังก์ชันเปรียบเทียบสำหรับการจัดลำดับความสำคัญในฮีป
// ใช้สำหรับอัลกอริทึม Dijkstra
typedef struct {
    int vertex;     // จุดยอด
    float dist;     // ระยะทาง/เวลาจากจุดเริ่มต้น
    float priority; // ค่าฮิวริสติก
} HeapNode;

// ฟังก์ชันสำหรับสร้างฮีปใหม่
typedef struct {
    HeapNode* array;  // อาเรย์ของสมาชิกในฮีป
    int capacity;     // ความจุของฮีป
    int size;         // จำนวนสมาชิกในฮีปปัจจุบัน
} MinHeap;

// ฟังก์ชันสำหรับสร้างฮีปใหม่
MinHeap* create_min_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (heap == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for heap\n");
        exit(1);
    }
    
    heap->array = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    if (heap->array == NULL && capacity > 0) {
        fprintf(stderr, "Error: Unable to allocate memory for heap array\n");
        free(heap);
        exit(1);
    }
    
    heap->capacity = capacity;
    heap->size = 0;
    
    return heap;
}

// ฟังก์ชันสำหรับสลับตำแหน่งของสมาชิกในฮีป
void swap_heap_nodes(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

// ฟังก์ชันสำหรับเลื่อนสมาชิกขึ้นในฮีป
void heapify_up(MinHeap* heap, int idx) {
    int parent = (idx - 1) / 2;
    
    if (idx > 0 && heap->array[parent].priority > heap->array[idx].priority) {
        swap_heap_nodes(&heap->array[parent], &heap->array[idx]);
        heapify_up(heap, parent);
    }
}

// ฟังก์ชันสำหรับเลื่อนสมาชิกลงในฮีป
void heapify_down(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < heap->size && heap->array[left].priority < heap->array[smallest].priority) {
        smallest = left;
    }
    
    if (right < heap->size && heap->array[right].priority < heap->array[smallest].priority) {
        smallest = right;
    }
    
    if (smallest != idx) {
        swap_heap_nodes(&heap->array[smallest], &heap->array[idx]);
        heapify_down(heap, smallest);
    }
}

// ฟังก์ชันสำหรับเพิ่มสมาชิกใหม่ลงในฮีป
void insert_min_heap(MinHeap* heap, int vertex, float dist, float priority) {
    if (heap->size >= heap->capacity) {
        fprintf(stderr, "Error: Heap is full\n");
        return;
    }
    
    heap->array[heap->size].vertex = vertex;
    heap->array[heap->size].dist = dist;
    heap->array[heap->size].priority = priority;
    
    heap->size++;
    heapify_up(heap, heap->size - 1);
}

// ฟังก์ชันสำหรับลบสมาชิกที่มีค่าน้อยที่สุดออกจากฮีป
HeapNode extract_min(MinHeap* heap) {
    if (heap->size <= 0) {
        fprintf(stderr, "Error: Heap is empty\n");
        HeapNode empty = {-1, FLT_MAX, FLT_MAX};
        return empty;
    }
    
    HeapNode min = heap->array[0];
    
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    
    heapify_down(heap, 0);
    
    return min;
}

// ฟังก์ชันสำหรับตรวจสอบว่าจุดยอดอยู่ในฮีปหรือไม่
bool is_in_heap(MinHeap* heap, int vertex) {
    for (int i = 0; i < heap->size; i++) {
        if (heap->array[i].vertex == vertex) {
            return true;
        }
    }
    return false;
}

// ฟังก์ชันสำหรับอัพเดตค่าระยะทาง/เวลาของจุดยอดในฮีป
void update_dist_in_heap(MinHeap* heap, int vertex, float dist, float priority) {
    for (int i = 0; i < heap->size; i++) {
        if (heap->array[i].vertex == vertex) {
            heap->array[i].dist = dist;
            heap->array[i].priority = priority;
            
            heapify_up(heap, i);
            return;
        }
    }
}

// ฟังก์ชันสำหรับลบฮีปและคืนหน่วยความจำ
void free_heap(MinHeap* heap) {
    if (heap == NULL) return;
    
    if (heap->array != NULL) {
        free(heap->array);
    }
    
    free(heap);
}

// ฟังก์ชันสำหรับสร้างเส้นทางจากอาเรย์ของจุดก่อนหน้า
Route* build_path(Graph* graph, int* prev, int src, int dest) {
    int count = 0;
    int current = dest;
    
    while (current != -1) {
        count++;
        current = prev[current];
    }
    
    Route* route = create_route(count);
    route->length = count;
    
    current = dest;
    int i = count - 1;
    
    while (current != -1) {
        route->path[i] = current;
        current = prev[current];
        i--;
    }
    
    route->total_time = calculate_route_time(graph, route);
    route->total_distance = calculate_route_distance(graph, route);
    
    return route;
}

// ฟังก์ชันสำหรับสร้างเส้นทางโดยใช้อัลกอริทึม Dijkstra
Route* find_shortest_path(Graph* graph, int src, int dest) {
    if (src < 0 || src >= graph->num_vertices ||
        dest < 0 || dest >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex ID\n");
        return NULL;
    }
    
    float* dist = (float*)malloc(graph->num_vertices * sizeof(float));
    int* prev = (int*)malloc(graph->num_vertices * sizeof(int));
    bool* visited = (bool*)malloc(graph->num_vertices * sizeof(bool));
    
    if (dist == NULL || prev == NULL || visited == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        exit(1);
    }
    
    for (int i = 0; i < graph->num_vertices; i++) {
        dist[i] = FLT_MAX;
        prev[i] = -1;
        visited[i] = false;
    }
    
    dist[src] = 0.0;
    
    MinHeap* heap = create_min_heap(graph->num_vertices);
    
    insert_min_heap(heap, src, 0.0, 0.0);
    
    while (heap->size > 0) {
        HeapNode min = extract_min(heap);
        int u = min.vertex;
        
        if (u == dest) {
            break;
        }
        
        if (visited[u]) {
            continue;
        }
        
        visited[u] = true;
        
        Edge* current = graph->vertices[u].head;
        while (current != NULL) {
            int v = current->dest;
            float weight = current->weight;
            
            if (!visited[v] && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                
                if (is_in_heap(heap, v)) {
                    update_dist_in_heap(heap, v, dist[v], dist[v]);
                } else {
                    insert_min_heap(heap, v, dist[v], dist[v]);
                }
            }
            
            current = current->next;
        }
    }
    
    Route* route = build_path(graph, prev, src, dest);
    
    free(dist);
    free(prev);
    free(visited);
    free_heap(heap);
    
    return route;
}

// ฟังก์ชันสำหรับค้นหาเส้นทางที่มีการจราจรน้อยที่สุด
Route* find_least_congested_path(Graph* graph, int src, int dest) {
    if (src < 0 || src >= graph->num_vertices ||
        dest < 0 || dest >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex ID\n");
        return NULL;
    }
    
    float* congestion = (float*)malloc(graph->num_vertices * sizeof(float));
    int* prev = (int*)malloc(graph->num_vertices * sizeof(int));
    bool* visited = (bool*)malloc(graph->num_vertices * sizeof(bool));
    
    if (congestion == NULL || prev == NULL || visited == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        exit(1);
    }
    
    for (int i = 0; i < graph->num_vertices; i++) {
        congestion[i] = FLT_MAX;
        prev[i] = -1;
        visited[i] = false;
    }
    
    congestion[src] = 0.0;
    
    MinHeap* heap = create_min_heap(graph->num_vertices);
    
    insert_min_heap(heap, src, 0.0, 0.0);
    
    while (heap->size > 0) {
        HeapNode min = extract_min(heap);
        int u = min.vertex;
        
        if (u == dest) {
            break;
        }
        
        if (visited[u]) {
            continue;
        }
        
        visited[u] = true;
        
        Edge* current = graph->vertices[u].head;
        while (current != NULL) {
            int v = current->dest;
            
            float road_congestion = (float)current->road->current_load / current->road->capacity;
            if (road_congestion > 1.0) road_congestion = 1.0;
            
            float path_congestion = (congestion[u] > road_congestion) ? congestion[u] : road_congestion;
            
            if (!visited[v] && path_congestion < congestion[v]) {
                congestion[v] = path_congestion;
                prev[v] = u;
                
                if (is_in_heap(heap, v)) {
                    update_dist_in_heap(heap, v, congestion[v], congestion[v]);
                } else {
                    insert_min_heap(heap, v, congestion[v], congestion[v]);
                }
            }
            
            current = current->next;
        }
    }
    
    Route* route = build_path(graph, prev, src, dest);
    
    free(congestion);
    free(prev);
    free(visited);
    free_heap(heap);
    
    return route;
}

// ฟังก์ชันสำหรับค้นหาเส้นทางที่ใช้เวลาน้อยที่สุด
Route* find_fastest_path(Graph* graph, int src, int dest) {
    return find_shortest_path(graph, src, dest);
}

// ฟังก์ชันสำหรับแสดงข้อมูลของเส้นทาง
void print_route(Graph* graph, Route* route) {
    if (route == NULL) {
        printf("Route is empty\n");
        return;
    }
    
    printf("Route (Number of intersections: %d):\n", route->length);
    
    for (int i = 0; i < route->length; i++) {
        int junction_id = route->path[i];
        printf("  %d. Intersection ID: %d", i + 1, junction_id);
        
        if (graph->vertices[junction_id].name != NULL) {
            printf(" (%s)", graph->vertices[junction_id].name);
        }
        
        printf("\n");
        
        if (i < route->length - 1) {
            int next_junction_id = route->path[i + 1];
            
            Edge* current = graph->vertices[junction_id].head;
            while (current != NULL) {
                if (current->dest == next_junction_id) {
                    printf("     Road to next intersection: Lanes=%d, Length=%.2f km, Speed Limit=%.2f km/h, Capacity=%d, Current Load=%d\n",
                           current->road->lanes, current->road->length, current->road->speed_limit,
                           current->road->capacity, current->road->current_load);
                    printf("     Travel Time: %.2f hours\n", current->weight);
                    break;
                }
                current = current->next;
            }
        }
    }
    
    printf("\nTotal Travel Time: %.2f hours\n", route->total_time);
    printf("Total Distance: %.2f kilometers\n", route->total_distance);
}

// ฟังก์ชันสำหรับเปรียบเทียบเส้นทาง
void compare_routes(Graph* graph, Route* route1, Route* route2) {
    if (route1 == NULL || route2 == NULL) {
        printf("Both routes are required for comparison\n");
        return;
    }
    
    printf("Route Comparison:\n");
    
    printf("Travel Time:\n");
    printf("  Route 1: %.2f hours\n", route1->total_time);
    printf("  Route 2: %.2f hours\n", route2->total_time);
    
    float time_diff = route1->total_time - route2->total_time;
    if (time_diff > 0) {
        printf("  Route 2 is faster by %.2f hours (%.2f%%)\n",
               time_diff, (time_diff / route1->total_time) * 100.0);
    } else if (time_diff < 0) {
        printf("  Route 1 is faster by %.2f hours (%.2f%%)\n",
               -time_diff, (-time_diff / route2->total_time) * 100.0);
    } else {
        printf("  Both routes have the same travel time\n");
    }
    
    printf("\nDistance:\n");
    printf("  Route 1: %.2f kilometers\n", route1->total_distance);
    printf("  Route 2: %.2f kilometers\n", route2->total_distance);
    
    float distance_diff = route1->total_distance - route2->total_distance;
    if (distance_diff > 0) {
        printf("  Route 2 is shorter by %.2f kilometers (%.2f%%)\n",
               distance_diff, (distance_diff / route1->total_distance) * 100.0);
    } else if (distance_diff < 0) {
        printf("  Route 1 is shorter by %.2f kilometers (%.2f%%)\n",
               -distance_diff, (-distance_diff / route2->total_distance) * 100.0);
    } else {
        printf("  Both routes have the same distance\n");
    }
    
    printf("\nNumber of Intersections:\n");
    printf("  Route 1: %d intersections\n", route1->length);
    printf("  Route 2: %d intersections\n", route2->length);
    
    int junction_diff = route1->length - route2->length;
    if (junction_diff > 0) {
        printf("  Route 2 has %d fewer intersections\n", junction_diff);
    } else if (junction_diff < 0) {
        printf("  Route 1 has %d fewer intersections\n", -junction_diff);
    } else {
        printf("  Both routes have the same number of intersections\n");
    }
}

// ฟังก์ชันสำหรับลบเส้นทางและคืนหน่วยความจำ
void free_route(Route* route) {
    if (route == NULL) return;
    
    if (route->path != NULL) {
        free(route->path);
    }
    
    free(route);
}

// ฟังก์ชันสำหรับค้นหาเส้นทางที่ดีที่สุดโดยพิจารณาหลายปัจจัย
Route* find_optimal_path(Graph* graph, int src, int dest, float time_weight, float distance_weight, float congestion_weight) {
    if (src < 0 || src >= graph->num_vertices ||
        dest < 0 || dest >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex ID\n");
        return NULL;
    }
    
    float* cost = (float*)malloc(graph->num_vertices * sizeof(float));
    int* prev = (int*)malloc(graph->num_vertices * sizeof(int));
    bool* visited = (bool*)malloc(graph->num_vertices * sizeof(bool));
    
    if (cost == NULL || prev == NULL || visited == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        exit(1);
    }
    
    for (int i = 0; i < graph->num_vertices; i++) {
        cost[i] = FLT_MAX;
        prev[i] = -1;
        visited[i] = false;
    }
    
    cost[src] = 0.0;
    
    MinHeap* heap = create_min_heap(graph->num_vertices);
    
    insert_min_heap(heap, src, 0.0, 0.0);
    
    while (heap->size > 0) {
        HeapNode min = extract_min(heap);
        int u = min.vertex;
        
        if (u == dest) {
            break;
        }
        
        if (visited[u]) {
            continue;
        }
        
        visited[u] = true;
        
        Edge* current = graph->vertices[u].head;
        while (current != NULL) {
            int v = current->dest;
            
            float travel_time = current->weight;
            float distance = current->road->length;
            
            float congestion = (float)current->road->current_load / current->road->capacity;
            if (congestion > 1.0) congestion = 1.0;
            
            float total_cost = cost[u] +
                              (time_weight * travel_time) +
                              (distance_weight * distance) +
                              (congestion_weight * congestion);
            
            if (!visited[v] && total_cost < cost[v]) {
                cost[v] = total_cost;
                prev[v] = u;
                
                if (is_in_heap(heap, v)) {
                    update_dist_in_heap(heap, v, cost[v], cost[v]);
                } else {
                    insert_min_heap(heap, v, cost[v], cost[v]);
                }
            }
            
            current = current->next;
        }
    }
    
    Route* route = build_path(graph, prev, src, dest);
    
    free(cost);
    free(prev);
    free(visited);
    free_heap(heap);
    
    return route;
}
