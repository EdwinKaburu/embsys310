/*
 * FuelTank.cpp
 *
 *  Created on: Jun 17, 2021
 *      Author: EdwinKaburu
 */

#include <FuelTank.h>

namespace APP {

// Default Main Tank with Grades along with Levels/Capacities/Rates.

FuelTank MAIN_TANK = {
		{
				{ 250, GRADE_87, 0.25 },
				{ 250, GRADE_89, 0.35 },
				{ 250, GRADE_91, 0.45 },
				{ 250, GRADE_93, 0.55 }
		},
		1000, 0.10
};

}
