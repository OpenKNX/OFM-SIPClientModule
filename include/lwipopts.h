#pragma once
#if ARDUINO_ARCH_ESP32     
#include "lwipoptsESP32.h"
#else
#include "lwipoptsRP2040.h"
#endif