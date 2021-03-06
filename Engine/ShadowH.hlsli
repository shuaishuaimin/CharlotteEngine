#define Shadow_RootSig\
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), "\
	"CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), "\
	"CBV(b1, visibility = SHADER_VISIBILITY_VERTEX), "\
	"DescriptorTable(SRV(t0, numDescriptors = 2), visibility = SHADER_VISIBILITY_PIXEL), "\
	"StaticSampler(s0, "\
		"addressU = TEXTURE_ADDRESS_CLAMP,"\
		"addressV = TEXTURE_ADDRESS_CLAMP,"\
		"addressW = TEXTURE_ADDRESS_CLAMP,"\
		"filter = FILTER_MIN_MAG_MIP_LINEAR),"