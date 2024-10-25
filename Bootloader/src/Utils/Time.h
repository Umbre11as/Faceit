#pragma once

#define MILLIS_TO_MICRO(x) (x * 1000)
#define SEC_TO_MICRO(x) MILLIS_TO_MICRO(x * 1000)
#define MIN_TO_MICRO(x) SEC_TO_MICRO(x * 60)
