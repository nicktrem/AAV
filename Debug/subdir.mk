################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../FilesystemTools.cpp \
../LetterIdentifier.cpp \
../Logger.cpp \
../MetadataImageReader.cpp \
../TargetFinder.cpp \
../main.cpp 

OBJS += \
./FilesystemTools.o \
./LetterIdentifier.o \
./Logger.o \
./MetadataImageReader.o \
./TargetFinder.o \
./main.o 

CPP_DEPS += \
./FilesystemTools.d \
./LetterIdentifier.d \
./Logger.d \
./MetadataImageReader.d \
./TargetFinder.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -I/usr/local/include/opencv4 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


