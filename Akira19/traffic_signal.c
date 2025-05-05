#include "traffic_signal.h"

// ฟังก์ชันสำหรับสร้างระบบสัญญาณไฟจราจรใหม่
SignalSystem* create_signal_system(Graph* graph) {
    SignalSystem* system = (SignalSystem*)malloc(sizeof(SignalSystem));
    if (system == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for traffic signal system\n");
        exit(1);
    }
    
    // นับจำนวนทางแยกที่มีสัญญาณไฟจราจร
    int count = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        if (graph->vertices[i].has_signal) {
            count++;
        }
    }
    
    system->num_signals = count;
    system->signals = (TrafficSignal*)malloc(count * sizeof(TrafficSignal));
    if (system->signals == NULL && count > 0) {
        fprintf(stderr, "Error: Unable to allocate memory for traffic signals\n");
        free(system);
        exit(1);
    }
    
    // สร้างสัญญาณไฟจราจรสำหรับแต่ละทางแยกที่มีสัญญาณไฟ
    int j = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        if (graph->vertices[i].has_signal) {
            // นับจำนวนเส้นทางที่เชื่อมต่อกับทางแยก
            int num_roads = 0;
            Edge* current = graph->vertices[i].head;
            while (current != NULL) {
                num_roads++;
                current = current->next;
            }
            
            // สร้างสัญญาณไฟจราจรสำหรับทางแยกนี้
            system->signals[j] = *create_traffic_signal(i, num_roads, true);
            j++;
        }
    }
    
    // สร้างคิวสำหรับจัดลำดับความสำคัญ
    system->queue = create_priority_queue();
    
    // เพิ่มทางแยกที่มีสัญญาณไฟลงในคิว
    for (int i = 0; i < system->num_signals; i++) {
        int junction_id = system->signals[i].junction_id;
        float congestion = calculate_junction_congestion(graph, junction_id);
        enqueue(system->queue, junction_id, congestion);
    }
    
    return system;
}

// ฟังก์ชันสำหรับสร้างสัญญาณไฟจราจรที่ทางแยก
TrafficSignal* create_traffic_signal(int junction_id, int num_phases, bool is_adaptive) {
    TrafficSignal* signal = (TrafficSignal*)malloc(sizeof(TrafficSignal));
    if (signal == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for traffic signal\n");
        exit(1);
    }
    
    signal->junction_id = junction_id;
    signal->num_phases = num_phases;
    signal->phases = (SignalPhase*)malloc(num_phases * sizeof(SignalPhase));
    if (signal->phases == NULL && num_phases > 0) {
        fprintf(stderr, "Error: Unable to allocate memory for signal phases\n");
        free(signal);
        exit(1);
    }
    
    signal->current_phase = 0;
    signal->is_adaptive = is_adaptive;
    
    // กำหนดค่าเริ่มต้นสำหรับแต่ละเฟส
    for (int i = 0; i < num_phases; i++) {
        signal->phases[i].direction = i % 4; // 0 = เหนือ, 1 = ตะวันออก, 2 = ใต้, 3 = ตะวันตก
        signal->phases[i].state = (i == 0) ? GREEN : RED; // เฟสแรกเป็นไฟเขียว ที่เหลือเป็นไฟแดง
        signal->phases[i].duration = 30; // ค่าเริ่มต้น 30 วินาที
        signal->phases[i].remaining_time = (i == 0) ? 30 : 0; // เฟสแรกเริ่มนับเวลา
    }
    
    return signal;
}

// ฟังก์ชันสำหรับกำหนดค่าเฟสของสัญญาณไฟจราจร
void set_signal_phase(TrafficSignal* signal, int phase_index, int direction, SignalState state, int duration) {
    if (phase_index < 0 || phase_index >= signal->num_phases) {
        fprintf(stderr, "Error: Invalid phase index\n");
        return;
    }
    
    signal->phases[phase_index].direction = direction;
    signal->phases[phase_index].state = state;
    signal->phases[phase_index].duration = duration;
    
    // ถ้าเป็นเฟสปัจจุบัน ให้ตั้งค่าเวลาที่เหลือ
    if (phase_index == signal->current_phase) {
        signal->phases[phase_index].remaining_time = duration;
    } else {
        signal->phases[phase_index].remaining_time = 0;
    }
}

// ฟังก์ชันสำหรับอัปเดตสัญญาณไฟจราจร (ลดเวลาที่เหลือและเปลี่ยนเฟส)
void update_traffic_signal(TrafficSignal* signal) {
    // ลดเวลาที่เหลือของเฟสปัจจุบัน
    signal->phases[signal->current_phase].remaining_time--;
    
    // ถ้าเวลาหมด ให้เปลี่ยนเฟส
    if (signal->phases[signal->current_phase].remaining_time <= 0) {
        // เปลี่ยนสถานะเฟสปัจจุบันเป็นไฟแดง
        signal->phases[signal->current_phase].state = RED;
        
        // เปลี่ยนไปเฟสถัดไป
        signal->current_phase = (signal->current_phase + 1) % signal->num_phases;
        
        // เปลี่ยนสถานะเฟสใหม่เป็นไฟเขียว และตั้งค่าเวลาที่เหลือ
        signal->phases[signal->current_phase].state = GREEN;
        signal->phases[signal->current_phase].remaining_time = signal->phases[signal->current_phase].duration;
    }
}

// ฟังก์ชันสำหรับคำนวณความหนาแน่นของการจราจรที่ทางแยก
float calculate_junction_congestion(Graph* graph, int junction_id) {
    if (junction_id < 0 || junction_id >= graph->num_vertices) {
        fprintf(stderr, "Error: Invalid junction ID\n");
        return 0.0;
    }
    
    // นับจำนวนรถทั้งหมดและความจุทั้งหมดของถนนที่เชื่อมต่อกับทางแยก
    int total_load = 0;
    int total_capacity = 0;
    
    Edge* current = graph->vertices[junction_id].head;
    while (current != NULL) {
        total_load += current->road->current_load;
        total_capacity += current->road->capacity;
        current = current->next;
    }
    
    // คำนวณความหนาแน่น (0.0 - 1.0)
    float congestion = (total_capacity > 0) ? (float)total_load / total_capacity : 0.0;
    if (congestion > 1.0) congestion = 1.0; // ป้องกันค่าเกิน
    
    return congestion;
}

// ฟังก์ชันสำหรับปรับระยะเวลาของเฟสตามความหนาแน่นของการจราจร
void adjust_signal_timing(Graph* graph, TrafficSignal* signal) {
    // ถ้าไม่ใช่ระบบปรับตัว ไม่ต้องปรับระยะเวลา
    if (!signal->is_adaptive) {
        return;
    }
    
    // คำนวณความหนาแน่นของการจราจรในแต่ละทิศทาง
    float direction_congestion[4] = {0.0, 0.0, 0.0, 0.0}; // เหนือ, ตะวันออก, ใต้, ตะวันตก
    
    Edge* current = graph->vertices[signal->junction_id].head;
    while (current != NULL) {
        // ใช้ความรู้ภายนอกสมมติว่าเราสามารถกำหนดทิศทางของถนนได้
        int direction = current->dest % 4; // ใช้ปลายทางเป็นตัวกำหนดทิศทาง (นี่เป็นเพียงตัวอย่าง)
        
        // คำนวณความหนาแน่นของถนนนี้
        float road_congestion = (float)current->road->current_load / current->road->capacity;
        if (road_congestion > 1.0) road_congestion = 1.0;
        
        // อัปเดตความหนาแน่นสูงสุดในทิศทางนี้
        if (road_congestion > direction_congestion[direction]) {
            direction_congestion[direction] = road_congestion;
        }
        
        current = current->next;
    }
    
    // ปรับระยะเวลาของแต่ละเฟสตามความหนาแน่น
    for (int i = 0; i < signal->num_phases; i++) {
        int direction = signal->phases[i].direction;
        
        // คำนวณระยะเวลาใหม่: ระยะเวลาพื้นฐาน + ส่วนเพิ่มตามความหนาแน่น
        int base_duration = 20; // ระยะเวลาพื้นฐาน (วินาที)
        int max_additional = 40; // ส่วนเพิ่มสูงสุด (วินาที)
        
        int new_duration = base_duration + (int)(direction_congestion[direction] * max_additional);
        
        // ตั้งค่าระยะเวลาใหม่
        signal->phases[i].duration = new_duration;
        
        // ถ้าเป็นเฟสปัจจุบันและเวลาที่เหลือมากกว่าระยะเวลาใหม่ ให้ปรับเวลาที่เหลือด้วย
        if (i == signal->current_phase && signal->phases[i].remaining_time > new_duration) {
            signal->phases[i].remaining_time = new_duration;
        }
    }
}

// ฟังก์ชันสำหรับจัดการคิวสัญญาณไฟจราจรอัจฉริยะ
void manage_signal_queue(Graph* graph, SignalSystem* system) {
    // ประเมินความหนาแน่นของการจราจรที่ทางแยกทั้งหมด
    for (int i = 0; i < system->num_signals; i++) {
        int junction_id = system->signals[i].junction_id;
        float congestion = calculate_junction_congestion(graph, junction_id);
        
        // อัปเดตค่าความสำคัญในคิว
        update_priority(system->queue, junction_id, congestion);
    }
}

// ฟังก์ชันสำหรับอัปเดตระบบสัญญาณไฟจราจรทั้งหมด
void update_signal_system(Graph* graph, SignalSystem* system) {
    // จัดการคิวสัญญาณไฟจราจรอัจฉริยะ
    manage_signal_queue(graph, system);
    
    // ปรับระยะเวลาของสัญญาณไฟจราจรตามความหนาแน่น
    for (int i = 0; i < system->num_signals; i++) {
        adjust_signal_timing(graph, &system->signals[i]);
        
        // อัปเดตสัญญาณไฟจราจร
        update_traffic_signal(&system->signals[i]);
    }
}

// ฟังก์ชันสำหรับแสดงข้อมูลของสัญญาณไฟจราจร
void print_traffic_signal(TrafficSignal* signal) {
    printf("Traffic Signal at Junction ID: %d\n", signal->junction_id);
    printf("  Number of phases: %d\n", signal->num_phases);
    printf("  Adaptive system: %s\n", signal->is_adaptive ? "Yes" : "No");
    printf("  Current phase: %d\n", signal->current_phase);
    
    printf("  Phase details:\n");
    for (int i = 0; i < signal->num_phases; i++) {
        printf("    Phase %d:\n", i);
        
        // แสดงทิศทาง
        const char* direction_str;
        switch (signal->phases[i].direction) {
            case 0: direction_str = "North"; break;
            case 1: direction_str = "East"; break;
            case 2: direction_str = "South"; break;
            case 3: direction_str = "West"; break;
            default: direction_str = "Unknown";
        }
        printf("      Direction: %s\n", direction_str);
        
        // แสดงสถานะ
        const char* state_str;
        switch (signal->phases[i].state) {
            case RED: state_str = "Red"; break;
            case GREEN: state_str = "Green"; break;
            case YELLOW: state_str = "Yellow"; break;
            default: state_str = "Unknown";
        }
        printf("      State: %s\n", state_str);
        
        printf("      Duration: %d seconds\n", signal->phases[i].duration);
        printf("      Remaining time: %d seconds\n", signal->phases[i].remaining_time);
    }
}

// ฟังก์ชันสำหรับแสดงข้อมูลของระบบสัญญาณไฟจราจร
void print_signal_system(SignalSystem* system) {
    printf("Traffic Signal System:\n");
    printf("Total number of traffic signals: %d\n", system->num_signals);
    
    // แสดงข้อมูลคิว
    printf("\nPriority Queue:\n");
    print_queue(system->queue);
    
    // แสดงข้อมูลสัญญาณไฟจราจรแต่ละจุด
    printf("\nTraffic Signal Details:\n");
    for (int i = 0; i < system->num_signals; i++) {
        print_traffic_signal(&system->signals[i]);
        printf("\n");
    }
}

// ฟังก์ชันสำหรับลบสัญญาณไฟจราจรและคืนหน่วยความจำ
void free_traffic_signal(TrafficSignal* signal) {
    if (signal == NULL) return;
    
    // ลบเฟส
    if (signal->phases != NULL) {
        free(signal->phases);
    }
    
    // ลบสัญญาณไฟจราจร
    free(signal);
}

// ฟังก์ชันสำหรับลบระบบสัญญาณไฟจราจรและคืนหน่วยความจำ
void free_signal_system(SignalSystem* system) {
    if (system == NULL) return;
    
    // ลบสัญญาณไฟจราจรทั้งหมด
    for (int i = 0; i < system->num_signals; i++) {
        free(system->signals[i].phases);
    }
    
    // ลบอาเรย์ของสัญญาณไฟจราจร
    free(system->signals);
    
    // ลบคิว
    free_queue(system->queue);
    
    // ลบระบบ
    free(system);
}
