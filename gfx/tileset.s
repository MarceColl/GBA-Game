
@{{BLOCK(tileset)

@=======================================================================
@
@	tileset, 32x8@8, 
@	+ palette 256 entries, not compressed
@	+ 4 tiles not compressed
@	Total size: 512 + 256 = 768
@
@	Time-stamp: 2017-01-31, 13:04:43
@	Exported by Cearn's GBA Image Transmogrifier, v0.8.13
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global tilesetTiles		@ 256 unsigned chars
	.hidden tilesetTiles
tilesetTiles:
	.word 0x02010101,0x01010102,0x00010101,0x01010100,0x03010101,0x01010103,0x02010101,0x01010102
	.word 0x00010101,0x01010100,0x03010101,0x01010103,0x02010101,0x01010102,0x00010101,0x01010100
	.word 0x03010101,0x01010103,0x02010101,0x01010102,0x00010101,0x01010100,0x00020003,0x01010102
	.word 0x03020003,0x01010100,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101
	.word 0x00010101,0x01010103,0x02010101,0x01010102,0x00010101,0x01010103,0x02030101,0x01010302
	.word 0x02000101,0x01010002,0x00030101,0x01010300,0x01010101,0x01010101,0x01010101,0x01010101
	.word 0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101
	.word 0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101,0x01010101

	.section .rodata
	.align	2
	.global tilesetPal		@ 512 unsigned chars
	.hidden tilesetPal
tilesetPal:
	.hword 0x71AB,0x1AED,0x732B,0x7E6C,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(tileset)
