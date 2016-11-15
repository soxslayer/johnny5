#pragma once

#define NAKED __attribute__((naked))
#define ISR __attribute__((isr))

ISR void bad_vector();
