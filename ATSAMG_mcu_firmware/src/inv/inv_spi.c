/*
 * inv_spi.c
 *
 * Created: 7/25/2017 11:22:59 AM
 *  Author: TDK InvenSense
 */ 

#include <asf.h>
#include "inv_spi.h"

/* Function prototype declaration */
/**
 * \brief Initialize SPI as master.
 * PDC and Interrupts Initialized but not used
 */

bool spiPrint = false;
void spiSpy(bool On_off)
{
  spiPrint = On_off ? true : false;
}
void spi_master_init1(void)
{
	/* Enable the peripheral and set SPI mode. */
	
	flexcom_enable(BOARD_FLEXCOM_SPI);
	flexcom_set_opmode(BOARD_FLEXCOM_SPI, FLEXCOM_SPI);

	spi_disable(SPI_MASTER_BASE);
	spi_reset(SPI_MASTER_BASE);
	spi_set_lastxfer(SPI_MASTER_BASE);
	spi_set_master_mode(SPI_MASTER_BASE);
	spi_disable_mode_fault_detect(SPI_MASTER_BASE);
	spi_set_peripheral_chip_select_value(SPI_MASTER_BASE, SPI_CHIP_SEL1);
	
	spi_set_clock_polarity(SPI_MASTER_BASE, SPI_CHIP_SEL1, SPI_CLK_POLARITY);
	spi_set_clock_phase(SPI_MASTER_BASE, SPI_CHIP_SEL1, SPI_CLK_PHASE);
	spi_set_bits_per_transfer(SPI_MASTER_BASE, SPI_CHIP_SEL1, SPI_CSR_BITS_8_BIT);
	spi_set_baudrate_div(SPI_MASTER_BASE, SPI_CHIP_SEL1, (sysclk_get_peripheral_hz() / SPI_CLK_SPEED));
	spi_set_transfer_delay(SPI_MASTER_BASE, SPI_CHIP_SEL1, SPI_DLYBS, SPI_DLYBCT);

	spi_enable(SPI_MASTER_BASE);
}

void spi_master_init(void)
{
	/* Enable the peripheral and set SPI mode. */
	
	flexcom_enable(BOARD_FLEXCOM_SPI);
	flexcom_set_opmode(BOARD_FLEXCOM_SPI, FLEXCOM_SPI);

	spi_disable(SPI_MASTER_BASE);
	spi_reset(SPI_MASTER_BASE);
	spi_set_lastxfer(SPI_MASTER_BASE);
	spi_set_master_mode(SPI_MASTER_BASE);
	spi_disable_mode_fault_detect(SPI_MASTER_BASE);
	spi_set_peripheral_chip_select_value(SPI_MASTER_BASE, SPI_CHIP_SEL);
	
	spi_set_clock_polarity(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_CLK_POLARITY);
	spi_set_clock_phase(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_CLK_PHASE);
	spi_set_bits_per_transfer(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_CSR_BITS_8_BIT);
	spi_set_baudrate_div(SPI_MASTER_BASE, SPI_CHIP_SEL, (sysclk_get_peripheral_hz() / SPI_CLK_SPEED));
	spi_set_transfer_delay(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_DLYBS, SPI_DLYBCT);

	spi_enable(SPI_MASTER_BASE);
}

void spi_master_write_register(uint8_t register_addr, uint32_t len, uint8_t * value)
{
	uint8_t reg	= register_addr; 
	const uint8_t *p_rbuf = value; 
	uint32_t rsize = len;
	uint32_t i;
	uint8_t uc_pcs = 0;
	uint16_t data = 0;
//	uint16_t data1 = 0;
//	uint8_t p_wbuf[200];

	reg &= WRITE_BIT_MASK;
if (spiPrint) {		
    int j;
	printf("write: 0x%02x ", reg);
	for (j = 0; j < len; j++) printf("%02x ", value[j]);
	printf("\r\n");
}
	spi_write(SPI_MASTER_BASE, reg, uc_pcs, 0); /* write cmd/reg-addr */
	while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_TDRE) == 0); /* Wait transfer data reg done */
    spi_read(SPI_MASTER_BASE, &data, &uc_pcs); /* dummy read */

	for (i = 0; i < rsize; i++) {

		spi_write(SPI_MASTER_BASE, p_rbuf[i], 0, 0); /* dummy write to generate clock */
		while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_TDRE) == 0); /* Wait transfer data reg done. */
		spi_read(SPI_MASTER_BASE, &data, &uc_pcs); /* read actual register data */
				//printf("SPI Write Iter: %d, value: %04X, data: %04X, pcs %04X\n\r", i, p_rbuf[i], data, uc_pcs);
				

	}
/*	printf("   return value --> 0x%02x", i, data1);
	for (int j = 0; j < 3; j++) printf("%02x", p_wbuf[j]);
	printf("\r\n");*/
	
	return;
	getchar();
}

void spi_master_set_read_register_cmd(uint8_t register_addr)
{
		uint8_t reg	= register_addr;
		uint8_t uc_pcs = 0;//SPI_CHIP_PCS;
		uint16_t data = 0;

		reg |= READ_BIT_MASK;
if (spiPrint) {printf("Set Rd: Addr: %02X \r\n", reg)	;}

		spi_write(SPI_MASTER_BASE, reg, uc_pcs, 0); /* write cmd/reg-addr */
		while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_TDRE) == 0); /* Wait transfer data reg done */
		spi_read(SPI_MASTER_BASE, &data, &uc_pcs); /* dummy read */
}

void spi_master_read_register(uint8_t register_addr, uint32_t len, uint8_t * value)
{
	uint8_t reg	= register_addr; 
	uint8_t *p_rbuf	= value; 
	uint32_t rsize	= len;
	uint32_t i;
	uint8_t uc_pcs = 0;//SPI_CHIP_PCS;
	uint16_t data = 0;
    uint16_t val[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // for future expansion
    val[2] = value[0];  // crc passed in

	reg |= READ_BIT_MASK;

if (spiPrint) {
	printf("Rd: Addr: %02X, length: %d ", reg, len);
	for (i=0; i<len; i++)
	printf(" Val:  %02X ", (uint) (value[i]));
	printf("\r\n");
}	

	spi_write(SPI_MASTER_BASE, reg, uc_pcs, 0); /* write cmd/reg-addr */
	while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_TDRE) == 0); /* Wait transfer data reg done */
	spi_read(SPI_MASTER_BASE, &data, &uc_pcs); /* dummy read */

	for (i = 0; i < rsize; i++) {
		spi_write(SPI_MASTER_BASE, val[i], uc_pcs, 0); /* dummy write to generate clock */
		while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_TDRE) == 0); /* Wait transfer data reg done. */
		spi_read(SPI_MASTER_BASE, &data, &uc_pcs); /* read actual register data */
		p_rbuf[i] = (uint8_t)(data & 0xFF);
	}
	return;
}

void spi_master_deinit(void)
{
	flexcom_disable(BOARD_FLEXCOM_SPI);
	
	spi_disable(SPI_MASTER_BASE);
	
	return;
}
