/*
 * inv_spi.h
 *
 * Created: 7/25/2017 11:23:14 AM
 *  Author: TDK InvenSense
 */ 


#ifndef INV_SPI_H_
#define INV_SPI_H_

#define SPI_Handler				FLEXCOM5_Handler
#define SPI_IRQn				FLEXCOM5_IRQn
#define SPI_CHIP_SEL1			0 /* NPCS0 */
#define SPI_CHIP_PCS1			spi_get_pcs(SPI_CHIP_SEL1)
#define SPI_CHIP_SEL			1 /* PA05_SPI5_NPCS1 */
#define SPI_CHIP_PCS			spi_get_pcs(SPI_CHIP_SEL)
#define SPI_CLK_POLARITY		0  /* Clock polarity.was 0 */
#define SPI_CLK_PHASE			1  /* Clock phase. was 1*/
#define SPI_DLYBS				0x40 /* Delay before SPCK. was 40*/
#define SPI_DLYBCT				0x10 /* Delay between consecutive transfers. was 10, even with 0x0C there is err*/
#define SPI_CLK_SPEED			500000 /* SPI clock setting (Hz). sheena */

#define READ_BIT_MASK			0x80
#define WRITE_BIT_MASK			0x7F

void spi_master_init1(void);
void spi_master_init(void);
void spi_master_deinit(void);
void spi_master_write_register(uint8_t register_addr, uint32_t len, uint8_t * value);
void spi_master_read_register(uint8_t register_addr, uint32_t len, uint8_t * value);
void spi_master_set_read_register_cmd(uint8_t register_addr);
void spiSpy(bool On_off);

#endif /* INV_SPI_H_ */