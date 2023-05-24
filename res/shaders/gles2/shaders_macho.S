	.global _square_frag_gles2
	.global _square_frag_gles2_size
	.global _square_vert_gles2
	.global _square_vert_gles2_size
	.section __TEXT,__const
_square_frag_gles2:
	.incbin "square_frag_gles2.glsl"
1:
_square_vert_gles2:
	.incbin "square_vert_gles2.glsl"
2:
_square_frag_gles2_size:
	.int 1b - _square_frag_gles2
_square_vert_gles2_size:
	.int 2b - _square_vert_gles2
