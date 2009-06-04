/*
 * The ar9170 firwmare gets stuffed into the
 * device's Program RAM, which has a size of 32K.
 * We use our own linker script to customize
 * the location of expected code but also to
 * avoid running out of the allowed space.
 */
MEMORY
{
    pram : ORIGIN = 0x200000, LENGTH = 16k
}

SECTIONS
{
	/*
	 * The ar9170 boot code will execute the code
	 * at address 0x04 from the loaded firmware as
	 * such we must ensure our starting routine
	 * is kept at that address.
	 */
	.padding : {
		/* NOP NOP just in case */
		LONG(0x00090009)
	} > pram

	.boot : { *(.boot) } > pram

	/* anything else can be anywhere */

	.text : { *(.text*) } > pram 
	.rodata : { *(.rodata*) } > pram 
	.data	: { *(.data*) } > pram
	.bss	: { *(.bss) } > pram
}
