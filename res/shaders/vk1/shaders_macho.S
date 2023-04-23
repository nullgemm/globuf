	.global _square_frag_vk1
	.global _square_frag_vk1_size
	.global _square_vert_vk1
	.global _square_vert_vk1_size
	.section __TEXT,__const
_square_frag_vk1:
	.incbin "square_frag_vk1.spv"
1:
_square_vert_vk1:
	.incbin "square_vert_vk1.spv"
2:
_square_frag_vk1_size:
	.int 1b - _square_frag_vk1
_square_vert_vk1_size:
	.int 2b - _square_vert_vk1
