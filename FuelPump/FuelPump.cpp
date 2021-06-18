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

FuelPump::FuelPump() :
		Active((QStateHandler) &FuelPump::InitialPseudoState, FUEL_PUMP,
				"FUEL_PUMP"), m_price(0.00f), m_gallons(0.00f), m_max_amount(0), m_payment(
				0), m_paid(false), m_graded(false), m_isbtn(false), m_grade(0),
				m_currTank(MAIN_TANK),m_currGrade(NULL),
				m_timeoutTimer(GetHsm().GetHsmn(), TIME_OUT_TIMER),
				m_pumpTimer(GetHsm().GetHsmn(), PUMP_TIMER) {
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
		return Q_HANDLED();
	}
	case FUEL_PUMP_PAYMENT_IND: {
		EVENT(e);
		FuelPumpPaymentInd const &req =
				static_cast<FuelPumpPaymentInd const&>(*e);

		me->m_price = 0; // reset price
		me->m_gallons = 0; // reset gallons

		me->m_payment = req.GetPayment(); // get user payment type
		me->m_max_amount = req.GetAmount(); // get user entered amount

		me->m_paid = true;

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
		Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
		Fw::Post(evt);
		char buf[30];
		snprintf(buf, sizeof(buf), "Price = %.2f", me->m_price);
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 30,
		COLOR24_BLUE, COLOR24_GREEN, 2);
		Fw::Post(evt);
		snprintf(buf, sizeof(buf), "Gallons = %.2f", me->m_gallons);
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 90,
		COLOR24_BLUE, COLOR24_GREEN, 2);
		Fw::Post(evt);

		if (me->m_payment == 0) {
			snprintf(buf, sizeof(buf), "Cash");
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 150,
			COLOR24_BLACK, COLOR24_WHITE, 2);

			Fw::Post(evt);
		} else {
			snprintf(buf, sizeof(buf), "Credit");
			evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 150,
			COLOR24_BLACK, COLOR24_WHITE, 2);

			Fw::Post(evt);
		}

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

		// ------------  Where To Go --------
		if (me->m_paid) {
			me->m_paid = false;
			return Q_TRAN(&FuelPump::Passive);
		}
		return Q_TRAN(&FuelPump::Idle);
	}
		/* case MDRAW: {
		 EVENT(e);

		 Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
		 Fw::Post(evt);
		 char buf[30];



		 evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());
		 Fw::Post(evt);

		 return Q_HANDLED();
		 }*/

	}

	return Q_SUPER(&FuelPump::Started);
}

QState FuelPump::Passive(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);

		// ----------- MDRAW EVENT -----------
		//Evt *evt = new Evt(GDRAW, GET_HSMN());
		//me->PostSync(evt);

		return Q_HANDLED();
	}
	case Q_INIT_SIG: {
		return Q_TRAN(&FuelPump::Waiting);
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		return Q_HANDLED();
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
	case TIME_OUT_TIMER: {
		EVENT(e);
		return Q_TRAN(&FuelPump::Idle);
	}
	case FUEL_PUMP_GRADE_REQ: {
		EVENT(e);
		FuelPumpGradeReq const &req = static_cast<FuelPumpGradeReq const&>(*e);


		FuelGrade *grade = me->m_currTank.GetFuelGradeG((Grade)req.GetGradeType());

		if(grade)
		{
			me->m_currGrade = grade;
			me->m_grade = req.GetGradeType();
			me->m_graded = true;

			Evt *evt = new FuelPumpGradeCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(),
					ERROR_SUCCESS);
			Fw::Post(evt);

			// ----------- MDRAW EVENT -----------
			evt = new Evt(GDRAW, GET_HSMN());
			me->PostSync(evt);

			return Q_HANDLED();

		}
		else
		{
			 Evt *evt = new FuelPumpGradeCfm(req.GetFrom(), GET_HSMN(),
					 req.GetSeq(), ERROR_PARAM, GET_HSMN(), FUEL_PUMP_REASON_INVALID_GRADE);
			 Fw::Post(evt);
			 return Q_HANDLED();
		}

	}
	case GDRAW: {
		return Q_TRAN(&FuelPump::Drawing);
	}

	}

	return Q_SUPER(&FuelPump::Passive);
}

QState FuelPump::Drawing(FuelPump *const me, QEvt const *const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		EVENT(e);
		Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
		Fw::Post(evt);

		char buf[30];

		snprintf(buf, sizeof(buf), "Grade = %lu", me->m_grade);

		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 210,
		COLOR24_BLUE, COLOR24_GREEN, 2);

		Fw::Post(evt);

		evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());

		Fw::Post(evt);

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

		// ------- Turn On Led --------
		Evt *evt = new GpioOutPatternReq(USER_LED, GET_HSMN(), GEN_SEQ(), 0,
				true);

		Fw::Post(evt);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);

		// -- Turn Off LED --
		Evt *evt = new GpioOutOffReq(USER_LED, GET_HSMN(), GEN_SEQ());
		Fw::Post(evt);

		return Q_HANDLED();
	}
	case Q_INIT_SIG: {
		return Q_TRAN(&FuelPump::Filling);
	}
	case PUMP_TIMER: {
		EVENT(e);
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

		if (me->m_isbtn == false ) {
			if (me->m_price <= me->m_max_amount) {

				me->m_gallons += me->m_currTank.GetGallonsRate();
				me->m_price += me->m_currGrade->GetPriceRate();

				// ----------- MDRAW EVENT -----------
				Evt *evt = new Evt(FUDRAW, GET_HSMN());
				me->PostSync(evt);

				me->m_pumpTimer.Restart(PUMP_TIMER);

				me->m_isbtn = true;

			}
		}

		return Q_HANDLED();
	}
	case FUDRAW: {

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
		snprintf(buf, sizeof(buf), "Price = %.2f", me->m_price);
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 30,
		COLOR24_BLUE, COLOR24_GREEN, 2);
		Fw::Post(evt);
		snprintf(buf, sizeof(buf), "Gallons = %.2f", me->m_gallons);
		evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 90,
		COLOR24_BLUE, COLOR24_GREEN, 2);
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

		// --------- Start Timer ----------
		// Update Node

		me->m_timeoutTimer.Start(NODEUP_TIME_OUT);

		return Q_HANDLED();
	}
	case Q_EXIT_SIG: {
		EVENT(e);
		me->m_timeoutTimer.Stop();
		return Q_HANDLED();
	}
	case TIME_OUT_TIMER: {

		EVENT(e);
		return Q_TRAN(&FuelPump::Idle);

	}

	}
	return Q_SUPER(&FuelPump::Passive);

}

}
