#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

/*************************************************************************
 ������� ��������, ��� ��������� �������� ������.
*************************************************************************/
void __attribute__ ((noinline)) i2c_delay(void)
{
	_delay_us(3);
}

/*************************************************************************
 ������� ��������, ��� �������� ���������� ������.
*************************************************************************/
void __attribute__ ((noinline)) delay_50(void)
{
	_delay_ms(50);
}


/*************************************************************************
 ������������� I2C ����������. ������ ���� ��������� �������.
*************************************************************************/
void i2c_init(void)
{
	I2C_DDR &= ~((1<<I2C_SCL)|(1<<I2C_SDA));
	I2C_DDR |= (1<<I2C_VCC);
	I2C_PORT &= ~((1<<I2C_SCL)|(1<<I2C_SDA)|(1<<I2C_VCC));
}/* i2c_init */

/*************************************************************************	
  �������� ��������� ������� � �������� ����.
*************************************************************************/
uint8_t i2c_start(void)
{
	SDA_UP();
	i2c_delay();
	SCL_UP();
	i2c_delay();	
	SDA_DOWN();
	i2c_delay();
	SCL_DOWN();
	i2c_delay();
	return 0;
}/* i2c_start */

/*************************************************************************
  ��������� ������ ������� � ����������� ����.
*************************************************************************/
void i2c_stop(void)
{
	SDA_DOWN();
	i2c_delay();
	SCL_UP();
	i2c_delay();
	SDA_UP();
	i2c_delay();
}/* i2c_stop */

/*************************************************************************
 �������� ���� ���� � ���������� ACK.
*************************************************************************/
uint8_t i2c_transmit( uint8_t byte )
{
	for(char i=0;i<8;i++){
		if (byte & (1<<7)) SDA_UP(); else SDA_DOWN();
		byte = byte<<1;
		SCL_UP();
		i2c_delay();
		SCL_DOWN();
		i2c_delay();
	}
	char ack;
	SCL_UP();
	i2c_delay();
	if(IS_ACK())ack = 0; else ack = 1;
	SCL_DOWN();
	i2c_delay();
	return ack;
}/* i2c_transmit */

/*************************************************************************
 ������ ���� ���� � ���������� ���������� �� ������ � �������� ACK.
*************************************************************************/
void i2c_receive_ack( uint8_t* byte)
{
	for(char i=0;i<8;i++){
		SCL_UP();
		i2c_delay();
		*byte = *byte<<1;
		if(IS_ACK()) *byte &= ~1; else *byte |= 1;
		SCL_DOWN();
		i2c_delay();
	}
	SDA_DOWN();
	SCL_UP();
	i2c_delay();
	SCL_DOWN();
	SDA_UP();
	i2c_delay();
}/* i2c_receive_ack */


/*************************************************************************
 ������ ���� ���� � ���������� ���������� �� ������ � ���������� ACK.
*************************************************************************/
uint8_t i2c_receive( uint8_t* byte)
{
	for(char i=0;i<8;i++){
		SCL_UP();
		i2c_delay();
		*byte = *byte<<1;
		if(IS_ACK()) *byte &= ~1; else *byte |= 1;
		SCL_DOWN();
		i2c_delay();
	}
	char ack;
	SCL_UP();
	i2c_delay();
	if(IS_ACK())ack = 0; else ack = 1;
	SCL_DOWN();
	i2c_delay();
	return ack;
}/* i2c_receive */

/*************************************************************************
  ��������� ������� ������ ��� �� �������� ����� 24c16
*************************************************************************/
uint8_t i2c_set_address_c16(uint8_t address, uint8_t for_read)
{
	i2c_start();
	if(i2c_transmit(CONTROL & (~1))) {i2c_stop(); return 1;}
	i2c_transmit(address);
	if(for_read){
		i2c_start();
		i2c_transmit(CONTROL | 1);
	}
	return 0;
}/*i2c_set_address_c16*/

/*************************************************************************
  ��������� ������� ������ ��� �� �������� ����� 24c64
*************************************************************************/
uint8_t i2c_set_address_c64(uint16_t address, uint8_t for_read)
{
	i2c_start();
	if(i2c_transmit(CONTROL & (~1))) {i2c_stop(); return 1;}
	i2c_transmit(address>>8);
	i2c_transmit(address);
	if(for_read){
		i2c_start();
		i2c_transmit(CONTROL | 1);
	}
	return 0;
}/*i2c_set_address_c64*/

/*************************************************************************
  ���������� ���� ���� �� ��������� ������ 24c64
*************************************************************************/
uint8_t i2c_write_c64( uint16_t address, uint8_t data )
{
	//WP_OFF();
	i2c_set_address_c64(address,0);
	i2c_transmit(data);
	i2c_stop();
	delay_50();
	//WP_ON();
	return 0;
}/* i2c_write_c64 */

/*************************************************************************
  ������ ���� ���� �� ��������� ������ 24c64
*************************************************************************/
uint8_t i2c_read_c64( uint16_t address, uint8_t* data )
{
	i2c_set_address_c64(address,1);
	i2c_receive(data);
	i2c_stop();
	return 0;
}/* i2c_read_c64 */