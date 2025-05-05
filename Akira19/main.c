#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  // สำหรับฟังก์ชัน sleep()
#include <stdbool.h>

#include "graph.h"
#include "queue.h"
#include "traffic_signal.h"
#include "route.h"
#include "simulation.h"


// ฟังก์ชันสำหรับสร้างเครือข่ายถนนตัวอย่าง
Graph* create_sample_network() {
    // สร้างกราฟขนาด 6 ทางแยก
    Graph* graph = create_graph(6);
    
    // เพิ่มทางแยก
    add_vertex(graph, 0, "Main Intersection", true);
    add_vertex(graph, 1, "North Intersection", true);
    add_vertex(graph, 2, "East Intersection", true);
    add_vertex(graph, 3, "South Intersection", true);
    add_vertex(graph, 4, "West Intersection", true);
    add_vertex(graph, 5, "Business District", true);
    
    // สร้างถนน
    // ถนนจากทางแยกหลักไปทางแยกอื่นๆ
    add_edge(graph, 0, 1, create_road(3, 2.0, 60.0, 300));
    add_edge(graph, 0, 2, create_road(2, 1.5, 50.0, 200));
    add_edge(graph, 0, 3, create_road(3, 2.0, 60.0, 300));
    add_edge(graph, 0, 4, create_road(2, 1.5, 50.0, 200));
    add_edge(graph, 0, 5, create_road(4, 1.0, 40.0, 400));
    
    // ถนนจากทางแยกต่างๆ กลับทางแยกหลัก
    add_edge(graph, 1, 0, create_road(3, 2.0, 60.0, 300));
    add_edge(graph, 2, 0, create_road(2, 1.5, 50.0, 200));
    add_edge(graph, 3, 0, create_road(3, 2.0, 60.0, 300));
    add_edge(graph, 4, 0, create_road(2, 1.5, 50.0, 200));
    add_edge(graph, 5, 0, create_road(4, 1.0, 40.0, 400));
    
    // ถนนเชื่อมระหว่างทางแยกอื่นๆ
    add_edge(graph, 1, 2, create_road(2, 3.0, 70.0, 250)); // เหนือ -> ตะวันออก
    add_edge(graph, 2, 3, create_road(2, 3.0, 70.0, 250));
    add_edge(graph, 3, 4, create_road(2, 3.0, 70.0, 250));
    add_edge(graph, 4, 1, create_road(2, 3.0, 70.0, 250));
    
    // ถนนเชื่อมไปย่านธุรกิจ
    add_edge(graph, 2, 5, create_road(3, 1.2, 45.0, 350));
    add_edge(graph, 5, 2, create_road(3, 1.2, 45.0, 350));
    add_edge(graph, 4, 5, create_road(3, 1.2, 45.0, 350));
    add_edge(graph, 5, 4, create_road(3, 1.2, 45.0, 350));
    
    return graph;
}

// ฟังก์ชันสำหรับจำลองการจราจรช่วงเวลาเช้า (เข้าเมือง)
void simulate_morning_traffic(TrafficSimulation* sim) {
    
    // จำนวนยานพาหนะจากทางแยกต่างๆ
    int traffic_from_north = 50;
    int traffic_from_east = 30;
    int traffic_from_south = 60;
    int traffic_from_west = 40;
    
    // สร้างการจราจรจากทางเหนือ (1) ไปยังย่านธุรกิจ (5)
    for (int i = 0; i < traffic_from_north; i++) {
        add_vehicle(sim, 1, 5);
    }
    
    // สร้างการจราจรจากทางตะวันออก (2) ไปยังย่านธุรกิจ (5)
    for (int i = 0; i < traffic_from_east; i++) {
        add_vehicle(sim, 2, 5);
    }
    
    // สร้างการจราจรจากทางใต้ (3) ไปยังย่านธุรกิจ (5)
    for (int i = 0; i < traffic_from_south; i++) {
        add_vehicle(sim, 3, 5);
    }
    
    // สร้างการจราจรจากทางตะวันตก (4) ไปยังย่านธุรกิจ (5)
    for (int i = 0; i < traffic_from_west; i++) {
        add_vehicle(sim, 4, 5);
    }
    
    printf("Morning traffic simulation: Added %d vehicles traveling to Business District\n",
           traffic_from_north + traffic_from_east + traffic_from_south + traffic_from_west);
}

// ฟังก์ชันสำหรับจำลองการจราจรช่วงเย็น (ออกจากเมือง)
void simulate_evening_traffic(TrafficSimulation* sim) {

    
    // จำนวนยานพาหนะไปยังทางแยกต่างๆ
    int traffic_to_north = 50;
    int traffic_to_east = 30;
    int traffic_to_south = 60;
    int traffic_to_west = 40;
    
    // สร้างการจราจรจากย่านธุรกิจ (5) ไปยังทางเหนือ (1)
    for (int i = 0; i < traffic_to_north; i++) {
        add_vehicle(sim, 5, 1);
    }
    
    // สร้างการจราจรจากย่านธุรกิจ (5) ไปยังทางตะวันออก (2)
    for (int i = 0; i < traffic_to_east; i++) {
        add_vehicle(sim, 5, 2);
    }
    
    // สร้างการจราจรจากย่านธุรกิจ (5) ไปยังทางใต้ (3)
    for (int i = 0; i < traffic_to_south; i++) {
        add_vehicle(sim, 5, 3);
    }
    
    // สร้างการจราจรจากย่านธุรกิจ (5) ไปยังทางตะวันตก (4)
    for (int i = 0; i < traffic_to_west; i++) {
        add_vehicle(sim, 5, 4);
    }
    
    printf("Evening traffic simulation: Added %d vehicles traveling from Business District\n",
           traffic_to_north + traffic_to_east + traffic_to_south + traffic_to_west);
}


int main() {
    printf("=== Intelligent Traffic Simulation System ===\n\n");
    
    // สร้างเครือข่ายถนน
    printf("Creating road network...\n");
    Graph* graph = create_sample_network();
    print_graph(graph);
    
    // สร้างระบบสัญญาณไฟจราจร
    printf("\nCreating traffic signal system...\n");
    SignalSystem* signal_system = create_signal_system(graph);
    print_signal_system(signal_system);
    
    // สร้างการจำลองระบบการจราจร
    printf("\nCreating traffic simulation...\n");
    TrafficSimulation* sim = create_simulation(graph, signal_system, 500);
    
    // เริ่มการจำลอง
    printf("\n=== Starting Traffic Simulation ===\n");
    start_simulation(sim);
    
    // จำลองการจราจรช่วงเช้า (เข้าเมือง)
    printf("\n=== Morning Rush Hour (Inbound) ===\n");
    simulate_morning_traffic(sim);
    
    // จำลองการจราจรไปสักพัก
    printf("\nSimulating traffic...\n");
    for (int i = 0; i < 300; i++) {
        update_simulation(sim);
        
        // แสดงสถานะทุก 60 วินาที
        if (i % 60 == 0) {
            print_simulation_status(sim);
        }
        
        // หน่วงเวลาเพื่อให้ดูผลการจำลองได้ง่ายขึ้น
        usleep(10000); // หน่วงเวลา 10 มิลลิวินาที
    }
    
    // วิเคราะห์ผลการจำลองช่วงเช้า
    printf("\n=== Morning Traffic Simulation Results ===\n");
    analyze_simulation_results(sim);
    
    // เคลียร์ยานพาหนะเก่าออกไป (จำลองให้ทุกคันถึงจุดหมายแล้ว)
    for (int i = 0; i < sim->num_vehicles; i++) {
        sim->vehicles[i].completed = true;
    }
    
    // จำลองการจราจรช่วงเย็น (ออกจากเมือง)
    printf("\n=== Evening Rush Hour (Outbound) ===\n");
    simulate_evening_traffic(sim);
    
    // จำลองการจราจรไปสักพัก
    printf("\nSimulating traffic...\n");
    for (int i = 0; i < 300; i++) {
        update_simulation(sim);
        
        // แสดงสถานะทุก 60 วินาที
        if (i % 60 == 0) {
            print_simulation_status(sim);
        }
        
        // หน่วงเวลาเพื่อให้ดูผลการจำลองได้ง่ายขึ้น
        usleep(10000); // หน่วงเวลา 10 มิลลิวินาที
    }
    
    
    printf("\n=== Evening Traffic Simulation Results ===\n");
    analyze_simulation_results(sim);
    
    
    printf("\n=== End of Traffic Simulation ===\n");
    stop_simulation(sim);
    
    // ทดลองหาเส้นทางที่ดีที่สุด
    printf("\n=== Testing Optimal Route Finding ===\n");
    
    printf("\nShortest path from North Intersection (1) to Business District (5):\n");
    Route* shortest_route = find_shortest_path(graph, 1, 5);
    print_route(graph, shortest_route);
    
    printf("\nLeast congested path from North Intersection (1) to Business District (5):\n");
    Route* least_congested_route = find_least_congested_path(graph, 1, 5);
    print_route(graph, least_congested_route);
    
    printf("\nFastest path from North Intersection (1) to Business District (5):\n");
    Route* fastest_route = find_fastest_path(graph, 1, 5);
    print_route(graph, fastest_route);
    
    printf("\nOptimal path based on multiple factors from North Intersection (1) to Business District (5):\n");
    Route* optimal_route = find_optimal_path(graph, 1, 5, 0.6, 0.2, 0.2);
    print_route(graph, optimal_route);
    
    // เปรียบเทียบเส้นทาง
    printf("\nComparing shortest path with least congested path:\n");
    compare_routes(graph, shortest_route, least_congested_route);
    
    printf("\nComparing fastest path with optimal multi-factor path:\n");
    compare_routes(graph, fastest_route, optimal_route);
    free_route(shortest_route);
    free_route(least_congested_route);
    free_route(fastest_route);
    free_route(optimal_route);
    free_simulation(sim);
    free_signal_system(signal_system);
    free_graph(graph);
    
    printf("\nProgram finished\n");
    
    return 0;
}
