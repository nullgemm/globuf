	.global _square_frag_gl1
	.global _square_frag_gl1_size
	.global _square_vert_gl1
	.global _square_vert_gl1_size
	.section __TEXT,__const
_square_frag_gl1:
	.incbin "square_frag_gl1.glsl"
1:
_square_vert_gl1:
	.incbin "square_vert_gl1.glsl"
2:
_square_frag_gl1_size:
	.int 1b - _square_frag_gl1
_square_vert_gl1_size:
	.int 2b - _square_vert_gl1
