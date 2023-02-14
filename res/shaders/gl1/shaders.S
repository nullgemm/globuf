	.global square_frag_gl1
	.global square_frag_gl1_size
	.global square_vert_gl1
	.global square_vert_gl1_size
	.section .rodata
square_frag_gl1:
	.incbin "square_frag_gl1.glsl"
1:
square_vert_gl1:
	.incbin "square_vert_gl1.glsl"
2:
square_frag_gl1_size:
	.int 1b - square_frag_gl1
square_vert_gl1_size:
	.int 2b - square_vert_gl1
