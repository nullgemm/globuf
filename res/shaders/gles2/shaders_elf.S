	.global square_frag_gles2
	.global square_frag_gles2_size
	.global square_vert_gles2
	.global square_vert_gles2_size
	.section .rodata
square_frag_gles2:
	.incbin "square_frag_gles2.glsl"
1:
square_vert_gles2:
	.incbin "square_vert_gles2.glsl"
2:
square_frag_gles2_size:
	.int 1b - square_frag_gles2
square_vert_gles2_size:
	.int 2b - square_vert_gles2
