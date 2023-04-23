	.global _cursorpix
	.global _cursorpix_size
	.section __TEXT,__const
_cursorpix:
	.incbin "cursorpix.bin"
1:
_cursorpix_size:
	.int 1b - _cursorpix
