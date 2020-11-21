################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
./src/MultiACS.cpp \
./src/MultiACSParameters.cpp \
./src/Reader.cpp \
./src/StackDGenerator.cpp \
./src/Tools.cpp \
./src/Writer.cpp \
./src/main.cpp 

OBJS += \
./src/MultiACS.o \
./src/MultiACSParameters.o \
./src/Reader.o \
./src/StackDGenerator.o \
./src/Tools.o \
./src/Writer.o \
./src/main.o 

CPP_DEPS += \
./src/MultiACS.d \
./src/MultiACSParameters.d \
./src/Reader.d \
./src/StackDGenerator.d \
./src/Tools.d \
./src/Writer.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fopenmp -fno-stack-protector -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


