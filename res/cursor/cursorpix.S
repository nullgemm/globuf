	.global cursorpix
	.global cursorpix_size
	.section .rodata
cursorpix:
	.incbin "cursorpix.bin"
1:
cursorpix_size:
	.int 1b - cursorpix
