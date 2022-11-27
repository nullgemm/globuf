	.global square_frag
	.global square_frag_size
	.global square_vert
	.global square_vert_size
	.section .rodata
square_frag:
	.incbin "square_frag_gl1.glsl"
1:
square_vert:
	.incbin "square_vert_gl1.glsl"
2:
square_frag_size:
	.int 1b - square_frag
square_vert_size:
	.int 2b - square_vert
