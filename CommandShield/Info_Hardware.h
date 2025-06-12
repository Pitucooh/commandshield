#ifndef INFO_HARDWARE_H
#define INFO_HARDWARE_H

int handle_hardware_command(const char* command);

void list_device_drivers();
void show_memory_info();
void show_cpu_info();
void show_disk_info();
void show_network_info();
void show_usb_info();
void show_process_info();

#endif // HARDWARE_INFO_H
