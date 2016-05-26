/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <CL/cl.h>
#include <android/log.h>
#include <stdio.h>
#include <time.h>
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */

#define LOG    "hello-jni"
#define LOGTIME    "hello-time"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)
#define LOGT(...)  __android_log_print(ANDROID_LOG_DEBUG,LOGTIME,__VA_ARGS__)
#define RAND_MAX 100

void sgemmInitialize (int rowa,int cola, int colb, float* matrixA, float* matrixB, float * matrixC)
{
    for (int i = 0; i < rowa; i++)
    {
        for (int j = 0; j < cola; j++)
        {
            int index = i * cola + j;

            /* Keep the values in the range [-1, 1]. */
            float randomeNumber = 0.3+0.7*index/cola/rowa;
            matrixA[index] = randomeNumber;

        }
    }
    for (int i = 0; i < cola; i++)
    {
        for (int j = 0; j < colb; j++)
        {
            int index = i * colb + j;

            /* Keep the values in the range [-1, 1]. */
            float randomeNumber = 0.5+0.5*index/cola/colb;;
            matrixB[index] = randomeNumber;

        }
    }
    for (int i = 0; i < rowa; i++)
    {
        for (int j = 0; j < colb; j++)
        {
            int index = i * colb + j;

            /* Keep the values in the range [-1, 1]. */
            float randomeNumber = 0.7+0.3*index/colb/rowa;;
            matrixC[index] = randomeNumber;
        }
    }
}

int createContext(cl_context* context)
{
    cl_int errorNumber = 0;
    cl_uint numberOfPlatforms = 0;
    cl_platform_id firstPlatformID = 0;

    /* Retrieve a single platform ID. */
    clGetPlatformIDs(1, &firstPlatformID, &numberOfPlatforms);

    /* Get a context with a GPU device from the platform found above. */
    cl_context_properties contextProperties [] = {CL_CONTEXT_PLATFORM, (cl_context_properties)firstPlatformID, 0};
    *context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errorNumber);

    LOGD("createContext code %d",errorNumber);
    return 1;
}

int createCommandQueue(cl_context context, cl_command_queue* commandQueue, cl_device_id* device)
{
    cl_int errorNumber = 0;
    size_t deviceBufferSize = 8;
    size_t deviceNum = 0;
    /* Retrieve the size of the buffer needed to contain information about the devices in this OpenCL context. */
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);

    /* Retrieve the list of devices available in this context. */
    cl_device_id devices[2];
    clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, &deviceNum);
    LOGD("deviceNum %d",deviceNum);
    /* Use the first available device in this context. */
    *device = devices[0];

    /* Set up the command queue with the selected device. */
    *commandQueue = clCreateCommandQueue(context, *device, CL_QUEUE_PROFILING_ENABLE, &errorNumber);

    LOGD("createCommandQueue code %d",errorNumber);
    char str[25];
    size_t str_size = 0;
    clGetDeviceInfo(*device,CL_DEVICE_NAME,25,str,&str_size);
    LOGD("device name %s",str);
    return 1;
}

int createProgram(cl_context context, cl_device_id device, char * filename, cl_program* program)
{
    cl_int errorNumber = 0;
    char charSource[4096];
    FILE * file=NULL;
    file = fopen(filename,"r+");
    if(!file) {
    	LOGD("openfile error");
    }
    fseek(file,0L,SEEK_END);
    int flen = ftell(file);
    LOGD("file length %d",flen);
    fseek(file,0L,SEEK_SET);
    fread(charSource,flen,1,file);
    charSource[flen]=0;
    LOGD("%s",&(charSource[2500]));
    char * charsourcePoint = charSource;
    *program = clCreateProgramWithSource(context, 1, &charsourcePoint, NULL, &errorNumber);
    LOGD("createProgram code %d",errorNumber);
    errorNumber = clBuildProgram(*program, 0, NULL, NULL, NULL, NULL);
    LOGD("buildProgram code %d",errorNumber);

    size_t logSize = 0;
    clGetProgramBuildInfo(*program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
    LOGD("build log size %d",logSize);

    if (logSize > 1)
    {
        char log[4096];
        clGetProgramBuildInfo(*program, device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);
    }

    return 1;
}

float printProfilingInfo(cl_event event)
{
    cl_ulong queuedTime = 0;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queuedTime, NULL);

    cl_ulong submittedTime = 0;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submittedTime, NULL);

    cl_ulong startTime = 0;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, NULL);

    cl_ulong endTime = 0;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);

    LOGD("Profiling information");
    /* OpenCL returns times in nano seconds. Print out the times in milliseconds (divide by a million). */
    LOGD("Queued time: %8.6f ms",1e-6*(submittedTime - queuedTime));
    LOGD("Wait time: %8.6f ms",1e-6*(startTime - submittedTime));
    LOGD("Run time: %8.6f ms",1e-6*(endTime - startTime));
    LOGD("queued time: %lld",queuedTime);
    LOGD("submitted time: %lld",submittedTime);
    LOGD("start time: %lld",startTime);
    LOGD("end time: %lld",endTime);
    return (1e-6*(endTime - submittedTime));
}

int cleanUpOpenCL(cl_context context, cl_command_queue commandQueue, cl_program program, cl_kernel kernel, cl_mem memoryObjectsa,cl_mem memoryObjectsb,cl_mem memoryObjectsc, int numberOfMemoryObjects)
{
	clReleaseContext(context);
	clReleaseCommandQueue(commandQueue);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(memoryObjectsa);
	clReleaseMemObject(memoryObjectsb);
	clReleaseMemObject(memoryObjectsc);

    return 0;
}

float sgemmMain(int rowa,int cola,int colb)
{
	 cl_context context = 0;
	 cl_command_queue commandQueue = 0;
	 cl_program program = 0;
	 cl_device_id device = 0;
	 cl_kernel kernel = 0;
	 const unsigned int numberOfMemoryObjects = 3;
	 cl_mem memoryObjectsa = 0;
	 cl_mem memoryObjectsb = 0;
	 cl_mem memoryObjectsc = 0;
	 cl_int errorNumber;
	 cl_uint clrowa = rowa;
	 cl_uint clcola = cola;
	 cl_uint clcolb = colb;
	 int err;
	 err = createContext(&context);
	 LOGD("create context");
	 err = createCommandQueue(context, &commandQueue, &device);
	 err = createProgram(context, device, "/mnt/sdcard/kernel/sgemm.cl", &program);
	 kernel = clCreateKernel(program, "sgemm", &errorNumber);
	 LOGD("createKernel code %d",errorNumber);
	 LOGD("start computing");
	 float alpha = 1;
	 float beta = 0.1;

	 /* Create the matrices. */
	 size_t matrixSizea = rowa * cola;
	 size_t matrixSizeb = cola * colb;
	 size_t matrixSizec = rowa * colb;

	 /* As all the matrices have the same size, the buffer size is common. */
	 size_t bufferSizea = matrixSizea * sizeof(float);
	 size_t bufferSizeb = matrixSizeb * sizeof(float);
	 size_t bufferSizec = matrixSizec * sizeof(float);

	 /* Create buffers for the matrices used in the kernel. */
	 int createMemoryObjectsSuccess = 0;
	 memoryObjectsa = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, bufferSizea, NULL, &errorNumber);
	 createMemoryObjectsSuccess &= errorNumber;
	 memoryObjectsb = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, bufferSizeb, NULL, &errorNumber);
	 createMemoryObjectsSuccess &= errorNumber;
	 memoryObjectsc = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, bufferSizec, NULL, &errorNumber);
	 createMemoryObjectsSuccess &= errorNumber;
	 LOGD("create memory err %d",createMemoryObjectsSuccess);
	 int mapMemoryObjectsSuccess = 0;
	 cl_float* matrixA = (cl_float*)clEnqueueMapBuffer(commandQueue, memoryObjectsa, CL_TRUE, CL_MAP_WRITE, 0, bufferSizea, 0, NULL, NULL, &errorNumber);
	 mapMemoryObjectsSuccess &= errorNumber;
	 cl_float* matrixB = (cl_float*)clEnqueueMapBuffer(commandQueue, memoryObjectsb, CL_TRUE, CL_MAP_WRITE, 0, bufferSizeb, 0, NULL, NULL, &errorNumber);
	 mapMemoryObjectsSuccess &= errorNumber;
	 cl_float* matrixC = (cl_float*)clEnqueueMapBuffer(commandQueue, memoryObjectsc, CL_TRUE, CL_MAP_WRITE, 0, bufferSizec, 0, NULL, NULL, &errorNumber);
	 mapMemoryObjectsSuccess &= errorNumber;
	 LOGD("map memory err %d",mapMemoryObjectsSuccess);

	 sgemmInitialize(rowa,cola,colb, matrixA, matrixB, matrixC);
	 LOGD("data initial finish");
	 int unmapMemoryObjectsSuccess = 0;
	 errorNumber = clEnqueueUnmapMemObject(commandQueue, memoryObjectsa, matrixA, 0, NULL, NULL);
	 LOGD("memory code %d",errorNumber);
	 unmapMemoryObjectsSuccess &= errorNumber;
	 errorNumber = clEnqueueUnmapMemObject(commandQueue, memoryObjectsb, matrixB, 0, NULL, NULL);
	 LOGD("memory code %d",errorNumber);
	 unmapMemoryObjectsSuccess &= errorNumber;
	 errorNumber = clEnqueueUnmapMemObject(commandQueue, memoryObjectsc, matrixC, 0, NULL, NULL);
	 LOGD("memory code %d",errorNumber);
	 unmapMemoryObjectsSuccess &= errorNumber;
	 LOGD("unmap memory err %d",unmapMemoryObjectsSuccess);

	 int setKernelArgumentsSuccess = 0;
	 errorNumber = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memoryObjectsa);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 1, sizeof(cl_mem), &memoryObjectsb);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 2, sizeof(cl_mem), &memoryObjectsc);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 3, sizeof(cl_uint), &clrowa);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 4, sizeof(cl_uint), &clcola);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 5, sizeof(cl_uint), &clcolb);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 6, sizeof(cl_float), &alpha);
	 setKernelArgumentsSuccess &= errorNumber;
	 errorNumber = clSetKernelArg(kernel, 7, sizeof(cl_float), &beta);
	 setKernelArgumentsSuccess &= errorNumber;
	 LOGD("setKernel err %d",setKernelArgumentsSuccess);

	 LOGD("start running kernel");
	 clock_t start_t,end_t;
	 float cost_time;
	 start_t = clock();
	 cl_event event = 0;
	 size_t globalWorksize[2] = {rowa, colb};
	 errorNumber = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalWorksize, NULL, 0, NULL, &event);
	 //LOGD("Enqueue err code %d",errorNumber);
	 errorNumber = clFinish(commandQueue);
	 end_t = clock();
	 cost_time = (float)(end_t-start_t)/CLOCKS_PER_SEC*1000;
	 LOGD("Finish err code %d",errorNumber);
	 float time;
	 time = printProfilingInfo(event);
	 LOGT("using CPU clock: %f ms",cost_time);
	 LOGT("using GPU clock: %f ms",time);
	 clReleaseEvent(event);
	 matrixC = (cl_float*)clEnqueueMapBuffer(commandQueue, memoryObjectsc, CL_TRUE, CL_MAP_READ, 0, bufferSizec, 0, NULL, NULL, &errorNumber);
	 clEnqueueUnmapMemObject(commandQueue, memoryObjectsc, matrixC, 0, NULL, NULL);
	 LOGD("read out matrixC finish");
	 LOGD("matrixC value C(0,0): %f",matrixC[0]);
	 cleanUpOpenCL(context, commandQueue, program, kernel, memoryObjectsa, memoryObjectsb,memoryObjectsc,numberOfMemoryObjects);
	 LOGD("RUNNING finsh");
	 return time;
}

jstring
Java_com_example_hellojni_HelloJni_calculateByOpenCL( JNIEnv* env,
                                                  jobject thiz,
                                                  jint jrowa, jint jcola, jint jcolb)
{
	int rowa = jrowa;
	int cola = jcola;
	int colb = jcolb;
	LOGD("A: %dx%d",rowa,cola);
	LOGD("B: %dx%d",cola,colb);
	LOGD("C: %dx%d",rowa,colb);
	cl_int err = 0;
	cl_uint numberOfPlatforms = 0;
	cl_platform_id firstPlatformID = 0;
	err = clGetPlatformIDs(1, &firstPlatformID, &numberOfPlatforms);
	cl_uint numOfDevices = 0;
	cl_device_id firstDeviceID;
	err = clGetDeviceIDs(firstPlatformID,CL_DEVICE_TYPE_ALL,1,&firstDeviceID,&numOfDevices);
	char str[20];
	sprintf(str,"%d",numOfDevices);
	LOGD("startSgemmMain");
	float time;
	time = sgemmMain(rowa,cola,colb);
	sprintf(str,"%12.6f",time);
	return (*env)->NewStringUTF(env, str);
}

jstring
Java_com_example_hellojni_HelloJni_getDeviceInfo( JNIEnv* env,
                                                  jobject thiz )
{

    cl_int errorNumber = 0;
    cl_uint numberOfPlatforms = 0;
    cl_platform_id firstPlatformID = 0;

    /* Retrieve a single platform ID. */
    clGetPlatformIDs(1, &firstPlatformID, &numberOfPlatforms);
    cl_device_id firstdevice = 0;
    cl_uint numberOfdevices = 0;
    clGetDeviceIDs(firstPlatformID,CL_DEVICE_TYPE_ALL,1,&firstdevice,&numberOfdevices);
    char str[25];
    size_t str_size = 0;
    clGetDeviceInfo(firstdevice,CL_DEVICE_NAME,25,str,&str_size);
    return (*env)->NewStringUTF(env, str);
}
