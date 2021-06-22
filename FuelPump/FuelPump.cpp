/*
 * FuelPump.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: EdwinKaburu
 */
#include "app_hsmn.h"
#include "fw_log.h"
#include "fw_assert.h"
#include "DispInterface.h"
#include "fw_active.h"
#include "GpioOutInterface.h"
#include "FuelPumpInterface.h"
#include "FuelPump.h"
#include "FuelTank.h"

FW_DEFINE_THIS_FILE("FuelPump.cpp")

namespace APP {

#undef ADD_EVT
#define ADD_EVT(e_) #e_,

static char const *const timerEvtName[] = { "FUEL_PUMP_TIMER_EVT_START",
FUEL_PUMP_TIMER_EVT };

static char const *const internalEvtName[] = { "FUEL_PUMP_INTERNAL_EVT_START",
FUEL_PUMP_INTERNAL_EVT };

static char const *const interfaceEvtName[] = { "FUEL_PUMP_INTERFACE_EVT_START",
FUEL_PUMP_INTERFACE_EVT };

static const char *DISMES_STRING[] = { "Insert Payment", "Select Grade",
		"Start Fueling", "Take Receipt", };

FuelPump::FuelPump() :
		Active((QStateHandler) &FuelPump::InitialPseudoState, FUEL_PUMP,
				"FUEL_PUMP"), m_price(0.00f), m_gallons(0.00f), m_max_amount(0), m_payment(
				0), m_paid(false), m_graded(false), m_isbtn(false), m_toUser(
				NULL), m_ingore(false), m_gallon_rate(0.00f), m_price_rate(
				0.00f), m_exit(false), m_grade(0), m_currTank(MAIN_TANK), m_currGrade(
				NULL), m_timeoutTimer(GetHsm().GetHsmn(), TIME_OUT_TIMER), m_pumpTimer(
				GetHsm().GetHsmn(), PUMP_TIMER) {
	SET_EVT_NAME(FUEL_PUMP);
}

QState FuelPump::InitialPseudoState(FuelPump *const me, QEvt const *const e) {
	(void) e;
	return Q_TRAN(&FuelPump::Root);
}

QState FuelPump::Root(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case Q_INIT_SIG: {
		return Q_TRAN(&FuelPump::Stopped);
	}
	case FUEL_PUMP_START_REQ: {
		EVENT(e);
		Evt const &req = EVT_CAST(*e);
		Evt *evt = new FuelPumpStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(),
				ERROR_STATE, GET_HSMN());
		Fw::Post(evt);
		return Q_HANDLED();
	}
	case FUEL_PUMP_STOP_REQ: {
		EVENT(e);
		me->GetHsm().Defer(e);
		return Q_TRAN(&FuelPump::Stopping);
	}

	}
	return Q_SUPER(&QHsm::top);
}

QState FuelPump::Stopped(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case FUEL_PUMP_STOP_REQ: {
		EVENT(e);
		Evt const &req = EVT_CAST(*e);
		Evt *evt = new FuelPumpStopCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(),
				ERROR_SUCCESS);
		Fw::Post(evt);
		return Q_HANDLED();
	}
	case FUEL_PUMP_START_REQ: {
		EVENT(e);
		Evt const &req = EVT_CAST(*e);
		me->GetHsm().SaveInSeq(req);

		return Q_TRAN(&FuelPump::Starting);

		//Evt *evt = new FuelPumpStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(),ERROR_SUCCESS);
		//Fw::Post(evt);
	}
	}
	return Q_SUPER(&FuelPump::Root);
}

QState FuelPump::Starting(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		// ----------- Start Display --------------
		//uint32_t timeout = FuelPumpStartReq::TIMEOUT_MS;
		//FW_ASSERT(timeout > DispStartReq::TIMEOUT_MS);

		me->GetHsm().ResetOutSeq();
		Evt *evt = new DispStartReq(ILI9341, GET_HSMN(), GEN_SEQ());
		me->GetHsm().SaveOutSeq(*evt);

		Fw::Post(evt);

		return Q_HANDLED();
	}
	case DISP_START_CFM: {
		EVENT(e);
		ErrorEvt const &cfm = ERROR_EVT_CAST(*e);
		bool Received;

		if (me->GetHsm().HandleCfmRsp(cfm, Received)) {
			Evt *evt = new Evt(DONE, GET_HSMN());
			me->PostSync(evt);
		}

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case DONE: {
		EVENT(e);

		Evt *evt = new FuelPumpStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(),
				me->GetHsm().GetInSeq(), ERROR_SUCCESS);

		Fw::Post(evt);
		me->GetHsm().ClearInSeq();

		return Q_TRAN(&FuelPump::Started);
	}

	}
	return Q_SUPER(&FuelPump::Root);
}

QState FuelPump::Stopping(FuelPump *const me, QEvt const *const e) {

	switch (e->sig) {

	case Q_ENTRY_SIG: {
		EVENT(e);

		// ----------- Start Display --------------
		//uint32_t timeout = FuelPumpStartReq::TIMEOUT_MS;
		//FW_ASSERT(timeout > DispStartReq::TIMEOUT_MS);

		me->GetHsm().ResetOutSeq();
		Evt *evt = new DispStopReq(ILI9341, GET_HSMN(), GEN_SEQ());
		me->GetHsm().SaveOutSeq(*evt);
		Fw::Post(evt);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		me->GetHsm().Recall();
		return Q_HANDLED();
	}
	case FUEL_PUMP_STOP_REQ: {
		EVENT(e);
		me->GetHsm().Defer(e);
		return Q_HANDLED();
	}
	case DISP_STOP_CFM: {
		EVENT(e);
		ErrorEvt const &cfm = ERROR_EVT_CAST(*e);
		bool Received;

		if (me->GetHsm().HandleCfmRsp(cfm, Received)) {
			Evt *evt = new Evt(DONE, GET_HSMN());
			me->PostSync(evt);
		}

		return Q_HANDLED();
	}
	case DONE: {
		EVENT(e);
		return Q_TRAN(&FuelPump::Stopped);
	}

	}

	return Q_SUPER(&FuelPump::Root);
}

QState FuelPump::Started(FuelPump *const me, QEvt const *const e) {
	EVENT(e);
	switch (e->sig) {

	case Q_ENTRY_SIG: {
		EVENT(e);
		// ----------- MDRAW EVENT -----------
		Evt *evt = new Evt(MDRAW, GET_HSMN());
		me->PostSync(evt);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case Q_INIT_SIG: {
		return Q_TRAN(&FuelPump::Idle);
	}

	}
	return Q_SUPER(&FuelPump::Root);
}

QState FuelPump::Idle(FuelPump *const me, QEvt const *const e) {

	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		// --- Immediate Reset ----
		me->m_price = 0; // reset price
		me->m_gallons = 0; // reset gallons
		me->m_price_rate = 0; // reset price rate
		me->m_gallon_rate = 0; // reset gallon rate

		me->m_grade = 0;

		// User Notification
		me->m_toUser = DISMES_STRING[IDLE_ENTRY];

		return Q_HANDLED();
	}
	case FUEL_PUMP_PAYMENT_IND: {
		EVENT(e);
		FuelPumpPaymentInd const &req =
				static_cast<FuelPumpPaymentInd const&>(*e);

		me->m_payment = req.GetPayment(); // get user payment type
		me->m_max_amount = req.GetAmount(); // get user entered amount

		me->m_paid = true;

		// ------------- User Notification  ----------------
		me->m_toUser = DISMES_STRING[GRADE_ENTRY];

		Evt *evt = new Evt(MDRAW, GET_HSMN());
		me->PostSync(evt);

		return Q_HANDLED();

	}
	case MDRAW: {
		return Q_TRAN(&FuelPump::IdleDrawing);
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&FuelPump::Started);
}

QState FuelPump::IdleDrawing(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {

	case Q_ENTRY_SIG: {
		EVENT(e);

		me->m_exit = false;

		me->InitDraw(me);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case DISP_DRAW_END_CFM: {
		EVENT(e);

		// ------------  Stay in Current State ? --------
		if (me->m_exit) {
			if (me->m_paid) {
				me->m_paid = false;

				// ----------- MDRAW EVENT -----------
				Evt *evt = new Evt(GDRAW, GET_HSMN());
				me->PostSync(evt);

				return Q_TRAN(&FuelPump::Passive);
			}
			return Q_TRAN(&FuelPump::Idle);
		} else {

			Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
			Fw::Post(evt);

			char buf[30];

			// -------------- State Drawing -------------
			evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 120, 100, 20,
			COLOR24_BLACK);
			Fw::Post(evt);

			// -------- User Notification ----
			snprintf(buf, sizeof(buf), me->m_toUser);
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 120,
			COLOR24_RED, COLOR24_BLACK, 2);
			Fw::Post(evt);

			snprintf(buf, sizeof(buf), "87");
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 7, 265,
			COLOR24_WHITE, COLOR24_RED, 3);
			Fw::Post(evt);

			snprintf(buf, sizeof(buf), "89");
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 67, 265,
			COLOR24_WHITE, COLOR24_RED, 3);
			Fw::Post(evt);

			snprintf(buf, sizeof(buf), "91");
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 127, 265,
			COLOR24_WHITE, COLOR24_RED, 3);

			Fw::Post(evt);

			snprintf(buf, sizeof(buf), "93");
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 187, 265,
			COLOR24_WHITE, COLOR24_RED, 3);

			Fw::Post(evt);

			evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());
			Fw::Post(evt);

			me->m_exit = true;
		}

		return Q_HANDLED();
	}

	}

	return Q_SUPER(&FuelPump::Started);
}

QState FuelPump::Passive(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case Q_INIT_SIG: {
		return Q_TRAN(&FuelPump::Waiting);
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case TIME_OUT_TIMER: {
		EVENT(e);
		// Internal Event To Draw, after Transition to Idle.
		// ----------- MDRAW EVENT -----------

		Evt *evt = new Evt(MDRAW, GET_HSMN());
		me->PostSync(evt);

		return Q_TRAN(&FuelPump::Idle);
	}

	}
	return Q_SUPER(&FuelPump::Started);
}

QState FuelPump::Waiting(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {

	case Q_ENTRY_SIG: {
		EVENT(e);

		me->m_timeoutTimer.Start(ACTIVE_TIME_OUT);
		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		// ------- Stop Time ----------
		me->m_timeoutTimer.Stop();
		return Q_HANDLED();
	}
	case FUEL_PUMP_GRADE_REQ: {
		EVENT(e);
		FuelPumpGradeReq const &req = static_cast<FuelPumpGradeReq const&>(*e);

		FuelGrade *grade = me->m_currTank.GetFuelGradeG(
				(Grade) req.GetGradeType());

		if (grade && me->m_graded == false) {
			me->m_currGrade = grade;

			me->m_grade = grade->GetGrade();
			me->m_graded = true;

			me->m_price_rate = grade->GetPriceRate();

			me->m_gallon_rate = me->m_currTank.GetGallonsRate();

			Evt *evt = new FuelPumpGradeCfm(req.GetFrom(), GET_HSMN(),
					req.GetSeq(), ERROR_SUCCESS);
			Fw::Post(evt);

			// ----------- GDRAW EVENT -----------
			evt = new Evt(GDRAW, GET_HSMN());
			me->PostSync(evt);

			// ------------- User Notification  ----------------
			me->m_toUser = DISMES_STRING[FUEL_ENTRY];

			return Q_HANDLED();

		} else {
			Evt *evt = new FuelPumpGradeCfm(req.GetFrom(), GET_HSMN(),
					req.GetSeq(), ERROR_PARAM, GET_HSMN(),
					FUEL_PUMP_REASON_INVALID_GRADE);
			Fw::Post(evt);
			return Q_HANDLED();
		}

	}
	case GDRAW: {
		return Q_TRAN(&FuelPump::WaitingDrawing);
	}

	}

	return Q_SUPER(&FuelPump::Passive);
}

QState FuelPump::WaitingDrawing(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		me->WaitDraw(me);

		return Q_HANDLED();
	}
	case DISP_DRAW_END_CFM: {
		EVENT(e);

		// ------------  Where To Go --------
		if (me->m_graded) {
			me->m_graded = false;

			return Q_TRAN(&FuelPump::Pumping);
		}
		return Q_TRAN(&FuelPump::Waiting);

	}

	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}

	}
	return Q_SUPER(&FuelPump::Passive);
}

QState FuelPump::Pumping(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		// ----------- Turn On Led -----------
		Evt *evt = new GpioOutPatternReq(USER_LED, GET_HSMN(), GEN_SEQ(), 0,
				true);

		Fw::Post(evt);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);

		// ----------- Turn Off LED -----------
		Evt *evt = new GpioOutOffReq(USER_LED, GET_HSMN(), GEN_SEQ());
		Fw::Post(evt);

		return Q_HANDLED();
	}
	case Q_INIT_SIG: {
		return Q_TRAN(&FuelPump::Filling);
	}
	case PUMP_TIMER: {
		EVENT(e);

		// ----------- COMPLETE EVENT -----------
		Evt *evt = new Evt(COMPLETE, GET_HSMN());
		me->PostSync(evt);

		return Q_TRAN(&FuelPump::Admission);
	}

	}
	return Q_SUPER(&FuelPump::Passive);
}

QState FuelPump::Filling(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);
		// -- Start Timer --
		me->m_pumpTimer.Start(PUMP_TIMER);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		me->m_pumpTimer.Stop();
		return Q_HANDLED();
	}
	case FUEL_PUMP_FILL_IND: {
		EVENT(e);

		if (me->m_isbtn == false) {

			//  --- Enough Fuel In Main Tank ? ----------

			if (me->m_gallons < me->m_currGrade->GetGradeCapacity()
					&& me->m_price < me->m_max_amount) {

				me->m_gallons += me->m_gallon_rate;

				if (me->m_price < (me->m_max_amount - me->m_price_rate)) {
					//
					me->m_price += me->m_price_rate;
				} else {
					me->m_price += 0.01f;
				}

				// ----------- MDRAW EVENT -----------
				Evt *evt = new Evt(FLDRAW, GET_HSMN());
				me->PostSync(evt);

				me->m_pumpTimer.Restart(PUMP_TIMER);
				me->m_isbtn = true;

			}

			/*if (me->m_price < me->m_max_amount) {

			 me->m_gallons += me->m_currTank.GetGallonsRate();
			 me->m_price += me->m_currGrade->GetPriceRate();

			 // ----------- MDRAW EVENT -----------
			 Evt *evt = new Evt(FLDRAW, GET_HSMN());
			 me->PostSync(evt);

			 me->m_pumpTimer.Restart(PUMP_TIMER);

			 me->m_isbtn = true;

			 }*/
		}

		return Q_HANDLED();
	}
	case FLDRAW: {

		EVENT(e);
		return Q_TRAN(&FuelPump::ReDrawing);

	}

	}
	return Q_SUPER(&FuelPump::Pumping);
}

QState FuelPump::ReDrawing(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {

	case Q_ENTRY_SIG: {
		EVENT(e);

		Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
		Fw::Post(evt);
		char buf[30];

		snprintf(buf, sizeof(buf), "%.2f", me->m_price);
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 130, 10,
		COLOR24_WHITE, COLOR24_BLACK, 2);
		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "%.2f", me->m_gallons);
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 130, 50,
		COLOR24_WHITE, COLOR24_BLACK, 2);
		Fw::Post(evt);

		evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());
		Fw::Post(evt);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
	}
	case DISP_DRAW_END_CFM: {
		EVENT(e);
		// Release m_isbtn

		if (me->m_isbtn) {
			me->m_isbtn = false;
		}

		return Q_TRAN(&FuelPump::Filling);
	}

	}
	return Q_SUPER(&FuelPump::Pumping);
}

QState FuelPump::Admission(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		// ------------- Update Grade Capacity -------------
		uint16_t new_capacity = me->m_currGrade->GetGradeCapacity()
				- me->m_gallons;

		me->m_currGrade->SetGradeCapacity(new_capacity);

		// ------------- Update Main Tank -------------
		me->m_currTank.UpdateTankCapacity();

		// ------------- User Notification  ----------------
		me->m_toUser = DISMES_STRING[ADMISSION_ENTRY];

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);

		return Q_HANDLED();
	}
	case COMPLETE: {

		// ----------- GDRAW EVENT -----------
		Evt *evt = new Evt(GDRAW, GET_HSMN());
		me->PostSync(evt);

		return Q_TRAN(&FuelPump::Waiting);

	}

	}
	return Q_SUPER(&FuelPump::Passive);

}

void FuelPump::InitDraw(FuelPump *const me) {

	Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
	Fw::Post(evt);

	// -------------- Main Rectangle ----------
	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 0, 240, 320,
	COLOR24_BLACK);
	Fw::Post(evt);

	char buf[30];

	// Price

	snprintf(buf, sizeof(buf), "Price = ");

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 10,
	COLOR24_WHITE, COLOR24_BLACK, 2);
	Fw::Post(evt);

	snprintf(buf, sizeof(buf), "%.2f", me->m_price);

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 130, 10,
	COLOR24_WHITE, COLOR24_BLACK, 2);
	Fw::Post(evt);

	// -------------- Gallons -----------------

	snprintf(buf, sizeof(buf), "Gallons = ");

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 50,
	COLOR24_WHITE, COLOR24_BLACK, 2);

	Fw::Post(evt);

	snprintf(buf, sizeof(buf), "%.2f", me->m_gallons);

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 130, 50,
	COLOR24_WHITE, COLOR24_BLACK, 2);

	Fw::Post(evt);

	// Payment Type
	if (me->m_payment == 0) {

		snprintf(buf, sizeof(buf), "Payment = Cash");
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 90,
		COLOR24_WHITE, COLOR24_BLACK, 2);

		Fw::Post(evt);
	} else if (me->m_payment == 1) {
		snprintf(buf, sizeof(buf), "Payment = Credit");
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 90,
		COLOR24_WHITE, COLOR24_BLACK, 2);

		Fw::Post(evt);
	}

	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 150, 110, 65,
	COLOR24_BLACK);
	Fw::Post(evt);

	snprintf(buf, sizeof(buf), "Capacity");

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 60, 150,
	COLOR24_BLACK, COLOR24_GRAY, 2);

	Fw::Post(evt);

	snprintf(buf, sizeof(buf), "Tank:");

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 170,
	COLOR24_WHITE, COLOR24_BLACK, 2);

	Fw::Post(evt);
	snprintf(buf, sizeof(buf), "%d", me->m_currTank.GetTankCapacity());

	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 200,
	COLOR24_WHITE, COLOR24_BLACK, 2);
	Fw::Post(evt);

	//snprintf(buf, sizeof(buf), "Idle");
	//evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 130,
	//COLOR24_WHITE, COLOR24_BLACK, 2);
	//Fw::Post(evt);

	// ------------ Draw Rectangle   ---------------

	// ------------- Grade 87 ---------------------
	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 250, 50, 50,
	COLOR24_RED);
	Fw::Post(evt);

	// ------------- Grade 89 ---------------------

	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 60, 250, 50, 50,
	COLOR24_RED);
	Fw::Post(evt);

	// ------------- Grade 91 ---------------------
	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 120, 250, 50, 50,
	COLOR24_RED);
	Fw::Post(evt);

	//  ------------- Grade 93 ---------------------
	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 180, 250, 50, 50,
	COLOR24_RED);
	Fw::Post(evt);

	// ----------- End of Draw Rectangle -------------

	evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());
	Fw::Post(evt);

}

void FuelPump::WaitDraw(FuelPump *const me) {

	Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
	Fw::Post(evt);

	char buf[30];

	if (me->m_grade == 87) {
		// ------------- Grade 87 ---------------------
		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 250, 50, 50,
		COLOR24_GREEN);
		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "87");
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 7, 265,
		COLOR24_WHITE, COLOR24_GREEN, 3);
		Fw::Post(evt);

		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 120, 170, 120, 65,
		COLOR24_BLACK);
		Fw::Post(evt);

		// --------------- Grade Capacity Drawing ------------------
		snprintf(buf, sizeof(buf), "Grade:");

		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 170,
		COLOR24_WHITE, COLOR24_BLACK, 2);

		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "%d", me->m_currGrade->GetGradeCapacity());
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 200,
		COLOR24_WHITE, COLOR24_BLACK, 2);
		Fw::Post(evt);

	} else if (me->m_grade == 89) {
		// ------------- Grade 89 ---------------------

		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 60, 250, 50, 50,
		COLOR24_GREEN);
		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "89");
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 67, 265,
		COLOR24_WHITE, COLOR24_GREEN, 3);
		Fw::Post(evt);

		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 120, 170, 120, 65,
		COLOR24_BLACK);
		Fw::Post(evt);

		// --------------- Grade Capacity Drawing ------------------
		snprintf(buf, sizeof(buf), "Grade:");

		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 170,
		COLOR24_WHITE, COLOR24_BLACK, 2);

		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "%d", me->m_currGrade->GetGradeCapacity());
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 200,
		COLOR24_WHITE, COLOR24_BLACK, 2);
		Fw::Post(evt);

	} else if (me->m_grade == 91) {
		// ------------- Grade 91 ---------------------
		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 120, 250, 50, 50,
		COLOR24_GREEN);
		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "91");
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 127, 265,
		COLOR24_WHITE, COLOR24_GREEN, 3);

		Fw::Post(evt);

		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 120, 170, 120, 65,
		COLOR24_BLACK);
		Fw::Post(evt);

		// --------------- Grade Capacity Drawing ------------------
		snprintf(buf, sizeof(buf), "Grade:");

		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 170,
		COLOR24_WHITE, COLOR24_BLACK, 2);

		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "%d", me->m_currGrade->GetGradeCapacity());
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 200,
		COLOR24_WHITE, COLOR24_BLACK, 2);
		Fw::Post(evt);

	} else if (me->m_grade == 93) {
		//  ------------- Grade 93 ---------------------
		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 180, 250, 50, 50,
		COLOR24_GREEN);
		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "93");
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 187, 265,
		COLOR24_WHITE, COLOR24_GREEN, 3);

		Fw::Post(evt);

		evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 120, 170, 120, 65,
		COLOR24_BLACK);
		Fw::Post(evt);

		// --------------- Grade Capacity Drawing ------------------
		snprintf(buf, sizeof(buf), "Grade:");

		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 170,
		COLOR24_WHITE, COLOR24_BLACK, 2);

		Fw::Post(evt);

		snprintf(buf, sizeof(buf), "%d", me->m_currGrade->GetGradeCapacity());
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 120, 200,
		COLOR24_WHITE, COLOR24_BLACK, 2);
		Fw::Post(evt);

	}

	//if (me->m_graded) {

	// -------------- State Drawing -------------
	evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 120, 150, 20,
	COLOR24_BLACK);
	Fw::Post(evt);

	// -------- User Notification ----
	snprintf(buf, sizeof(buf), me->m_toUser);
	evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 120,
	COLOR24_RED, COLOR24_BLACK, 2);
	Fw::Post(evt);

	//}
	/*else {
	 // --------------- State Drawing ------------------
	 evt = new DispDrawRectReq(ILI9341, GET_HSMN(), 0, 120, 100, 20,
	 COLOR24_BLACK);
	 Fw::Post(evt);

	 snprintf(buf, sizeof(buf), "Select Grade");
	 evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 0, 120,
	 COLOR24_RED, COLOR24_BLACK, 2);
	 Fw::Post(evt);
	 }*/

	evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());

	Fw::Post(evt);

}

}
