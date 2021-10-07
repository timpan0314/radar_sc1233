/*
 * Copyright (c) 2015-2018 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_term.h
 * @brief Sensor driver implemantation for GPIO on MBED\@HRM1017
 * @version (PRELIMINARY)
 * @warning This is just a sample source code.
 */

#ifndef __RS_TERM_H__
#define __RS_TERM_H__

#include <stdint.h>
//#include "mbed.h"  Tim

#include "rs24g_types.h"
#include "defs/rs_dev_term.h"

template<typename T>
class rs_gpio
{
private:
	T   gpio;
public:
	rs_gpio(PinName pin) : gpio(pin) {};
	int get(void) {
		return gpio;
	};
	rs_ret_t get(rs_bool_t *val) {
		if (get() == 1) *val = RS_TRUE;
		else *val = RS_FALSE;
		return RS_OK;
	};
	rs_ret_t set(rs_bool_t val) {
		if (val == RS_TRUE) gpio = 1;
		else gpio = 0;
		return RS_OK;
	};

	T* ptr() {return &gpio;};
};

class rs_trigger : public rs_gpio<InterruptIn>
{
private:
	bool rise = false;
	bool fall = false;
public:
	rs_trigger(PinName pin) : rs_gpio<InterruptIn>(pin) {};

	void enable_int() {
		ptr()->enable_irq();
	};

	void disable_int() {
		ptr()->disable_irq();
	};

	void setcb_rise(void (*func)(rs_trigger *), rs_trigger *arg1) {
		ptr()->rise(callback(func, arg1));
	};

	void setcb_fall(void (*func)(rs_trigger *), rs_trigger *arg1) {
		ptr()->fall(callback(func, arg1));
	};

	static void trigger_rise_cb(rs_trigger *trig) {
		trig->rise = true;
	}

	static void trigger_fall_cb(rs_trigger *trig) {
		trig->fall = true;
	}

	rs_ret_t set_trigger(uint32_t trigger) {
		if ((trigger & RS_TERM_TRIGGER_RISING)) {
			setcb_rise(trigger_rise_cb, this);
		}
		if (trigger & RS_TERM_TRIGGER_FALLING) {
			setcb_fall(trigger_fall_cb, this);
		}
		return RS_OK;
	}

	rs_ret_t get_trigger(uint32_t trigger, rs_bool_t *val) {
		return RS_ENOSUPPORT;
	}

	rs_ret_t clear_trigger(void) {
		rise = false;
		fall = false;
		return RS_OK;
	}

	rs_ret_t wait_trigger(uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val) {
		int remain = timeout;
		bool ret = false;
		do {
			if ((trigger & RS_TERM_TRIGGER_RISING) && rise) {
				ret = true;
				rise = false;
				*val = true;
			}
			if ((trigger & RS_TERM_TRIGGER_FALLING) && fall) {
				ret = true;
				fall = false;
				*val = false;
			}
			if (ret == true) break;
			wait_us(1000);
			remain -= 1;
		} while (remain > 0);
		if (!ret) return RS_ETOUT;
		return RS_OK;
	}
};

/**
 * Sensor driver for GPIO on MBED\@HRM1017
 *
 * @version (PRLIMINARY)
 */
class rs_term
{
private:
	rs_gpio<DigitalOut> Nrst;
	rs_gpio<DigitalOut> Ce;
	rs_trigger Or;
	rs_trigger Detout;

public:
	rs_term(PinName _nrst, PinName _ce, PinName _or, PinName _detout) : Nrst(_nrst), Ce(_ce), Or(_or), Detout(_detout) {};
	virtual ~rs_term(void) {};

	/**
	 * read GPIO
	 *
	 * @param[in] term terminal ID for Sensor
	 * @param[out] val pointer of read value
	 */
	rs_ret_t get(uint32_t term, rs_bool_t *val) {
		switch (term) {
		case RS_TERM_NRST:
			return Nrst.get(val);
		case RS_TERM_CE:
			return Ce.get(val);
		case RS_TERM_OR:
			return Or.get(val);
		case RS_TERM_DETOUT:
			return Detout.get(val);
		default:
			return RS_EINV;
		}
	};

	/**
	 * write(set) GPIO
	 *
	 * @param[in] term ternial ID for Sensor
	 * @param[out] val value to write(set)
	 */
	rs_ret_t set(uint32_t term, rs_bool_t val) {
		switch (term) {
		case RS_TERM_NRST:
			return Nrst.set(val);
		case RS_TERM_CE:
			return Ce.set(val);
		default:
			return RS_EINV;
		}
	};

	/**
	 * Setup GPIO as trigger
	 *
	 * @param[in] term terminal ID for Sensor
	 * @param[in] trigger type of trigger
	 */
	rs_ret_t set_trigger(uint32_t term, uint32_t trigger) {
		switch (term) {
		case RS_TERM_OR:
			return Or.set_trigger(trigger);
		case RS_TERM_DETOUT:
			return Detout.set_trigger(trigger);
		default:
			return RS_EINV;
		}
	};

	/**
	 * Clear triggered flag
	 *
	 * @param[in] term terminal ID for Sensor
	 */
	rs_ret_t clear_trigger(uint32_t term) {
		switch (term) {
		case RS_TERM_OR:
			return Or.clear_trigger();
		case RS_TERM_DETOUT:
			return Detout.clear_trigger();
		default:
			return RS_EINV;
		}
	};

	/**
	 * Wait trigger event
	 *
	 * @param[in] term terminal ID for Sensor
	 * @param[in] timeout timeout in ms.
	 * @param[in] trigger type of trigger to wait
	 * @param[out] val pointer of value at triggerd
	 */
	rs_ret_t wait_trigger(uint32_t term, uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val) {
		switch (term) {
		case RS_TERM_OR:
			return Or.wait_trigger(timeout, trigger, val);
		case RS_TERM_DETOUT:
			return Detout.wait_trigger(timeout, trigger, val);
		default:
			return RS_EINV;
		}
	};
};
#endif /* __RS_TERM_H__ */
