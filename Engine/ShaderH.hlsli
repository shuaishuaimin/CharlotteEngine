#define TreeStump_RootSig \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
	"RootConstants(b0, num32BitConstants = 4), " \
	"CBV(b1, visibility = SHADER_VISIBILITY_ALL), " \
	"DescriptorTable(SRV(t0, numDescriptors = 2), visibility = SHADER_VISIBILITY_PIXEL), " \
	"DescriptorTable(SRV(t2, numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL), " \
	"StaticSampler(s0," \
		"addressU = TEXTURE_ADDRESS_CLAMP, " \
		"addressV = TEXTURE_ADDRESS_CLAMP, " \
		"addressW = TEXTURE_ADDRESS_CLAMP, " \
		"filter = FILTER_MIN_MAG_MIP_LINEAR), "\
	"StaticSampler(s1," \
		"addressU = TEXTURE_ADDRESS_BORDER," \
		"addressV = TEXTURE_ADDRESS_BORDER," \
		"addressW = TEXTURE_ADDRESS_BORDER," \
		"filter = FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,"\
		"mipLODBias =0 ,"\
		"maxAnisotropy = 16,"\
		"comparisonFunc = COMPARISON_LESS_EQUAL,"\
		"borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK),"