#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <string>

#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)
#define SIZE 2048 // column/row size. This is changed for every run
#define TILE_SIZE 8 // column/row size in a tile. It can only be 1,2,4,8,16 because of the limitation from the GPU

// C version of matrix multiplcation. Use this function for result validation and execution time comaprison
void matrix_mul_sequence (int *A_mat,
                          int *B_mat,
                          int *C_mat)
{
	for (int j=0; j<SIZE; j++) {
		for (int i=0; i<SIZE; i++)
			for (int k=0; k<SIZE; k++)
				C_mat[j*SIZE + i] += A_mat[j*SIZE + k] * B_mat[k*SIZE + i];
	}
}

int main(void)
{
    // A, B are input matrix, C is the output matrix for OpenCL, C_seq is the output matrix for reference implementation.
    int *A = new int[SIZE*SIZE];
    int *B = new int[SIZE*SIZE];
    int *C = new int[SIZE*SIZE];
    int *C_seq = new int[SIZE*SIZE];

    //Initialize matrix
    for(int j=0; j<SIZE; j++) {
		for(int i=0; i<SIZE; i++) {
			A[j*SIZE + i] = rand() % 20;
			B[j*SIZE + i] = rand() % 20;
			C[j*SIZE + i] = 0;
			C_seq[j*SIZE + i] = 0;
		}
    }

	std::chrono::high_resolution_clock::time_point t1, t2;
	t1 = std::chrono::high_resolution_clock::now();
    matrix_mul_sequence(A, B, C_seq);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Reference C matrix multiplication: "
		<< (float)(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count())/1000000
		<< " sec"
		<< std::endl;
	// this stays the same

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("matrix_mul.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices); // get gpu device, need to do same for cpu

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret); // create gpu context, need to do same for cpu

    // Create a command queue with the capability of performance profiling for target device
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret); // need to create command queue for cpu

    // Create memory buffers on the device for each matrix
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE*SIZE*sizeof(int), NULL, &ret); // need to create memory buffers for specific device
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE*SIZE*sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE*SIZE*sizeof(int), NULL, &ret);

    // Copy the matrix A, B and C to each device memory counterpart
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), A, 0, NULL, NULL); // copy matrix into memory buffers, same for cpu. No need to re-initialize matixes then.
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), B, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), C, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret); // need to create kernel for cpu as well

    // Build and compile the OpenCL kernel program
    std::string build_option = "-DTILE_SIZE=" + std::to_string(TILE_SIZE);
    ret = clBuildProgram(program, 1, &device_id, build_option.c_str(), NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE) { // If compile failed, print the error message
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *) malloc(log_size);

		// Get the log and print it
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("%s\n", log);
	}

    // Create the OpenCL kernel
    cl_kernel kernel;
	kernel = clCreateKernel(program, "matrix_mul", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    int dimention = 2; // In this example, We will use 2 dimention index
    size_t global_item_size[] = {SIZE, SIZE, 1};
    size_t local_item_size[] = {TILE_SIZE, TILE_SIZE, 1};

	cl_event perf_event;
	cl_ulong start, end;

	// Execute the OpenCL kernel
    ret = clEnqueueNDRangeKernel(command_queue, kernel, dimention, NULL, global_item_size, local_item_size, 0, NULL, &perf_event);
    // Capture performance event from target device. In this case the event is to retrive the execution time.
    ret = clWaitForEvents(1, &perf_event);
    ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
    ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);
	std::cout << "OpenCL GPU matrix multiplication: " << (float)(end - start)/1000000000 << " sec" << std::endl;

    // Read the memory buffer C from the device into the local variable C
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), C, 0, NULL, NULL);

	// Make sure all the command in the command queue has been executed
    ret = clFinish(command_queue);

    bool validate = true;
    for(int j=0; j<SIZE; j++) {
		for(int i=0; i<SIZE; i++) {
			if (C[j*SIZE + i] != C_seq[j*SIZE + i])
				validate = false;
		}
	}

	if (validate == false)
		std::cout << "The results are mismatched !!" << std::endl;

	/************************************************************
	 Stays with gpu context. Do tile multiplication now
	*************************************************************/

	ret = clReleaseKernel(kernel); // need to clean kernel, nothing else.

								   // Create the OpenCL kernel
	kernel = clCreateKernel(program, "matrix_mul_tile", &ret);

	// Set the arguments of the kernel, same arguments as before
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

	dimention = 2; // In this example, We will use 2 dimention index, passing same arguments as before
	size_t global_item_size2[] = { SIZE, SIZE, 1 };
	size_t local_item_size2[] = { TILE_SIZE, TILE_SIZE, 1 };


	cl_event perf_event2;
	cl_ulong start2, end2;

	// Execute the OpenCL kernel
	ret = clEnqueueNDRangeKernel(command_queue, kernel, dimention, NULL, global_item_size2, local_item_size2, 0, NULL, &perf_event2);
	// Capture performance event from target device. In this case the event is to retrive the execution time.
	ret = clWaitForEvents(1, &perf_event2);
	ret = clGetEventProfilingInfo(perf_event2, CL_PROFILING_COMMAND_START, sizeof(start2), &start2, NULL);
	ret = clGetEventProfilingInfo(perf_event2, CL_PROFILING_COMMAND_END, sizeof(end2), &end2, NULL);
	std::cout << "OpenCL GPU tile matrix multiplication: " << (float)(end2 - start2) / 1000000000 << " sec" << std::endl;

	// Read the memory buffer C from the device into the local variable C
	ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE * sizeof(int), C, 0, NULL, NULL);

	// Make sure all the command in the command queue has been executed
	ret = clFinish(command_queue);

	validate = true;
	for (int j = 0; j<SIZE; j++) {
		for (int i = 0; i<SIZE; i++) {
			if (C[j*SIZE + i] != C_seq[j*SIZE + i])
				validate = false;
		}
	}

	if (validate == false)
		std::cout << "The results are mismatched !!" << std::endl;


    // Clean up
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

	/************************************************************
		 Restart context. Do CPU Runs
	*************************************************************/
	cl_platform_id cpuplatform_id = NULL;
	cl_device_id cpudevice_id = NULL;
	cl_uint cpuret_num_devices;
	cl_uint cpuret_num_platforms;
	cl_int cpuret = clGetPlatformIDs(1, &cpuplatform_id, &cpuret_num_platforms);
	cpuret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &cpudevice_id, &cpuret_num_devices); //cpu device

																							// Create an OpenCL context
	cl_context cpucontext = clCreateContext(NULL, 1, &cpudevice_id, NULL, NULL, &cpuret); // create gpu context, need to do same for cpu

																				 // Create a command queue with the capability of performance profiling for target device
	cl_command_queue cpucommand_queue = clCreateCommandQueue(cpucontext, cpudevice_id, CL_QUEUE_PROFILING_ENABLE, &cpuret); // need to create command queue for cpu

																												// Create memory buffers on the device for each matrix
	a_mem_obj = clCreateBuffer(cpucontext, CL_MEM_READ_WRITE, SIZE*SIZE * sizeof(int), NULL, &cpuret); // need to create memory buffers for specific device
	b_mem_obj = clCreateBuffer(cpucontext, CL_MEM_READ_WRITE, SIZE*SIZE * sizeof(int), NULL, &cpuret);
	c_mem_obj = clCreateBuffer(cpucontext, CL_MEM_READ_WRITE, SIZE*SIZE * sizeof(int), NULL, &cpuret);

	// Copy the matrix A, B and C to each device memory counterpart
	ret = clEnqueueWriteBuffer(cpucommand_queue, a_mem_obj, CL_TRUE, 0, SIZE*SIZE * sizeof(int), A, 0, NULL, NULL); // copy matrix into memory buffers, same for cpu. No need to re-initialize matixes then.
	ret = clEnqueueWriteBuffer(cpucommand_queue, b_mem_obj, CL_TRUE, 0, SIZE*SIZE * sizeof(int), B, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(cpucommand_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE * sizeof(int), C, 0, NULL, NULL);

	// Create a program from the kernel source
	cl_program cpuprogram = clCreateProgramWithSource(cpucontext, 1, (const char **)&source_str, (const size_t *)&source_size, &cpuret); // need to create kernel for cpu as well

																																// Build and compile the OpenCL kernel program
	build_option = "-DTILE_SIZE=" + std::to_string(TILE_SIZE);
	cpuret = clBuildProgram(cpuprogram, 1, &cpudevice_id, build_option.c_str(), NULL, NULL);
	if (cpuret == CL_BUILD_PROGRAM_FAILURE) { // If compile failed, print the error message
										   // Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(cpuprogram, cpudevice_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *)malloc(log_size);

		// Get the log and print it
		clGetProgramBuildInfo(cpuprogram, cpudevice_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("%s\n", log);
	}

	// Create the OpenCL kernel
	cl_kernel cpukernel;
	cpukernel = clCreateKernel(cpuprogram, "matrix_mul", &cpuret);

	// Set the arguments of the kernel
	cpuret = clSetKernelArg(cpukernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	cpuret = clSetKernelArg(cpukernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	cpuret = clSetKernelArg(cpukernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

	dimention = 2; // In this example, We will use 2 dimention index
	size_t cpuglobal_item_size[] = { SIZE, SIZE, 1 };
	size_t cpulocal_item_size[] = { TILE_SIZE, TILE_SIZE, 1 };

	cl_event cpuperf_event;
	cl_ulong cpustart, cpuend;

	// Execute the OpenCL kernel
	cpuret = clEnqueueNDRangeKernel(cpucommand_queue, cpukernel, dimention, NULL, cpuglobal_item_size, cpulocal_item_size, 0, NULL, &cpuperf_event);
	// Capture performance event from target device. In this case the event is to retrive the execution time.
	cpuret = clWaitForEvents(1, &cpuperf_event);
	cpuret = clGetEventProfilingInfo(cpuperf_event, CL_PROFILING_COMMAND_START, sizeof(cpustart), &cpustart, NULL);
	cpuret = clGetEventProfilingInfo(cpuperf_event, CL_PROFILING_COMMAND_END, sizeof(cpuend), &cpuend, NULL);
	std::cout << "OpenCL CPU matrix multiplication: " << (float)(cpuend - cpustart) / 1000000000 << " sec" << std::endl;

	// Read the memory buffer C from the device into the local variable C
	cpuret = clEnqueueReadBuffer(cpucommand_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE * sizeof(int), C, 0, NULL, NULL);

	// Make sure all the command in the command queue has been executed
	cpuret = clFinish(cpucommand_queue);

	validate = true;
	for (int j = 0; j<SIZE; j++) {
		for (int i = 0; i<SIZE; i++) {
			if (C[j*SIZE + i] != C_seq[j*SIZE + i])
				validate = false;
		}
	}

	if (validate == false)
		std::cout << "The results are mismatched !!" << std::endl;
	cpuret = clReleaseKernel(cpukernel); // release cpu kernel for tile

	/************************************************************
		 Noe restart only kernel again. Do Tile Multiplication
	*************************************************************/
	// Create the OpenCL kernel
	cpukernel = clCreateKernel(cpuprogram, "matrix_mul_tile", &cpuret); // recreate kernel now with tile program

	// Set the arguments of the kernel
	cpuret = clSetKernelArg(cpukernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	cpuret = clSetKernelArg(cpukernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	cpuret = clSetKernelArg(cpukernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

	dimention = 2; // In this example, We will use 2 dimention index
	size_t cpuglobal_item_size2[] = { SIZE, SIZE, 1 };
	size_t cpulocal_item_size2[] = { TILE_SIZE, TILE_SIZE, 1 };

	cl_event cpuperf_event2;
	cl_ulong cpustart2, cpuend2;

	// Execute the OpenCL kernel
	cpuret = clEnqueueNDRangeKernel(cpucommand_queue, cpukernel, dimention, NULL, cpuglobal_item_size2, cpulocal_item_size2, 0, NULL, &cpuperf_event2);
	// Capture performance event from target device. In this case the event is to retrive the execution time.
	cpuret = clWaitForEvents(1, &cpuperf_event2);
	cpuret = clGetEventProfilingInfo(cpuperf_event2, CL_PROFILING_COMMAND_START, sizeof(cpustart2), &cpustart2, NULL);
	cpuret = clGetEventProfilingInfo(cpuperf_event2, CL_PROFILING_COMMAND_END, sizeof(cpuend2), &cpuend2, NULL);
	std::cout << "OpenCL CPU tile matrix multiplication: " << (float)(cpuend2 - cpustart2) / 1000000000 << " sec" << std::endl;

	// Read the memory buffer C from the device into the local variable C
	cpuret = clEnqueueReadBuffer(cpucommand_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE * sizeof(int), C, 0, NULL, NULL);

	// Make sure all the command in the command queue has been executed
	cpuret = clFinish(cpucommand_queue);

	validate = true;
	for (int j = 0; j<SIZE; j++) {
		for (int i = 0; i<SIZE; i++) {
			if (C[j*SIZE + i] != C_seq[j*SIZE + i])
				validate = false;
		}
	}

	if (validate == false)
		std::cout << "The results are mismatched !!" << std::endl;



	cpuret = clReleaseKernel(cpukernel);
	cpuret = clReleaseProgram(cpuprogram);
	cpuret = clReleaseMemObject(a_mem_obj);
	cpuret = clReleaseMemObject(b_mem_obj);
	cpuret = clReleaseMemObject(c_mem_obj);
	cpuret = clReleaseCommandQueue(cpucommand_queue);
	cpuret = clReleaseContext(cpucontext);


	std::cout << "Press Enter to finish..." << std::endl;
	getchar();
    return 0;
}
