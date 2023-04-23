	.global square_frag_vk1
	.global square_frag_vk1_size
	.global square_vert_vk1
	.global square_vert_vk1_size
	.section .rodata
square_frag_vk1:
	.incbin "square_frag_vk1.spv"
1:
square_vert_vk1:
	.incbin "square_vert_vk1.spv"
2:
square_frag_vk1_size:
	.int 1b - square_frag_vk1
square_vert_vk1_size:
	.int 2b - square_vert_vk1
