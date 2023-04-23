	.global _iconpix
	.global _iconpix_size
	.section __TEXT,__const
_iconpix:
	.incbin "iconpix.bin"
1:
_iconpix_size:
	.int 1b - _iconpix
