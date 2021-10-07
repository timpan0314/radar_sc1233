/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_dev_poc.cpp
 */

#include <stdint.h>
#include <stdlib.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_term.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_opcode.h"
#include "main.h"

//#include "stm32f0xx.h"  //Tim

// Tim will implement later!!!
//#if defined(RS_CPUIF_USE_I2C)
//#include "rs_i2c.h"
//static rs_i2c _spi(0x50, I2C_SDA0, I2C_SCL0, 400000);
//static DigitalOut sclk(SPI_PSELSCK1, 0); /* Ground if I2C_SEL=1 */
//static DigitalIn sdo(SPI_PSELMISO1); /* Open if I2C_SEL=1 */
//#else
//#include "rs_spi.h"
//#  if defined(RS_MBED_USE_STANDARD_NAMES)
//static rs_spi _spi(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS, 10000000);
//#  else
//static rs_spi _spi(P0_12, P0_13, P0_15, P0_14, 10000000);
//#  endif
//#endif
//#include "rs_term.h"
//#  if defined(RS_MBED_USE_STANDARD_NAMES)
//static rs_term _term(D0, D1, D2, D3);
//#  else
//static rs_term _term(P0_30, P0_0, P0_28, P0_29);
//#  endif


//#include "rs_ctl_dev_poc.h"
#include "rs_ctl_dev.h"
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;


int is_or_triggered = 0;
int is_detect_triggered = 0;
/**
 * open device file
 */
RS_IF rs_ret_t rs_dev_open(rs_ctl_dev_t *dev) {
	
	//*dev = &hi2c1;
        HAL_GPIO_WritePin(RADAR_I2CSEL_GPIO_Port, RADAR_I2CSEL_Pin, GPIO_PIN_RESET);
        *dev = &hspi1;
	return RS_OK;
}

/**
 * close device file
 */
RS_IF rs_ret_t rs_dev_close(rs_ctl_dev_t dev) {
	//(void)dev;
	return RS_OK;
}

/**
 * write data to device
 * @sa rs_spi::write(), rs_i2c::write()
 */
RS_IF rs_ret_t rs_dev_write(rs_ctl_dev_t dev, const uint8_t *wdata, size_t size) {
    
    HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_RESET);
    //int ret = HAL_SPI_Transmit(reinterpret_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(wdata),size,500);
  int ret = HAL_SPI_Transmit(&hspi1,const_cast<uint8_t *>(wdata),size,1000);
    HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
    // int ret = HAL_SPI_Transmit(static_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(wdata),size,500);
 //HAL_I2C_Master_Transmit(static_cast<I2C_HandleTypeDef *>(dev),0x50,const_cast<uint8_t *>(wdata),size,500);
	return ret;
}

/**
 * write data to device for low memory usage
 * @sa rs_spi::write_low_memory(), rs_i2c::write_low_memory()
 */
RS_IF rs_ret_t rs_dev_write_low_memory(rs_ctl_dev_t dev, const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize) {
      int ret = 0;
      HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_RESET);
      ret = HAL_SPI_Transmit(reinterpret_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(hdata),hsize,500);
      if(ret !=0){
           HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
           return ret;
      }
      ret = HAL_SPI_Transmit(reinterpret_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(wdata),wsize,500);
      HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
      return ret;
}

/**
 * read data from device
 * @sa rs_spi::read(), rs_i2c::read()
 */
RS_IF rs_ret_t rs_dev_read(rs_ctl_dev_t dev, const uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize) {
        int ret = 0;
        HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_RESET);
        //ret = HAL_SPI_Transmit(reinterpret_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(wdata),wsize,500);
        ret = HAL_SPI_Transmit(&hspi1,const_cast<uint8_t *>(wdata),wsize,1000);
         if(ret !=0){
         HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
           return ret;
        }
        ret = HAL_SPI_Transmit(&hspi1,const_cast<uint8_t *>(wdata),1,1000);
        
        if(ret !=0){
          HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
           return ret;
        }
        
        ret = HAL_SPI_Receive(&hspi1,const_cast<uint8_t *>(rdata),rsize,1000);
        //ret = HAL_SPI_Receive(reinterpret_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(rdata),rsize,500);
        if(ret !=0){
           HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
           return ret;
        }
        HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
        //	HAL_I2C_Mem_Read (static_cast<I2C_HandleTypeDef *>(dev), 0x50, const_cast<uint8_t *>(wdata), wsize, rdata, rsize, 500);
        return ret;
}

RS_IF rs_ret_t rs_dev_spi_read(rs_ctl_dev_t dev, const uint8_t *wdata, uint8_t *rdata, size_t size) {
        HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_RESET);
        int ret = HAL_SPI_TransmitReceive(reinterpret_cast<SPI_HandleTypeDef *>(dev),const_cast<uint8_t *>(wdata),rdata,size,500);
        HAL_GPIO_WritePin(RADAR_SENB_GPIO_Port, RADAR_SENB_Pin, GPIO_PIN_SET);
	return ret;
}

/**
 * read device terminal (GPIO) value
 * @sa rs_term::get()
 */
RS_IF rs_ret_t rs_dev_term_get(rs_ctl_dev_t dev, uint32_t term, rs_bool_t *val) {
  switch(term){
  case RS_TERM_NRST:
    *val = HAL_GPIO_ReadPin(RADAR_NRST_GPIO_Port, RADAR_NRST_Pin);
    break;
  case RS_TERM_CE:
    *val = HAL_GPIO_ReadPin(RADAR_CE_GPIO_Port, RADAR_CE_Pin);
    break;
  case RS_TERM_OR:
    *val = HAL_GPIO_ReadPin(RADAR_OR_GPIO_Port, RADAR_OR_Pin);
    break;
  case RS_TERM_DETOUT:
    *val = HAL_GPIO_ReadPin(RADAR_DETOUT_GPIO_Port, RADAR_DETOUT_Pin);
    break;
  default:
    return RS_EINV;
  }
	return 0;
}

/**
 * write device terminal (GPIO) value
 * @sa rs_term::set()
 */
RS_IF rs_ret_t rs_dev_term_set(rs_ctl_dev_t dev, uint32_t term, rs_bool_t val) {
  switch(term){
  case RS_TERM_NRST:
    if(val==0)
    HAL_GPIO_WritePin(RADAR_NRST_GPIO_Port,RADAR_NRST_Pin,GPIO_PIN_RESET);
    else
    HAL_GPIO_WritePin(RADAR_NRST_GPIO_Port,RADAR_NRST_Pin,GPIO_PIN_SET);
    break;
  case RS_TERM_CE:
    if(val==0)
    HAL_GPIO_WritePin(RADAR_CE_GPIO_Port,RADAR_CE_Pin,GPIO_PIN_RESET);
    else
     HAL_GPIO_WritePin(RADAR_CE_GPIO_Port,RADAR_CE_Pin,GPIO_PIN_SET); 
    break;
  default:
    return RS_EINV;
  }
  
  return 0;
}

/**
 * setup terminal (GPIO) as trigger
 * @sa rs_term::set_trigger()
 */
RS_IF rs_ret_t rs_dev_term_set_trigger(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger) {
  switch(term){
  case RS_TERM_OR:
    is_or_triggered = 0;
    //HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    break;
  case RS_TERM_DETOUT:
    is_detect_triggered = 0;
    //HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    break;    
  }
	return 0;
}

/**
 * clear triggered flag
 * @sa rs_term::clear_trigger()
 */
RS_IF rs_ret_t rs_dev_term_clear_trigger(rs_ctl_dev_t dev, uint32_t term) {
	switch (term){
	case RS_TERM_OR:
		is_or_triggered = 0;
		//HAL_NVIC_DisableIRQ(EXTI1_IRQn);
		return 0;
	case RS_TERM_DETOUT:
		is_detect_triggered = 0;
		//HAL_NVIC_DisableIRQ(EXTI0_IRQn);
		return 0;
	}
	return 0;
}

/**
 * wait trigger event
 * @sa rs_term::wait_trigger()
 */
RS_IF rs_ret_t rs_dev_term_wait_trigger(rs_ctl_dev_t dev, uint32_t term, uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val) {
	switch(term){
	case RS_TERM_OR:
		while(timeout--){
			if(is_or_triggered == 1){
				*val = HAL_GPIO_ReadPin(RADAR_OR_GPIO_Port, RADAR_OR_Pin);
				return RS_OK;
			}
			HAL_Delay(1);
		}
		*val = HAL_GPIO_ReadPin(RADAR_OR_GPIO_Port, RADAR_OR_Pin);
		return RS_ETOUT;
	case RS_TERM_DETOUT:
		while(timeout--){
			if(is_detect_triggered == 1){
				*val = HAL_GPIO_ReadPin(RADAR_DETOUT_GPIO_Port, RADAR_DETOUT_Pin);
				return RS_OK;
			}
			HAL_Delay(1);
		}
		*val = HAL_GPIO_ReadPin(RADAR_DETOUT_GPIO_Port, RADAR_DETOUT_Pin);
		return RS_ETOUT;
	default:
		return -1;
	}
}
