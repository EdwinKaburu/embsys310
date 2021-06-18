/*
 * FuelTank.h
 *
 *  Created on: Jun 17, 2021
 *      Author: EdwinKaburu
 */
// FUEL_PUMP_INTERFACE_H
#ifndef FUEL_TANK_INTERFACE_H
#define FUEL_TANK_INTERFACE_H


//#include "bsp.h"
#include <stdint.h>
#include "fw_log.h"
#include "qassert.h"

//#define GPIO_PATTERN_ASSERT(t_) ((t_)? (void)0: Q_onAssert("GpioPattern.h", (int32_t)__LINE__))

namespace APP {

// Fuel Grades To be Used.
typedef enum {
    GRADE_87 = 87,
    GRADE_89 = 89,
    GRADE_91 = 91,
    GRADE_93 = 93
} Grade;

// Fuel Grade Capacity
enum {
    FUEL_GRADE_CAPACITY = 4
};

class FuelGrade {
public:
    uint16_t c_grade_capacity;
    Grade fuel_grade;

    Grade GetGrade() const {
        return fuel_grade;
    }

    uint16_t GetGradeCapacity() const {
        return c_grade_capacity;;
    }

    void SetGradeCapacity(uint16_t new_capacity) {
        c_grade_capacity = new_capacity;
    }

};

class FuelTank {
public:

    FuelGrade tank_grades[FUEL_GRADE_CAPACITY];

    // Do not Call / Access , It is not stable / constant
    uint16_t c_tank_capacity;


    uint16_t GetUpdateFuelTankCapacity() {
        // Capacity Changes, so tank_capacity cannot be constant.

        FuelGrade *grade; // Get Grade
        c_tank_capacity = 0; // Rest Tank Capacity

        for (int a = 0; a < FUEL_GRADE_CAPACITY; a++) {
            grade = &tank_grades[a];
            c_tank_capacity += grade->GetGradeCapacity();
        }

        return c_tank_capacity;
    }

    FuelGrade *GetFuelGradeI(int index)
    {
        if(index < FUEL_GRADE_CAPACITY)
        {
            return &tank_grades[index];
        }
        return NULL;
    }

    FuelGrade *GetFuelGradeG(Grade fg) {
        for (int a = 0; a < FUEL_GRADE_CAPACITY; a++) {
            if (tank_grades[a].GetGrade() == fg) {
                return &tank_grades[a];
            }
        }
        return NULL;
    }

};

// Capacity or Levels of each Fuel Grade Tank Can Change
extern FuelTank MAIN_TANK;

}


#endif /* FUEL_TANK_INTERFACE_H */
