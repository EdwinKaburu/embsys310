/*
 * FuelPump.h
 *
 *  Created on: Jun 14, 2021
 *      Author: EdwinKaburu
 */

#ifndef FUEL_PUMP_H
#define FUEL_PUMP_H


#include "qpcpp.h"
#include "fw_region.h"
#include "fw_timer.h"
#include "fw_evt.h"
#include "app_hsmn.h"
#include "FuelTank.h"

using namespace QP;
using namespace FW;

namespace APP {

class FuelPump: public Active{

public:
	FuelPump();

protected:
	static QState InitialPseudoState(FuelPump * const me, QEvt const * const e);
	    static QState Root(FuelPump * const me, QEvt const * const e);
	        static QState Stopped(FuelPump * const me, QEvt const * const e);
	        static QState Starting(FuelPump * const me, QEvt const * const e);
	        static QState Stopping(FuelPump * const me, QEvt const * const e);
	        static QState Started(FuelPump * const me, QEvt const * const e);
	           static QState Idle(FuelPump * const me, QEvt const * const e);
	           static QState IdleDrawing(FuelPump * const me, QEvt const * const e);
	           static QState Passive(FuelPump * const me, QEvt const * const e);
	              static QState Waiting(FuelPump * const me, QEvt const * const e);
	              static QState Drawing(FuelPump * const me, QEvt const * const e);
	              static QState Pumping(FuelPump * const me, QEvt const * const e);
	                 static QState Filling(FuelPump * const me, QEvt const * const e);
	                 static QState ReDrawing(FuelPump * const me, QEvt const * const e);
	              static QState Admission(FuelPump * const me, QEvt const * const e);

Timer m_timeoutTimer;
Timer m_pumpTimer;

float m_price;
float m_gallons;

bool m_paid;
bool m_graded;

bool m_isbtn;

uint8_t m_payment;
uint8_t m_max_amount;

uint32_t m_grade;

FuelTank  &m_currTank;
FuelGrade  *m_currGrade;

enum{
	ACTIVE_TIME_OUT = 20000,
	PUMP_TIME_OUT = 10000,
	NODEUP_TIME_OUT = 10000
};

#define FUEL_PUMP_TIMER_EVT \
    ADD_EVT(TIME_OUT_TIMER) \
    ADD_EVT(PUMP_TIMER) \


#define FUEL_PUMP_INTERNAL_EVT \
    ADD_EVT(DONE) \
    ADD_EVT(REDRAW)\
	ADD_EVT(MDRAW) \
	ADD_EVT(GDRAW) \
	ADD_EVT(FUDRAW)


#undef ADD_EVT
#define ADD_EVT(e_) e_,

    enum {
        FUEL_PUMP_EVT_START = TIMER_EVT_START(FUEL_PUMP),
        FUEL_PUMP_TIMER_EVT
    };

    enum {
        FUEL_PUMP_INTERNAL_EVT_START = INTERNAL_EVT_START(FUEL_PUMP),
        FUEL_PUMP_INTERNAL_EVT
    };

};

} //namespace APP

#endif /* FUEL_PUMP_H */
