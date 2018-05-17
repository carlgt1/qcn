#include <mach/mach.h>
#include <mach/mach_error.h>
#include <stdio.h>

int main() {
  	natural_t cpuCount;
	processor_info_array_t infoArray;
	mach_msg_type_number_t infoCount;

	kern_return_t error = host_processor_info(mach_host_self(),
		PROCESSOR_CPU_LOAD_INFO, &cpuCount, &infoArray, &infoCount);
	if (error) {
		mach_error("host_processor_info error:", error);
		return error;
	}

	processor_cpu_load_info_data_t* cpuLoadInfo =
		(processor_cpu_load_info_data_t*) infoArray;

	printf("Mach CPU Load Statistics: (100 ticks per CPU per second)\n");
	unsigned long totalUseTicks = 0;
	unsigned long totalTicks = 0;

	const char* stateName[] = { "user", "syst", "idle", "nice" };

	for (int cpu=0; cpu<cpuCount; cpu++)
		for (int state=0; state<CPU_STATE_MAX; state++) {
			unsigned long ticks = 
                         cpuLoadInfo[cpu].cpu_ticks[state];
			printf("CPU %d %s: %10lu.\n", cpu, 
stateName[state], ticks);
                        if (state != 2)
                           totalUseTicks += ticks;

			totalTicks += ticks;
		}

	printf("CPU Usage:  %f %\n", 100.0f * ((float)(totalUseTicks) / (float) totalTicks));
	printf("Total ticks:%10lu.\n", totalTicks);

	vm_deallocate(mach_task_self(), (vm_address_t)infoArray, infoCount);
	return error;
}

