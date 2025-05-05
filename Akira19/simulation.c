/*
* simulation.c
* การดำเนินการของการจำลองระบบการจราจร
*/

#include "simulation.h"

// ฟังก์ชันสำหรับสร้างการจำลองระบบการจราจรใหม่
TrafficSimulation* create_simulation(Graph* graph, SignalSystem* signal_system, int max_vehicles) {
    TrafficSimulation* sim = (TrafficSimulation*)malloc(sizeof(TrafficSimulation));
    if (sim == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for simulation\n");
        exit(1);
    }
    
    sim->graph = graph;
    sim->signal_system = signal_system;
    
    sim->vehicles = (Vehicle*)malloc(max_vehicles * sizeof(Vehicle));
    if (sim->vehicles == NULL && max_vehicles > 0) {
        fprintf(stderr, "Error: Unable to allocate memory for vehicles\n");
        free(sim);
        exit(1);
    }
    
    sim->num_vehicles = 0;
    sim->max_vehicles = max_vehicles;
    sim->time_step = 0;
    sim->is_running = false;
    
    // ตั้งค่าเริ่มต้นสำหรับเลขสุ่ม
    srand((unsigned int)time(NULL));
    
    return sim;
}

// ฟังก์ชันสำหรับสร้างยานพาหนะใหม่ในการจำลอง
int add_vehicle(TrafficSimulation* sim, int origin, int destination) {
    if (sim->num_vehicles >= sim->max_vehicles) {
        fprintf(stderr, "Error: Cannot add vehicle because limit exceeded\n");
        return -1;
    }
    
    if (origin < 0 || origin >= sim->graph->num_vertices ||
        destination < 0 || destination >= sim->graph->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex ID\n");
        return -1;
    }
    
    // สร้างยานพาหนะใหม่
    int vehicle_id = sim->num_vehicles;
    
    sim->vehicles[vehicle_id].id = vehicle_id;
    sim->vehicles[vehicle_id].origin = origin;
    sim->vehicles[vehicle_id].destination = destination;
    
    // หาเส้นทางที่ดีที่สุด
    sim->vehicles[vehicle_id].route = find_optimal_path(
        sim->graph, origin, destination, 0.6, 0.2, 0.2);
    
    if (sim->vehicles[vehicle_id].route == NULL) {
        fprintf(stderr, "Error: Unable to find route\n");
        return -1;
    }
    
    // ตั้งค่าเริ่มต้น
    sim->vehicles[vehicle_id].route_index = 0;
    sim->vehicles[vehicle_id].current_road = -1;
    sim->vehicles[vehicle_id].current_pos = 0;
    sim->vehicles[vehicle_id].speed = 0.0;
    sim->vehicles[vehicle_id].completed = false;
    
    // เพิ่มยานพาหนะลงบนถนนแรกในเส้นทาง
    if (sim->vehicles[vehicle_id].route->length > 1) {
        int src = sim->vehicles[vehicle_id].route->path[0];
        int dest = sim->vehicles[vehicle_id].route->path[1];
        
        // หาเส้นเชื่อมไปยังทางแยกถัดไป
        Edge* current = sim->graph->vertices[src].head;
        while (current != NULL) {
            if (current->dest == dest) {
                // เพิ่มการจราจรบนถนนนี้
                current->road->current_load++;
                
                // อัปเดตน้ำหนักของเส้นเชื่อม
                current->weight = calculate_travel_time(current->road);
                
                // ตั้งค่าถนนปัจจุบัน
                sim->vehicles[vehicle_id].current_road = dest;
                
                // ตั้งค่าความเร็วเริ่มต้น
                sim->vehicles[vehicle_id].speed = current->road->speed_limit;
                
                break;
            }
            current = current->next;
        }
        
        sim->vehicles[vehicle_id].route_index = 1;
    } else {
        // หากเส้นทางมีเพียงจุดเดียว (origin = destination)
        sim->vehicles[vehicle_id].completed = true;
    }
    
    sim->num_vehicles++;
    
    return vehicle_id;
}

// ฟังก์ชันสำหรับอัปเดตตำแหน่งของยานพาหนะ
void update_vehicle(TrafficSimulation* sim, int vehicle_id) {
    if (vehicle_id < 0 || vehicle_id >= sim->num_vehicles) {
        fprintf(stderr, "Error: Invalid vehicle ID\n");
        return;
    }
    
    Vehicle* vehicle = &sim->vehicles[vehicle_id];
    
    // ถ้ายานพาหนะถึงจุดหมายแล้ว ไม่ต้องอัปเดต
    if (vehicle->completed) {
        return;
    }
    
    // ถ้ายานพาหนะยังไม่ได้อยู่บนถนน (เพิ่งสร้าง) ไม่ต้องอัปเดต
    if (vehicle->current_road == -1) {
        return;
    }
    
    // หาถนนปัจจุบันที่ยานพาหนะกำลังเดินทาง
    int current_junction_idx = vehicle->route_index - 1;
    int src = vehicle->route->path[current_junction_idx];
    int dest = vehicle->route->path[current_junction_idx + 1];
    
    Edge* current_edge = NULL;
    Edge* edge = sim->graph->vertices[src].head;
    
    while (edge != NULL) {
        if (edge->dest == dest) {
            current_edge = edge;
            break;
        }
        edge = edge->next;
    }
    
    if (current_edge == NULL) {
        fprintf(stderr, "Error: Current road not found\n");
        return;
    }
    
    // คำนวณการเคลื่อนที่ของยานพาหนะ (ระยะทางต่อวินาที)
    float distance_per_second = vehicle->speed / 3600.0; // กม./ชม. เป็น กม./วินาที
    
    // เพิ่มตำแหน่งปัจจุบัน
    vehicle->current_pos += (int)(distance_per_second * 1000); // เป็นเมตร
    
    // ถ้าถึงปลายทางของถนนปัจจุบัน
    if (vehicle->current_pos >= (int)(current_edge->road->length * 1000)) {
        // ลดการจราจรบนถนนปัจจุบัน
        current_edge->road->current_load--;
        
        // อัปเดตน้ำหนักของเส้นเชื่อม
        current_edge->weight = calculate_travel_time(current_edge->road);
        
        // ถ้าถึงจุดหมายปลายทางแล้ว
        if (dest == vehicle->destination) {
            vehicle->completed = true;
            return;
        }
        
        // เลื่อนไปยังถนนถัดไปในเส้นทาง
        vehicle->route_index++;
        
        // ถ้าไม่มีถนนถัดไป (ถึงจุดหมายปลายทางแล้ว)
        if (vehicle->route_index >= vehicle->route->length) {
            vehicle->completed = true;
            return;
        }
        
        // หาถนนถัดไป
        int next_src = dest;
        int next_dest = vehicle->route->path[vehicle->route_index];
        
        Edge* next_edge = NULL;
        edge = sim->graph->vertices[next_src].head;
        
        while (edge != NULL) {
            if (edge->dest == next_dest) {
                next_edge = edge;
                break;
            }
            edge = edge->next;
        }
        
        if (next_edge == NULL) {
            fprintf(stderr, "Error: Next road not found\n");
            return;
        }
        
        // เพิ่มการจราจรบนถนนถัดไป
        next_edge->road->current_load++;
        
        // อัปเดตน้ำหนักของเส้นเชื่อม
        next_edge->weight = calculate_travel_time(next_edge->road);
        
        // อัปเดตตำแหน่งปัจจุบัน
        vehicle->current_road = next_dest;
        vehicle->current_pos = 0;
        
        // ปรับความเร็วตามความเร็วจำกัดของถนนใหม่
        vehicle->speed = next_edge->road->speed_limit;
        
        // ปรับความเร็วตามความหนาแน่นของการจราจร
        float congestion = (float)next_edge->road->current_load / next_edge->road->capacity;
        if (congestion > 1.0) congestion = 1.0;
        
        // ลดความเร็วตามความหนาแน่น
        vehicle->speed *= (1.0 - 0.7 * congestion);
    }
}

// ฟังก์ชันสำหรับอัปเดตสถานะของการจำลองในแต่ละขั้นตอนเวลา
void update_simulation(TrafficSimulation* sim) {
    if (!sim->is_running) {
        return;
    }
    
    // เพิ่มขั้นตอนเวลา
    sim->time_step++;
    
    // อัปเดตระบบสัญญาณไฟจราจร
    update_signal_system(sim->graph, sim->signal_system);
    
    // อัปเดตยานพาหนะทุกคัน
    for (int i = 0; i < sim->num_vehicles; i++) {
        update_vehicle(sim, i);
    }
    
    // อัปเดตน้ำหนักของเส้นเชื่อมทั้งหมด
    update_edge_weight(sim->graph);
}

// ฟังก์ชันสำหรับเริ่มการจำลอง
void start_simulation(TrafficSimulation* sim) {
    sim->is_running = true;
    printf("Starting traffic simulation\n");
}

// ฟังก์ชันสำหรับหยุดการจำลอง
void stop_simulation(TrafficSimulation* sim) {
    sim->is_running = false;
    printf("Stopping traffic simulation\n");
}

// ฟังก์ชันสำหรับสร้างการจราจรแบบสุ่ม
void generate_random_traffic(TrafficSimulation* sim, int num_vehicles) {
    if (num_vehicles <= 0) {
        fprintf(stderr, "Error: Number of vehicles must be greater than 0\n");
        return;
    }
    
    if (sim->num_vehicles + num_vehicles > sim->max_vehicles) {
        fprintf(stderr, "Error: Number of vehicles exceeds limit\n");
        return;
    }
    
    for (int i = 0; i < num_vehicles; i++) {
        // สุ่มจุดต้นทางและปลายทาง
        int origin = rand() % sim->graph->num_vertices;
        int destination;
        
        // สุ่มจุดปลายทางที่ไม่ใช่จุดต้นทาง
        do {
            destination = rand() % sim->graph->num_vertices;
        } while (destination == origin);
        
        // เพิ่มยานพาหนะ
        add_vehicle(sim, origin, destination);
    }
}

// ฟังก์ชันสำหรับวิเคราะห์ผลการจำลอง
void analyze_simulation_results(TrafficSimulation* sim) {
    printf("\nSimulation Analysis Results (Time: %d seconds):\n", sim->time_step);
    
    // นับจำนวนยานพาหนะที่เดินทางถึงจุดหมายแล้ว
    int completed_count = 0;
    for (int i = 0; i < sim->num_vehicles; i++) {
        if (sim->vehicles[i].completed) {
            completed_count++;
        }
    }
    
    printf("Total vehicles: %d\n", sim->num_vehicles);
    printf("Vehicles reached destination: %d (%.2f%%)\n",
           completed_count, ((float)completed_count / sim->num_vehicles) * 100.0);
    
    // คำนวณความหนาแน่นของการจราจรเฉลี่ย
    float total_congestion = 0.0;
    int road_count = 0;
    
    for (int i = 0; i < sim->graph->num_vertices; i++) {
        Edge* current = sim->graph->vertices[i].head;
        while (current != NULL) {
            float congestion = (float)current->road->current_load / current->road->capacity;
            if (congestion > 1.0) congestion = 1.0;
            
            total_congestion += congestion;
            road_count++;
            
            current = current->next;
        }
    }
    
    float avg_congestion = (road_count > 0) ? total_congestion / road_count : 0.0;
    printf("Average traffic congestion: %.2f%%\n", avg_congestion * 100.0);
    
    // หาถนนที่มีการจราจรหนาแน่นที่สุด
    float max_congestion = 0.0;
    int max_congestion_src = -1;
    int max_congestion_dest = -1;
    
    for (int i = 0; i < sim->graph->num_vertices; i++) {
        Edge* current = sim->graph->vertices[i].head;
        while (current != NULL) {
            float congestion = (float)current->road->current_load / current->road->capacity;
            if (congestion > max_congestion) {
                max_congestion = congestion;
                max_congestion_src = i;
                max_congestion_dest = current->dest;
            }
            
            current = current->next;
        }
    }
    
    if (max_congestion_src != -1) {
        printf("Most congested road: from intersection %d to intersection %d (%.2f%%)\n",
               max_congestion_src, max_congestion_dest, max_congestion * 100.0);
    }
}

// ฟังก์ชันสำหรับแสดงข้อมูลของการจำลอง
void print_simulation_status(TrafficSimulation* sim) {
    printf("\nSimulation Status (Time: %d seconds):\n", sim->time_step);
    printf("Status: %s\n", sim->is_running ? "Running" : "Stopped");
    printf("Number of vehicles: %d / %d\n", sim->num_vehicles, sim->max_vehicles);
    
    // นับจำนวนยานพาหนะที่เดินทางถึงจุดหมายแล้ว
    int completed_count = 0;
    for (int i = 0; i < sim->num_vehicles; i++) {
        if (sim->vehicles[i].completed) {
            completed_count++;
        }
    }
    
    printf("Vehicles reached destination: %d\n", completed_count);
    
    // แสดงข้อมูลของยานพาหนะบางส่วน (แสดงเพียง 5 คันแรก)
    int display_count = (sim->num_vehicles < 5) ? sim->num_vehicles : 5;
    printf("\nVehicle Information (showing first %d):\n", display_count);
    
    for (int i = 0; i < display_count; i++) {
        Vehicle* vehicle = &sim->vehicles[i];
        
        printf("Vehicle ID: %d\n", vehicle->id);
        printf("  Origin: %d, Destination: %d\n", vehicle->origin, vehicle->destination);
        printf("  Status: %s\n", vehicle->completed ? "Reached destination" : "Traveling");
        
        if (!vehicle->completed) {
            printf("  Current position: Road to intersection %d (%.2f km)\n",
                 vehicle->current_road, (float)vehicle->current_pos / 1000.0);
            printf("  Current speed: %.2f km/h\n", vehicle->speed);
        }
        
        printf("\n");
    }
}

// ฟังก์ชันสำหรับลบการจำลองและคืนหน่วยความจำ
void free_simulation(TrafficSimulation* sim) {
    if (sim == NULL) return;
    
    // ลบยานพาหนะทั้งหมด
    for (int i = 0; i < sim->num_vehicles; i++) {
        if (sim->vehicles[i].route != NULL) {
            free_route(sim->vehicles[i].route);
        }
    }
    
    // ลบอาเรย์ของยานพาหนะ
    if (sim->vehicles != NULL) {
        free(sim->vehicles);
    }
    
    // ลบการจำลอง
    free(sim);
}
