	.global iconpix
	.global iconpix_size
	.section .rodata
iconpix:
	.incbin "iconpix.bin"
1:
iconpix_size:
	.int 1b - iconpix
