#include "interp_handler.h"

static opcode_handler_t original_handler[2000] = { 0 };

static int interp_handler_0(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[0] TSRMLS_DC);
}
static int interp_handler_1(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1] TSRMLS_DC);
}
static int interp_handler_2(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[2] TSRMLS_DC);
}
static int interp_handler_3(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[3] TSRMLS_DC);
}
static int interp_handler_4(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[4] TSRMLS_DC);
}
static int interp_handler_5(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[5] TSRMLS_DC);
}
static int interp_handler_6(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[6] TSRMLS_DC);
}
static int interp_handler_7(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[7] TSRMLS_DC);
}
static int interp_handler_8(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[8] TSRMLS_DC);
}
static int interp_handler_9(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[9] TSRMLS_DC);
}
static int interp_handler_10(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[10] TSRMLS_DC);
}
static int interp_handler_11(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[11] TSRMLS_DC);
}
static int interp_handler_12(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[12] TSRMLS_DC);
}
static int interp_handler_13(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[13] TSRMLS_DC);
}
static int interp_handler_14(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[14] TSRMLS_DC);
}
static int interp_handler_15(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[15] TSRMLS_DC);
}
static int interp_handler_16(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[16] TSRMLS_DC);
}
static int interp_handler_17(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[17] TSRMLS_DC);
}
static int interp_handler_18(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[18] TSRMLS_DC);
}
static int interp_handler_19(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[19] TSRMLS_DC);
}
static int interp_handler_20(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[20] TSRMLS_DC);
}
static int interp_handler_21(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[21] TSRMLS_DC);
}
static int interp_handler_22(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[22] TSRMLS_DC);
}
static int interp_handler_23(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[23] TSRMLS_DC);
}
static int interp_handler_24(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[24] TSRMLS_DC);
}
static int interp_handler_25(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[25] TSRMLS_DC);
}
static int interp_handler_26(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[26] TSRMLS_DC);
}
static int interp_handler_27(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[27] TSRMLS_DC);
}
static int interp_handler_28(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[28] TSRMLS_DC);
}
static int interp_handler_29(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[29] TSRMLS_DC);
}
static int interp_handler_30(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[30] TSRMLS_DC);
}
static int interp_handler_31(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[31] TSRMLS_DC);
}
static int interp_handler_32(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[32] TSRMLS_DC);
}
static int interp_handler_33(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[33] TSRMLS_DC);
}
static int interp_handler_34(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[34] TSRMLS_DC);
}
static int interp_handler_35(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[35] TSRMLS_DC);
}
static int interp_handler_36(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[36] TSRMLS_DC);
}
static int interp_handler_37(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[37] TSRMLS_DC);
}
static int interp_handler_38(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[38] TSRMLS_DC);
}
static int interp_handler_39(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[39] TSRMLS_DC);
}
static int interp_handler_40(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[40] TSRMLS_DC);
}
static int interp_handler_41(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[41] TSRMLS_DC);
}
static int interp_handler_42(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[42] TSRMLS_DC);
}
static int interp_handler_43(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[43] TSRMLS_DC);
}
static int interp_handler_44(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[44] TSRMLS_DC);
}
static int interp_handler_45(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[45] TSRMLS_DC);
}
static int interp_handler_46(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[46] TSRMLS_DC);
}
static int interp_handler_47(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[47] TSRMLS_DC);
}
static int interp_handler_48(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[48] TSRMLS_DC);
}
static int interp_handler_49(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[49] TSRMLS_DC);
}
static int interp_handler_50(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[50] TSRMLS_DC);
}
static int interp_handler_51(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[51] TSRMLS_DC);
}
static int interp_handler_52(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[52] TSRMLS_DC);
}
static int interp_handler_53(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[53] TSRMLS_DC);
}
static int interp_handler_54(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[54] TSRMLS_DC);
}
static int interp_handler_55(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[55] TSRMLS_DC);
}
static int interp_handler_56(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[56] TSRMLS_DC);
}
static int interp_handler_57(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[57] TSRMLS_DC);
}
static int interp_handler_58(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[58] TSRMLS_DC);
}
static int interp_handler_59(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[59] TSRMLS_DC);
}
static int interp_handler_60(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[60] TSRMLS_DC);
}
static int interp_handler_61(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[61] TSRMLS_DC);
}
static int interp_handler_62(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[62] TSRMLS_DC);
}
static int interp_handler_63(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[63] TSRMLS_DC);
}
static int interp_handler_64(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[64] TSRMLS_DC);
}
static int interp_handler_65(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[65] TSRMLS_DC);
}
static int interp_handler_66(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[66] TSRMLS_DC);
}
static int interp_handler_67(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[67] TSRMLS_DC);
}
static int interp_handler_68(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[68] TSRMLS_DC);
}
static int interp_handler_69(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[69] TSRMLS_DC);
}
static int interp_handler_70(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[70] TSRMLS_DC);
}
static int interp_handler_71(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[71] TSRMLS_DC);
}
static int interp_handler_72(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[72] TSRMLS_DC);
}
static int interp_handler_73(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[73] TSRMLS_DC);
}
static int interp_handler_74(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[74] TSRMLS_DC);
}
static int interp_handler_75(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[75] TSRMLS_DC);
}
static int interp_handler_76(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[76] TSRMLS_DC);
}
static int interp_handler_77(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[77] TSRMLS_DC);
}
static int interp_handler_78(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[78] TSRMLS_DC);
}
static int interp_handler_79(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[79] TSRMLS_DC);
}
static int interp_handler_80(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[80] TSRMLS_DC);
}
static int interp_handler_81(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[81] TSRMLS_DC);
}
static int interp_handler_82(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[82] TSRMLS_DC);
}
static int interp_handler_83(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[83] TSRMLS_DC);
}
static int interp_handler_84(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[84] TSRMLS_DC);
}
static int interp_handler_85(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[85] TSRMLS_DC);
}
static int interp_handler_86(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[86] TSRMLS_DC);
}
static int interp_handler_87(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[87] TSRMLS_DC);
}
static int interp_handler_88(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[88] TSRMLS_DC);
}
static int interp_handler_89(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[89] TSRMLS_DC);
}
static int interp_handler_90(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[90] TSRMLS_DC);
}
static int interp_handler_91(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[91] TSRMLS_DC);
}
static int interp_handler_92(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[92] TSRMLS_DC);
}
static int interp_handler_93(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[93] TSRMLS_DC);
}
static int interp_handler_94(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[94] TSRMLS_DC);
}
static int interp_handler_95(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[95] TSRMLS_DC);
}
static int interp_handler_96(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[96] TSRMLS_DC);
}
static int interp_handler_97(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[97] TSRMLS_DC);
}
static int interp_handler_98(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[98] TSRMLS_DC);
}
static int interp_handler_99(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[99] TSRMLS_DC);
}
static int interp_handler_100(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[100] TSRMLS_DC);
}
static int interp_handler_101(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[101] TSRMLS_DC);
}
static int interp_handler_102(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[102] TSRMLS_DC);
}
static int interp_handler_103(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[103] TSRMLS_DC);
}
static int interp_handler_104(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[104] TSRMLS_DC);
}
static int interp_handler_105(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[105] TSRMLS_DC);
}
static int interp_handler_106(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[106] TSRMLS_DC);
}
static int interp_handler_107(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[107] TSRMLS_DC);
}
static int interp_handler_108(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[108] TSRMLS_DC);
}
static int interp_handler_109(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[109] TSRMLS_DC);
}
static int interp_handler_110(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[110] TSRMLS_DC);
}
static int interp_handler_111(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[111] TSRMLS_DC);
}
static int interp_handler_112(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[112] TSRMLS_DC);
}
static int interp_handler_113(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[113] TSRMLS_DC);
}
static int interp_handler_114(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[114] TSRMLS_DC);
}
static int interp_handler_115(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[115] TSRMLS_DC);
}
static int interp_handler_116(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[116] TSRMLS_DC);
}
static int interp_handler_117(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[117] TSRMLS_DC);
}
static int interp_handler_118(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[118] TSRMLS_DC);
}
static int interp_handler_119(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[119] TSRMLS_DC);
}
static int interp_handler_120(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[120] TSRMLS_DC);
}
static int interp_handler_121(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[121] TSRMLS_DC);
}
static int interp_handler_122(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[122] TSRMLS_DC);
}
static int interp_handler_123(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[123] TSRMLS_DC);
}
static int interp_handler_124(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[124] TSRMLS_DC);
}
static int interp_handler_125(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[125] TSRMLS_DC);
}
static int interp_handler_126(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[126] TSRMLS_DC);
}
static int interp_handler_127(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[127] TSRMLS_DC);
}
static int interp_handler_128(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[128] TSRMLS_DC);
}
static int interp_handler_129(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[129] TSRMLS_DC);
}
static int interp_handler_130(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[130] TSRMLS_DC);
}
static int interp_handler_131(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[131] TSRMLS_DC);
}
static int interp_handler_132(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[132] TSRMLS_DC);
}
static int interp_handler_133(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[133] TSRMLS_DC);
}
static int interp_handler_134(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[134] TSRMLS_DC);
}
static int interp_handler_135(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[135] TSRMLS_DC);
}
static int interp_handler_136(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[136] TSRMLS_DC);
}
static int interp_handler_137(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[137] TSRMLS_DC);
}
static int interp_handler_138(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[138] TSRMLS_DC);
}
static int interp_handler_139(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[139] TSRMLS_DC);
}
static int interp_handler_140(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[140] TSRMLS_DC);
}
static int interp_handler_141(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[141] TSRMLS_DC);
}
static int interp_handler_142(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[142] TSRMLS_DC);
}
static int interp_handler_143(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[143] TSRMLS_DC);
}
static int interp_handler_144(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[144] TSRMLS_DC);
}
static int interp_handler_145(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[145] TSRMLS_DC);
}
static int interp_handler_146(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[146] TSRMLS_DC);
}
static int interp_handler_147(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[147] TSRMLS_DC);
}
static int interp_handler_148(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[148] TSRMLS_DC);
}
static int interp_handler_149(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[149] TSRMLS_DC);
}
static int interp_handler_150(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[150] TSRMLS_DC);
}
static int interp_handler_151(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[151] TSRMLS_DC);
}
static int interp_handler_152(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[152] TSRMLS_DC);
}
static int interp_handler_153(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[153] TSRMLS_DC);
}
static int interp_handler_154(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[154] TSRMLS_DC);
}
static int interp_handler_155(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[155] TSRMLS_DC);
}
static int interp_handler_156(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[156] TSRMLS_DC);
}
static int interp_handler_157(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[157] TSRMLS_DC);
}
static int interp_handler_158(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[158] TSRMLS_DC);
}
static int interp_handler_159(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[159] TSRMLS_DC);
}
static int interp_handler_160(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[160] TSRMLS_DC);
}
static int interp_handler_161(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[161] TSRMLS_DC);
}
static int interp_handler_162(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[162] TSRMLS_DC);
}
static int interp_handler_163(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[163] TSRMLS_DC);
}
static int interp_handler_164(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[164] TSRMLS_DC);
}
static int interp_handler_165(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[165] TSRMLS_DC);
}
static int interp_handler_166(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[166] TSRMLS_DC);
}
static int interp_handler_167(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[167] TSRMLS_DC);
}
static int interp_handler_168(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[168] TSRMLS_DC);
}
static int interp_handler_169(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[169] TSRMLS_DC);
}
static int interp_handler_170(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[170] TSRMLS_DC);
}
static int interp_handler_171(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[171] TSRMLS_DC);
}
static int interp_handler_172(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[172] TSRMLS_DC);
}
static int interp_handler_173(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[173] TSRMLS_DC);
}
static int interp_handler_174(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[174] TSRMLS_DC);
}
static int interp_handler_175(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[175] TSRMLS_DC);
}
static int interp_handler_176(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[176] TSRMLS_DC);
}
static int interp_handler_177(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[177] TSRMLS_DC);
}
static int interp_handler_178(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[178] TSRMLS_DC);
}
static int interp_handler_179(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[179] TSRMLS_DC);
}
static int interp_handler_180(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[180] TSRMLS_DC);
}
static int interp_handler_181(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[181] TSRMLS_DC);
}
static int interp_handler_182(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[182] TSRMLS_DC);
}
static int interp_handler_183(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[183] TSRMLS_DC);
}
static int interp_handler_184(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[184] TSRMLS_DC);
}
static int interp_handler_185(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[185] TSRMLS_DC);
}
static int interp_handler_186(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[186] TSRMLS_DC);
}
static int interp_handler_187(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[187] TSRMLS_DC);
}
static int interp_handler_188(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[188] TSRMLS_DC);
}
static int interp_handler_189(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[189] TSRMLS_DC);
}
static int interp_handler_190(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[190] TSRMLS_DC);
}
static int interp_handler_191(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[191] TSRMLS_DC);
}
static int interp_handler_192(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[192] TSRMLS_DC);
}
static int interp_handler_193(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[193] TSRMLS_DC);
}
static int interp_handler_194(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[194] TSRMLS_DC);
}
static int interp_handler_195(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[195] TSRMLS_DC);
}
static int interp_handler_196(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[196] TSRMLS_DC);
}
static int interp_handler_197(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[197] TSRMLS_DC);
}
static int interp_handler_198(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[198] TSRMLS_DC);
}
static int interp_handler_199(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[199] TSRMLS_DC);
}
static int interp_handler_200(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[200] TSRMLS_DC);
}
static int interp_handler_201(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[201] TSRMLS_DC);
}
static int interp_handler_202(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[202] TSRMLS_DC);
}
static int interp_handler_203(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[203] TSRMLS_DC);
}
static int interp_handler_204(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[204] TSRMLS_DC);
}
static int interp_handler_205(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[205] TSRMLS_DC);
}
static int interp_handler_206(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[206] TSRMLS_DC);
}
static int interp_handler_207(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[207] TSRMLS_DC);
}
static int interp_handler_208(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[208] TSRMLS_DC);
}
static int interp_handler_209(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[209] TSRMLS_DC);
}
static int interp_handler_210(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[210] TSRMLS_DC);
}
static int interp_handler_211(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[211] TSRMLS_DC);
}
static int interp_handler_212(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[212] TSRMLS_DC);
}
static int interp_handler_213(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[213] TSRMLS_DC);
}
static int interp_handler_214(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[214] TSRMLS_DC);
}
static int interp_handler_215(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[215] TSRMLS_DC);
}
static int interp_handler_216(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[216] TSRMLS_DC);
}
static int interp_handler_217(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[217] TSRMLS_DC);
}
static int interp_handler_218(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[218] TSRMLS_DC);
}
static int interp_handler_219(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[219] TSRMLS_DC);
}
static int interp_handler_220(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[220] TSRMLS_DC);
}
static int interp_handler_221(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[221] TSRMLS_DC);
}
static int interp_handler_222(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[222] TSRMLS_DC);
}
static int interp_handler_223(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[223] TSRMLS_DC);
}
static int interp_handler_224(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[224] TSRMLS_DC);
}
static int interp_handler_225(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[225] TSRMLS_DC);
}
static int interp_handler_226(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[226] TSRMLS_DC);
}
static int interp_handler_227(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[227] TSRMLS_DC);
}
static int interp_handler_228(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[228] TSRMLS_DC);
}
static int interp_handler_229(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[229] TSRMLS_DC);
}
static int interp_handler_230(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[230] TSRMLS_DC);
}
static int interp_handler_231(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[231] TSRMLS_DC);
}
static int interp_handler_232(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[232] TSRMLS_DC);
}
static int interp_handler_233(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[233] TSRMLS_DC);
}
static int interp_handler_234(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[234] TSRMLS_DC);
}
static int interp_handler_235(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[235] TSRMLS_DC);
}
static int interp_handler_236(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[236] TSRMLS_DC);
}
static int interp_handler_237(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[237] TSRMLS_DC);
}
static int interp_handler_238(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[238] TSRMLS_DC);
}
static int interp_handler_239(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[239] TSRMLS_DC);
}
static int interp_handler_240(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[240] TSRMLS_DC);
}
static int interp_handler_241(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[241] TSRMLS_DC);
}
static int interp_handler_242(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[242] TSRMLS_DC);
}
static int interp_handler_243(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[243] TSRMLS_DC);
}
static int interp_handler_244(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[244] TSRMLS_DC);
}
static int interp_handler_245(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[245] TSRMLS_DC);
}
static int interp_handler_246(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[246] TSRMLS_DC);
}
static int interp_handler_247(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[247] TSRMLS_DC);
}
static int interp_handler_248(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[248] TSRMLS_DC);
}
static int interp_handler_249(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[249] TSRMLS_DC);
}
static int interp_handler_250(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[250] TSRMLS_DC);
}
static int interp_handler_251(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[251] TSRMLS_DC);
}
static int interp_handler_252(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[252] TSRMLS_DC);
}
static int interp_handler_253(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[253] TSRMLS_DC);
}
static int interp_handler_254(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[254] TSRMLS_DC);
}
static int interp_handler_255(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[255] TSRMLS_DC);
}
static int interp_handler_256(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[256] TSRMLS_DC);
}
static int interp_handler_257(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[257] TSRMLS_DC);
}
static int interp_handler_258(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[258] TSRMLS_DC);
}
static int interp_handler_259(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[259] TSRMLS_DC);
}
static int interp_handler_260(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[260] TSRMLS_DC);
}
static int interp_handler_261(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[261] TSRMLS_DC);
}
static int interp_handler_262(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[262] TSRMLS_DC);
}
static int interp_handler_263(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[263] TSRMLS_DC);
}
static int interp_handler_264(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[264] TSRMLS_DC);
}
static int interp_handler_265(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[265] TSRMLS_DC);
}
static int interp_handler_266(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[266] TSRMLS_DC);
}
static int interp_handler_267(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[267] TSRMLS_DC);
}
static int interp_handler_268(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[268] TSRMLS_DC);
}
static int interp_handler_269(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[269] TSRMLS_DC);
}
static int interp_handler_270(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[270] TSRMLS_DC);
}
static int interp_handler_271(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[271] TSRMLS_DC);
}
static int interp_handler_272(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[272] TSRMLS_DC);
}
static int interp_handler_273(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[273] TSRMLS_DC);
}
static int interp_handler_274(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[274] TSRMLS_DC);
}
static int interp_handler_275(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[275] TSRMLS_DC);
}
static int interp_handler_276(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[276] TSRMLS_DC);
}
static int interp_handler_277(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[277] TSRMLS_DC);
}
static int interp_handler_278(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[278] TSRMLS_DC);
}
static int interp_handler_279(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[279] TSRMLS_DC);
}
static int interp_handler_280(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[280] TSRMLS_DC);
}
static int interp_handler_281(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[281] TSRMLS_DC);
}
static int interp_handler_282(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[282] TSRMLS_DC);
}
static int interp_handler_283(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[283] TSRMLS_DC);
}
static int interp_handler_284(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[284] TSRMLS_DC);
}
static int interp_handler_285(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[285] TSRMLS_DC);
}
static int interp_handler_286(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[286] TSRMLS_DC);
}
static int interp_handler_287(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[287] TSRMLS_DC);
}
static int interp_handler_288(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[288] TSRMLS_DC);
}
static int interp_handler_289(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[289] TSRMLS_DC);
}
static int interp_handler_290(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[290] TSRMLS_DC);
}
static int interp_handler_291(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[291] TSRMLS_DC);
}
static int interp_handler_292(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[292] TSRMLS_DC);
}
static int interp_handler_293(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[293] TSRMLS_DC);
}
static int interp_handler_294(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[294] TSRMLS_DC);
}
static int interp_handler_295(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[295] TSRMLS_DC);
}
static int interp_handler_296(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[296] TSRMLS_DC);
}
static int interp_handler_297(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[297] TSRMLS_DC);
}
static int interp_handler_298(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[298] TSRMLS_DC);
}
static int interp_handler_299(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[299] TSRMLS_DC);
}
static int interp_handler_300(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[300] TSRMLS_DC);
}
static int interp_handler_301(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[301] TSRMLS_DC);
}
static int interp_handler_302(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[302] TSRMLS_DC);
}
static int interp_handler_303(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[303] TSRMLS_DC);
}
static int interp_handler_304(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[304] TSRMLS_DC);
}
static int interp_handler_305(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[305] TSRMLS_DC);
}
static int interp_handler_306(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[306] TSRMLS_DC);
}
static int interp_handler_307(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[307] TSRMLS_DC);
}
static int interp_handler_308(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[308] TSRMLS_DC);
}
static int interp_handler_309(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[309] TSRMLS_DC);
}
static int interp_handler_310(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[310] TSRMLS_DC);
}
static int interp_handler_311(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[311] TSRMLS_DC);
}
static int interp_handler_312(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[312] TSRMLS_DC);
}
static int interp_handler_313(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[313] TSRMLS_DC);
}
static int interp_handler_314(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[314] TSRMLS_DC);
}
static int interp_handler_315(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[315] TSRMLS_DC);
}
static int interp_handler_316(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[316] TSRMLS_DC);
}
static int interp_handler_317(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[317] TSRMLS_DC);
}
static int interp_handler_318(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[318] TSRMLS_DC);
}
static int interp_handler_319(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[319] TSRMLS_DC);
}
static int interp_handler_320(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[320] TSRMLS_DC);
}
static int interp_handler_321(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[321] TSRMLS_DC);
}
static int interp_handler_322(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[322] TSRMLS_DC);
}
static int interp_handler_323(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[323] TSRMLS_DC);
}
static int interp_handler_324(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[324] TSRMLS_DC);
}
static int interp_handler_325(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[325] TSRMLS_DC);
}
static int interp_handler_326(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[326] TSRMLS_DC);
}
static int interp_handler_327(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[327] TSRMLS_DC);
}
static int interp_handler_328(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[328] TSRMLS_DC);
}
static int interp_handler_329(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[329] TSRMLS_DC);
}
static int interp_handler_330(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[330] TSRMLS_DC);
}
static int interp_handler_331(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[331] TSRMLS_DC);
}
static int interp_handler_332(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[332] TSRMLS_DC);
}
static int interp_handler_333(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[333] TSRMLS_DC);
}
static int interp_handler_334(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[334] TSRMLS_DC);
}
static int interp_handler_335(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[335] TSRMLS_DC);
}
static int interp_handler_336(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[336] TSRMLS_DC);
}
static int interp_handler_337(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[337] TSRMLS_DC);
}
static int interp_handler_338(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[338] TSRMLS_DC);
}
static int interp_handler_339(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[339] TSRMLS_DC);
}
static int interp_handler_340(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[340] TSRMLS_DC);
}
static int interp_handler_341(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[341] TSRMLS_DC);
}
static int interp_handler_342(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[342] TSRMLS_DC);
}
static int interp_handler_343(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[343] TSRMLS_DC);
}
static int interp_handler_344(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[344] TSRMLS_DC);
}
static int interp_handler_345(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[345] TSRMLS_DC);
}
static int interp_handler_346(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[346] TSRMLS_DC);
}
static int interp_handler_347(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[347] TSRMLS_DC);
}
static int interp_handler_348(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[348] TSRMLS_DC);
}
static int interp_handler_349(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[349] TSRMLS_DC);
}
static int interp_handler_350(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[350] TSRMLS_DC);
}
static int interp_handler_351(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[351] TSRMLS_DC);
}
static int interp_handler_352(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[352] TSRMLS_DC);
}
static int interp_handler_353(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[353] TSRMLS_DC);
}
static int interp_handler_354(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[354] TSRMLS_DC);
}
static int interp_handler_355(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[355] TSRMLS_DC);
}
static int interp_handler_356(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[356] TSRMLS_DC);
}
static int interp_handler_357(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[357] TSRMLS_DC);
}
static int interp_handler_358(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[358] TSRMLS_DC);
}
static int interp_handler_359(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[359] TSRMLS_DC);
}
static int interp_handler_360(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[360] TSRMLS_DC);
}
static int interp_handler_361(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[361] TSRMLS_DC);
}
static int interp_handler_362(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[362] TSRMLS_DC);
}
static int interp_handler_363(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[363] TSRMLS_DC);
}
static int interp_handler_364(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[364] TSRMLS_DC);
}
static int interp_handler_365(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[365] TSRMLS_DC);
}
static int interp_handler_366(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[366] TSRMLS_DC);
}
static int interp_handler_367(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[367] TSRMLS_DC);
}
static int interp_handler_368(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[368] TSRMLS_DC);
}
static int interp_handler_369(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[369] TSRMLS_DC);
}
static int interp_handler_370(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[370] TSRMLS_DC);
}
static int interp_handler_371(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[371] TSRMLS_DC);
}
static int interp_handler_372(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[372] TSRMLS_DC);
}
static int interp_handler_373(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[373] TSRMLS_DC);
}
static int interp_handler_374(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[374] TSRMLS_DC);
}
static int interp_handler_375(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[375] TSRMLS_DC);
}
static int interp_handler_376(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[376] TSRMLS_DC);
}
static int interp_handler_377(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[377] TSRMLS_DC);
}
static int interp_handler_378(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[378] TSRMLS_DC);
}
static int interp_handler_379(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[379] TSRMLS_DC);
}
static int interp_handler_380(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[380] TSRMLS_DC);
}
static int interp_handler_381(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[381] TSRMLS_DC);
}
static int interp_handler_382(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[382] TSRMLS_DC);
}
static int interp_handler_383(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[383] TSRMLS_DC);
}
static int interp_handler_384(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[384] TSRMLS_DC);
}
static int interp_handler_385(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[385] TSRMLS_DC);
}
static int interp_handler_386(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[386] TSRMLS_DC);
}
static int interp_handler_387(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[387] TSRMLS_DC);
}
static int interp_handler_388(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[388] TSRMLS_DC);
}
static int interp_handler_389(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[389] TSRMLS_DC);
}
static int interp_handler_390(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[390] TSRMLS_DC);
}
static int interp_handler_391(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[391] TSRMLS_DC);
}
static int interp_handler_392(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[392] TSRMLS_DC);
}
static int interp_handler_393(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[393] TSRMLS_DC);
}
static int interp_handler_394(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[394] TSRMLS_DC);
}
static int interp_handler_395(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[395] TSRMLS_DC);
}
static int interp_handler_396(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[396] TSRMLS_DC);
}
static int interp_handler_397(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[397] TSRMLS_DC);
}
static int interp_handler_398(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[398] TSRMLS_DC);
}
static int interp_handler_399(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[399] TSRMLS_DC);
}
static int interp_handler_400(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[400] TSRMLS_DC);
}
static int interp_handler_401(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[401] TSRMLS_DC);
}
static int interp_handler_402(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[402] TSRMLS_DC);
}
static int interp_handler_403(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[403] TSRMLS_DC);
}
static int interp_handler_404(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[404] TSRMLS_DC);
}
static int interp_handler_405(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[405] TSRMLS_DC);
}
static int interp_handler_406(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[406] TSRMLS_DC);
}
static int interp_handler_407(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[407] TSRMLS_DC);
}
static int interp_handler_408(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[408] TSRMLS_DC);
}
static int interp_handler_409(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[409] TSRMLS_DC);
}
static int interp_handler_410(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[410] TSRMLS_DC);
}
static int interp_handler_411(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[411] TSRMLS_DC);
}
static int interp_handler_412(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[412] TSRMLS_DC);
}
static int interp_handler_413(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[413] TSRMLS_DC);
}
static int interp_handler_414(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[414] TSRMLS_DC);
}
static int interp_handler_415(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[415] TSRMLS_DC);
}
static int interp_handler_416(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[416] TSRMLS_DC);
}
static int interp_handler_417(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[417] TSRMLS_DC);
}
static int interp_handler_418(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[418] TSRMLS_DC);
}
static int interp_handler_419(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[419] TSRMLS_DC);
}
static int interp_handler_420(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[420] TSRMLS_DC);
}
static int interp_handler_421(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[421] TSRMLS_DC);
}
static int interp_handler_422(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[422] TSRMLS_DC);
}
static int interp_handler_423(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[423] TSRMLS_DC);
}
static int interp_handler_424(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[424] TSRMLS_DC);
}
static int interp_handler_425(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[425] TSRMLS_DC);
}
static int interp_handler_426(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[426] TSRMLS_DC);
}
static int interp_handler_427(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[427] TSRMLS_DC);
}
static int interp_handler_428(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[428] TSRMLS_DC);
}
static int interp_handler_429(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[429] TSRMLS_DC);
}
static int interp_handler_430(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[430] TSRMLS_DC);
}
static int interp_handler_431(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[431] TSRMLS_DC);
}
static int interp_handler_432(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[432] TSRMLS_DC);
}
static int interp_handler_433(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[433] TSRMLS_DC);
}
static int interp_handler_434(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[434] TSRMLS_DC);
}
static int interp_handler_435(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[435] TSRMLS_DC);
}
static int interp_handler_436(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[436] TSRMLS_DC);
}
static int interp_handler_437(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[437] TSRMLS_DC);
}
static int interp_handler_438(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[438] TSRMLS_DC);
}
static int interp_handler_439(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[439] TSRMLS_DC);
}
static int interp_handler_440(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[440] TSRMLS_DC);
}
static int interp_handler_441(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[441] TSRMLS_DC);
}
static int interp_handler_442(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[442] TSRMLS_DC);
}
static int interp_handler_443(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[443] TSRMLS_DC);
}
static int interp_handler_444(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[444] TSRMLS_DC);
}
static int interp_handler_445(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[445] TSRMLS_DC);
}
static int interp_handler_446(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[446] TSRMLS_DC);
}
static int interp_handler_447(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[447] TSRMLS_DC);
}
static int interp_handler_448(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[448] TSRMLS_DC);
}
static int interp_handler_449(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[449] TSRMLS_DC);
}
static int interp_handler_450(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[450] TSRMLS_DC);
}
static int interp_handler_451(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[451] TSRMLS_DC);
}
static int interp_handler_452(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[452] TSRMLS_DC);
}
static int interp_handler_453(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[453] TSRMLS_DC);
}
static int interp_handler_454(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[454] TSRMLS_DC);
}
static int interp_handler_455(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[455] TSRMLS_DC);
}
static int interp_handler_456(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[456] TSRMLS_DC);
}
static int interp_handler_457(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[457] TSRMLS_DC);
}
static int interp_handler_458(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[458] TSRMLS_DC);
}
static int interp_handler_459(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[459] TSRMLS_DC);
}
static int interp_handler_460(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[460] TSRMLS_DC);
}
static int interp_handler_461(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[461] TSRMLS_DC);
}
static int interp_handler_462(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[462] TSRMLS_DC);
}
static int interp_handler_463(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[463] TSRMLS_DC);
}
static int interp_handler_464(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[464] TSRMLS_DC);
}
static int interp_handler_465(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[465] TSRMLS_DC);
}
static int interp_handler_466(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[466] TSRMLS_DC);
}
static int interp_handler_467(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[467] TSRMLS_DC);
}
static int interp_handler_468(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[468] TSRMLS_DC);
}
static int interp_handler_469(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[469] TSRMLS_DC);
}
static int interp_handler_470(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[470] TSRMLS_DC);
}
static int interp_handler_471(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[471] TSRMLS_DC);
}
static int interp_handler_472(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[472] TSRMLS_DC);
}
static int interp_handler_473(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[473] TSRMLS_DC);
}
static int interp_handler_474(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[474] TSRMLS_DC);
}
static int interp_handler_475(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[475] TSRMLS_DC);
}
static int interp_handler_476(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[476] TSRMLS_DC);
}
static int interp_handler_477(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[477] TSRMLS_DC);
}
static int interp_handler_478(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[478] TSRMLS_DC);
}
static int interp_handler_479(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[479] TSRMLS_DC);
}
static int interp_handler_480(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[480] TSRMLS_DC);
}
static int interp_handler_481(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[481] TSRMLS_DC);
}
static int interp_handler_482(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[482] TSRMLS_DC);
}
static int interp_handler_483(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[483] TSRMLS_DC);
}
static int interp_handler_484(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[484] TSRMLS_DC);
}
static int interp_handler_485(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[485] TSRMLS_DC);
}
static int interp_handler_486(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[486] TSRMLS_DC);
}
static int interp_handler_487(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[487] TSRMLS_DC);
}
static int interp_handler_488(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[488] TSRMLS_DC);
}
static int interp_handler_489(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[489] TSRMLS_DC);
}
static int interp_handler_490(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[490] TSRMLS_DC);
}
static int interp_handler_491(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[491] TSRMLS_DC);
}
static int interp_handler_492(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[492] TSRMLS_DC);
}
static int interp_handler_493(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[493] TSRMLS_DC);
}
static int interp_handler_494(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[494] TSRMLS_DC);
}
static int interp_handler_495(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[495] TSRMLS_DC);
}
static int interp_handler_496(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[496] TSRMLS_DC);
}
static int interp_handler_497(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[497] TSRMLS_DC);
}
static int interp_handler_498(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[498] TSRMLS_DC);
}
static int interp_handler_499(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[499] TSRMLS_DC);
}
static int interp_handler_500(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[500] TSRMLS_DC);
}
static int interp_handler_501(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[501] TSRMLS_DC);
}
static int interp_handler_502(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[502] TSRMLS_DC);
}
static int interp_handler_503(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[503] TSRMLS_DC);
}
static int interp_handler_504(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[504] TSRMLS_DC);
}
static int interp_handler_505(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[505] TSRMLS_DC);
}
static int interp_handler_506(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[506] TSRMLS_DC);
}
static int interp_handler_507(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[507] TSRMLS_DC);
}
static int interp_handler_508(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[508] TSRMLS_DC);
}
static int interp_handler_509(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[509] TSRMLS_DC);
}
static int interp_handler_510(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[510] TSRMLS_DC);
}
static int interp_handler_511(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[511] TSRMLS_DC);
}
static int interp_handler_512(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[512] TSRMLS_DC);
}
static int interp_handler_513(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[513] TSRMLS_DC);
}
static int interp_handler_514(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[514] TSRMLS_DC);
}
static int interp_handler_515(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[515] TSRMLS_DC);
}
static int interp_handler_516(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[516] TSRMLS_DC);
}
static int interp_handler_517(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[517] TSRMLS_DC);
}
static int interp_handler_518(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[518] TSRMLS_DC);
}
static int interp_handler_519(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[519] TSRMLS_DC);
}
static int interp_handler_520(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[520] TSRMLS_DC);
}
static int interp_handler_521(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[521] TSRMLS_DC);
}
static int interp_handler_522(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[522] TSRMLS_DC);
}
static int interp_handler_523(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[523] TSRMLS_DC);
}
static int interp_handler_524(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[524] TSRMLS_DC);
}
static int interp_handler_525(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[525] TSRMLS_DC);
}
static int interp_handler_526(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[526] TSRMLS_DC);
}
static int interp_handler_527(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[527] TSRMLS_DC);
}
static int interp_handler_528(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[528] TSRMLS_DC);
}
static int interp_handler_529(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[529] TSRMLS_DC);
}
static int interp_handler_530(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[530] TSRMLS_DC);
}
static int interp_handler_531(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[531] TSRMLS_DC);
}
static int interp_handler_532(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[532] TSRMLS_DC);
}
static int interp_handler_533(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[533] TSRMLS_DC);
}
static int interp_handler_534(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[534] TSRMLS_DC);
}
static int interp_handler_535(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[535] TSRMLS_DC);
}
static int interp_handler_536(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[536] TSRMLS_DC);
}
static int interp_handler_537(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[537] TSRMLS_DC);
}
static int interp_handler_538(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[538] TSRMLS_DC);
}
static int interp_handler_539(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[539] TSRMLS_DC);
}
static int interp_handler_540(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[540] TSRMLS_DC);
}
static int interp_handler_541(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[541] TSRMLS_DC);
}
static int interp_handler_542(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[542] TSRMLS_DC);
}
static int interp_handler_543(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[543] TSRMLS_DC);
}
static int interp_handler_544(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[544] TSRMLS_DC);
}
static int interp_handler_545(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[545] TSRMLS_DC);
}
static int interp_handler_546(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[546] TSRMLS_DC);
}
static int interp_handler_547(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[547] TSRMLS_DC);
}
static int interp_handler_548(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[548] TSRMLS_DC);
}
static int interp_handler_549(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[549] TSRMLS_DC);
}
static int interp_handler_550(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[550] TSRMLS_DC);
}
static int interp_handler_551(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[551] TSRMLS_DC);
}
static int interp_handler_552(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[552] TSRMLS_DC);
}
static int interp_handler_553(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[553] TSRMLS_DC);
}
static int interp_handler_554(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[554] TSRMLS_DC);
}
static int interp_handler_555(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[555] TSRMLS_DC);
}
static int interp_handler_556(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[556] TSRMLS_DC);
}
static int interp_handler_557(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[557] TSRMLS_DC);
}
static int interp_handler_558(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[558] TSRMLS_DC);
}
static int interp_handler_559(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[559] TSRMLS_DC);
}
static int interp_handler_560(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[560] TSRMLS_DC);
}
static int interp_handler_561(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[561] TSRMLS_DC);
}
static int interp_handler_562(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[562] TSRMLS_DC);
}
static int interp_handler_563(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[563] TSRMLS_DC);
}
static int interp_handler_564(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[564] TSRMLS_DC);
}
static int interp_handler_565(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[565] TSRMLS_DC);
}
static int interp_handler_566(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[566] TSRMLS_DC);
}
static int interp_handler_567(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[567] TSRMLS_DC);
}
static int interp_handler_568(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[568] TSRMLS_DC);
}
static int interp_handler_569(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[569] TSRMLS_DC);
}
static int interp_handler_570(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[570] TSRMLS_DC);
}
static int interp_handler_571(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[571] TSRMLS_DC);
}
static int interp_handler_572(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[572] TSRMLS_DC);
}
static int interp_handler_573(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[573] TSRMLS_DC);
}
static int interp_handler_574(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[574] TSRMLS_DC);
}
static int interp_handler_575(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[575] TSRMLS_DC);
}
static int interp_handler_576(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[576] TSRMLS_DC);
}
static int interp_handler_577(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[577] TSRMLS_DC);
}
static int interp_handler_578(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[578] TSRMLS_DC);
}
static int interp_handler_579(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[579] TSRMLS_DC);
}
static int interp_handler_580(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[580] TSRMLS_DC);
}
static int interp_handler_581(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[581] TSRMLS_DC);
}
static int interp_handler_582(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[582] TSRMLS_DC);
}
static int interp_handler_583(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[583] TSRMLS_DC);
}
static int interp_handler_584(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[584] TSRMLS_DC);
}
static int interp_handler_585(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[585] TSRMLS_DC);
}
static int interp_handler_586(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[586] TSRMLS_DC);
}
static int interp_handler_587(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[587] TSRMLS_DC);
}
static int interp_handler_588(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[588] TSRMLS_DC);
}
static int interp_handler_589(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[589] TSRMLS_DC);
}
static int interp_handler_590(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[590] TSRMLS_DC);
}
static int interp_handler_591(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[591] TSRMLS_DC);
}
static int interp_handler_592(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[592] TSRMLS_DC);
}
static int interp_handler_593(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[593] TSRMLS_DC);
}
static int interp_handler_594(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[594] TSRMLS_DC);
}
static int interp_handler_595(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[595] TSRMLS_DC);
}
static int interp_handler_596(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[596] TSRMLS_DC);
}
static int interp_handler_597(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[597] TSRMLS_DC);
}
static int interp_handler_598(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[598] TSRMLS_DC);
}
static int interp_handler_599(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[599] TSRMLS_DC);
}
static int interp_handler_600(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[600] TSRMLS_DC);
}
static int interp_handler_601(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[601] TSRMLS_DC);
}
static int interp_handler_602(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[602] TSRMLS_DC);
}
static int interp_handler_603(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[603] TSRMLS_DC);
}
static int interp_handler_604(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[604] TSRMLS_DC);
}
static int interp_handler_605(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[605] TSRMLS_DC);
}
static int interp_handler_606(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[606] TSRMLS_DC);
}
static int interp_handler_607(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[607] TSRMLS_DC);
}
static int interp_handler_608(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[608] TSRMLS_DC);
}
static int interp_handler_609(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[609] TSRMLS_DC);
}
static int interp_handler_610(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[610] TSRMLS_DC);
}
static int interp_handler_611(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[611] TSRMLS_DC);
}
static int interp_handler_612(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[612] TSRMLS_DC);
}
static int interp_handler_613(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[613] TSRMLS_DC);
}
static int interp_handler_614(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[614] TSRMLS_DC);
}
static int interp_handler_615(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[615] TSRMLS_DC);
}
static int interp_handler_616(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[616] TSRMLS_DC);
}
static int interp_handler_617(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[617] TSRMLS_DC);
}
static int interp_handler_618(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[618] TSRMLS_DC);
}
static int interp_handler_619(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[619] TSRMLS_DC);
}
static int interp_handler_620(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[620] TSRMLS_DC);
}
static int interp_handler_621(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[621] TSRMLS_DC);
}
static int interp_handler_622(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[622] TSRMLS_DC);
}
static int interp_handler_623(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[623] TSRMLS_DC);
}
static int interp_handler_624(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[624] TSRMLS_DC);
}
static int interp_handler_625(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[625] TSRMLS_DC);
}
static int interp_handler_626(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[626] TSRMLS_DC);
}
static int interp_handler_627(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[627] TSRMLS_DC);
}
static int interp_handler_628(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[628] TSRMLS_DC);
}
static int interp_handler_629(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[629] TSRMLS_DC);
}
static int interp_handler_630(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[630] TSRMLS_DC);
}
static int interp_handler_631(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[631] TSRMLS_DC);
}
static int interp_handler_632(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[632] TSRMLS_DC);
}
static int interp_handler_633(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[633] TSRMLS_DC);
}
static int interp_handler_634(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[634] TSRMLS_DC);
}
static int interp_handler_635(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[635] TSRMLS_DC);
}
static int interp_handler_636(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[636] TSRMLS_DC);
}
static int interp_handler_637(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[637] TSRMLS_DC);
}
static int interp_handler_638(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[638] TSRMLS_DC);
}
static int interp_handler_639(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[639] TSRMLS_DC);
}
static int interp_handler_640(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[640] TSRMLS_DC);
}
static int interp_handler_641(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[641] TSRMLS_DC);
}
static int interp_handler_642(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[642] TSRMLS_DC);
}
static int interp_handler_643(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[643] TSRMLS_DC);
}
static int interp_handler_644(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[644] TSRMLS_DC);
}
static int interp_handler_645(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[645] TSRMLS_DC);
}
static int interp_handler_646(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[646] TSRMLS_DC);
}
static int interp_handler_647(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[647] TSRMLS_DC);
}
static int interp_handler_648(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[648] TSRMLS_DC);
}
static int interp_handler_649(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[649] TSRMLS_DC);
}
static int interp_handler_650(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[650] TSRMLS_DC);
}
static int interp_handler_651(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[651] TSRMLS_DC);
}
static int interp_handler_652(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[652] TSRMLS_DC);
}
static int interp_handler_653(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[653] TSRMLS_DC);
}
static int interp_handler_654(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[654] TSRMLS_DC);
}
static int interp_handler_655(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[655] TSRMLS_DC);
}
static int interp_handler_656(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[656] TSRMLS_DC);
}
static int interp_handler_657(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[657] TSRMLS_DC);
}
static int interp_handler_658(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[658] TSRMLS_DC);
}
static int interp_handler_659(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[659] TSRMLS_DC);
}
static int interp_handler_660(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[660] TSRMLS_DC);
}
static int interp_handler_661(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[661] TSRMLS_DC);
}
static int interp_handler_662(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[662] TSRMLS_DC);
}
static int interp_handler_663(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[663] TSRMLS_DC);
}
static int interp_handler_664(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[664] TSRMLS_DC);
}
static int interp_handler_665(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[665] TSRMLS_DC);
}
static int interp_handler_666(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[666] TSRMLS_DC);
}
static int interp_handler_667(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[667] TSRMLS_DC);
}
static int interp_handler_668(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[668] TSRMLS_DC);
}
static int interp_handler_669(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[669] TSRMLS_DC);
}
static int interp_handler_670(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[670] TSRMLS_DC);
}
static int interp_handler_671(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[671] TSRMLS_DC);
}
static int interp_handler_672(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[672] TSRMLS_DC);
}
static int interp_handler_673(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[673] TSRMLS_DC);
}
static int interp_handler_674(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[674] TSRMLS_DC);
}
static int interp_handler_675(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[675] TSRMLS_DC);
}
static int interp_handler_676(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[676] TSRMLS_DC);
}
static int interp_handler_677(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[677] TSRMLS_DC);
}
static int interp_handler_678(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[678] TSRMLS_DC);
}
static int interp_handler_679(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[679] TSRMLS_DC);
}
static int interp_handler_680(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[680] TSRMLS_DC);
}
static int interp_handler_681(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[681] TSRMLS_DC);
}
static int interp_handler_682(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[682] TSRMLS_DC);
}
static int interp_handler_683(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[683] TSRMLS_DC);
}
static int interp_handler_684(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[684] TSRMLS_DC);
}
static int interp_handler_685(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[685] TSRMLS_DC);
}
static int interp_handler_686(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[686] TSRMLS_DC);
}
static int interp_handler_687(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[687] TSRMLS_DC);
}
static int interp_handler_688(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[688] TSRMLS_DC);
}
static int interp_handler_689(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[689] TSRMLS_DC);
}
static int interp_handler_690(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[690] TSRMLS_DC);
}
static int interp_handler_691(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[691] TSRMLS_DC);
}
static int interp_handler_692(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[692] TSRMLS_DC);
}
static int interp_handler_693(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[693] TSRMLS_DC);
}
static int interp_handler_694(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[694] TSRMLS_DC);
}
static int interp_handler_695(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[695] TSRMLS_DC);
}
static int interp_handler_696(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[696] TSRMLS_DC);
}
static int interp_handler_697(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[697] TSRMLS_DC);
}
static int interp_handler_698(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[698] TSRMLS_DC);
}
static int interp_handler_699(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[699] TSRMLS_DC);
}
static int interp_handler_700(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[700] TSRMLS_DC);
}
static int interp_handler_701(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[701] TSRMLS_DC);
}
static int interp_handler_702(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[702] TSRMLS_DC);
}
static int interp_handler_703(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[703] TSRMLS_DC);
}
static int interp_handler_704(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[704] TSRMLS_DC);
}
static int interp_handler_705(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[705] TSRMLS_DC);
}
static int interp_handler_706(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[706] TSRMLS_DC);
}
static int interp_handler_707(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[707] TSRMLS_DC);
}
static int interp_handler_708(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[708] TSRMLS_DC);
}
static int interp_handler_709(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[709] TSRMLS_DC);
}
static int interp_handler_710(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[710] TSRMLS_DC);
}
static int interp_handler_711(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[711] TSRMLS_DC);
}
static int interp_handler_712(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[712] TSRMLS_DC);
}
static int interp_handler_713(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[713] TSRMLS_DC);
}
static int interp_handler_714(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[714] TSRMLS_DC);
}
static int interp_handler_715(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[715] TSRMLS_DC);
}
static int interp_handler_716(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[716] TSRMLS_DC);
}
static int interp_handler_717(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[717] TSRMLS_DC);
}
static int interp_handler_718(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[718] TSRMLS_DC);
}
static int interp_handler_719(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[719] TSRMLS_DC);
}
static int interp_handler_720(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[720] TSRMLS_DC);
}
static int interp_handler_721(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[721] TSRMLS_DC);
}
static int interp_handler_722(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[722] TSRMLS_DC);
}
static int interp_handler_723(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[723] TSRMLS_DC);
}
static int interp_handler_724(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[724] TSRMLS_DC);
}
static int interp_handler_725(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[725] TSRMLS_DC);
}
static int interp_handler_726(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[726] TSRMLS_DC);
}
static int interp_handler_727(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[727] TSRMLS_DC);
}
static int interp_handler_728(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[728] TSRMLS_DC);
}
static int interp_handler_729(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[729] TSRMLS_DC);
}
static int interp_handler_730(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[730] TSRMLS_DC);
}
static int interp_handler_731(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[731] TSRMLS_DC);
}
static int interp_handler_732(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[732] TSRMLS_DC);
}
static int interp_handler_733(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[733] TSRMLS_DC);
}
static int interp_handler_734(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[734] TSRMLS_DC);
}
static int interp_handler_735(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[735] TSRMLS_DC);
}
static int interp_handler_736(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[736] TSRMLS_DC);
}
static int interp_handler_737(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[737] TSRMLS_DC);
}
static int interp_handler_738(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[738] TSRMLS_DC);
}
static int interp_handler_739(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[739] TSRMLS_DC);
}
static int interp_handler_740(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[740] TSRMLS_DC);
}
static int interp_handler_741(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[741] TSRMLS_DC);
}
static int interp_handler_742(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[742] TSRMLS_DC);
}
static int interp_handler_743(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[743] TSRMLS_DC);
}
static int interp_handler_744(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[744] TSRMLS_DC);
}
static int interp_handler_745(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[745] TSRMLS_DC);
}
static int interp_handler_746(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[746] TSRMLS_DC);
}
static int interp_handler_747(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[747] TSRMLS_DC);
}
static int interp_handler_748(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[748] TSRMLS_DC);
}
static int interp_handler_749(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[749] TSRMLS_DC);
}
static int interp_handler_750(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[750] TSRMLS_DC);
}
static int interp_handler_751(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[751] TSRMLS_DC);
}
static int interp_handler_752(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[752] TSRMLS_DC);
}
static int interp_handler_753(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[753] TSRMLS_DC);
}
static int interp_handler_754(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[754] TSRMLS_DC);
}
static int interp_handler_755(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[755] TSRMLS_DC);
}
static int interp_handler_756(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[756] TSRMLS_DC);
}
static int interp_handler_757(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[757] TSRMLS_DC);
}
static int interp_handler_758(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[758] TSRMLS_DC);
}
static int interp_handler_759(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[759] TSRMLS_DC);
}
static int interp_handler_760(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[760] TSRMLS_DC);
}
static int interp_handler_761(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[761] TSRMLS_DC);
}
static int interp_handler_762(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[762] TSRMLS_DC);
}
static int interp_handler_763(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[763] TSRMLS_DC);
}
static int interp_handler_764(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[764] TSRMLS_DC);
}
static int interp_handler_765(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[765] TSRMLS_DC);
}
static int interp_handler_766(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[766] TSRMLS_DC);
}
static int interp_handler_767(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[767] TSRMLS_DC);
}
static int interp_handler_768(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[768] TSRMLS_DC);
}
static int interp_handler_769(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[769] TSRMLS_DC);
}
static int interp_handler_770(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[770] TSRMLS_DC);
}
static int interp_handler_771(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[771] TSRMLS_DC);
}
static int interp_handler_772(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[772] TSRMLS_DC);
}
static int interp_handler_773(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[773] TSRMLS_DC);
}
static int interp_handler_774(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[774] TSRMLS_DC);
}
static int interp_handler_775(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[775] TSRMLS_DC);
}
static int interp_handler_776(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[776] TSRMLS_DC);
}
static int interp_handler_777(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[777] TSRMLS_DC);
}
static int interp_handler_778(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[778] TSRMLS_DC);
}
static int interp_handler_779(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[779] TSRMLS_DC);
}
static int interp_handler_780(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[780] TSRMLS_DC);
}
static int interp_handler_781(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[781] TSRMLS_DC);
}
static int interp_handler_782(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[782] TSRMLS_DC);
}
static int interp_handler_783(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[783] TSRMLS_DC);
}
static int interp_handler_784(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[784] TSRMLS_DC);
}
static int interp_handler_785(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[785] TSRMLS_DC);
}
static int interp_handler_786(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[786] TSRMLS_DC);
}
static int interp_handler_787(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[787] TSRMLS_DC);
}
static int interp_handler_788(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[788] TSRMLS_DC);
}
static int interp_handler_789(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[789] TSRMLS_DC);
}
static int interp_handler_790(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[790] TSRMLS_DC);
}
static int interp_handler_791(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[791] TSRMLS_DC);
}
static int interp_handler_792(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[792] TSRMLS_DC);
}
static int interp_handler_793(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[793] TSRMLS_DC);
}
static int interp_handler_794(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[794] TSRMLS_DC);
}
static int interp_handler_795(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[795] TSRMLS_DC);
}
static int interp_handler_796(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[796] TSRMLS_DC);
}
static int interp_handler_797(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[797] TSRMLS_DC);
}
static int interp_handler_798(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[798] TSRMLS_DC);
}
static int interp_handler_799(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[799] TSRMLS_DC);
}
static int interp_handler_800(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[800] TSRMLS_DC);
}
static int interp_handler_801(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[801] TSRMLS_DC);
}
static int interp_handler_802(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[802] TSRMLS_DC);
}
static int interp_handler_803(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[803] TSRMLS_DC);
}
static int interp_handler_804(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[804] TSRMLS_DC);
}
static int interp_handler_805(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[805] TSRMLS_DC);
}
static int interp_handler_806(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[806] TSRMLS_DC);
}
static int interp_handler_807(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[807] TSRMLS_DC);
}
static int interp_handler_808(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[808] TSRMLS_DC);
}
static int interp_handler_809(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[809] TSRMLS_DC);
}
static int interp_handler_810(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[810] TSRMLS_DC);
}
static int interp_handler_811(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[811] TSRMLS_DC);
}
static int interp_handler_812(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[812] TSRMLS_DC);
}
static int interp_handler_813(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[813] TSRMLS_DC);
}
static int interp_handler_814(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[814] TSRMLS_DC);
}
static int interp_handler_815(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[815] TSRMLS_DC);
}
static int interp_handler_816(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[816] TSRMLS_DC);
}
static int interp_handler_817(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[817] TSRMLS_DC);
}
static int interp_handler_818(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[818] TSRMLS_DC);
}
static int interp_handler_819(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[819] TSRMLS_DC);
}
static int interp_handler_820(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[820] TSRMLS_DC);
}
static int interp_handler_821(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[821] TSRMLS_DC);
}
static int interp_handler_822(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[822] TSRMLS_DC);
}
static int interp_handler_823(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[823] TSRMLS_DC);
}
static int interp_handler_824(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[824] TSRMLS_DC);
}
static int interp_handler_825(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[825] TSRMLS_DC);
}
static int interp_handler_826(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[826] TSRMLS_DC);
}
static int interp_handler_827(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[827] TSRMLS_DC);
}
static int interp_handler_828(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[828] TSRMLS_DC);
}
static int interp_handler_829(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[829] TSRMLS_DC);
}
static int interp_handler_830(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[830] TSRMLS_DC);
}
static int interp_handler_831(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[831] TSRMLS_DC);
}
static int interp_handler_832(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[832] TSRMLS_DC);
}
static int interp_handler_833(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[833] TSRMLS_DC);
}
static int interp_handler_834(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[834] TSRMLS_DC);
}
static int interp_handler_835(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[835] TSRMLS_DC);
}
static int interp_handler_836(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[836] TSRMLS_DC);
}
static int interp_handler_837(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[837] TSRMLS_DC);
}
static int interp_handler_838(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[838] TSRMLS_DC);
}
static int interp_handler_839(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[839] TSRMLS_DC);
}
static int interp_handler_840(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[840] TSRMLS_DC);
}
static int interp_handler_841(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[841] TSRMLS_DC);
}
static int interp_handler_842(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[842] TSRMLS_DC);
}
static int interp_handler_843(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[843] TSRMLS_DC);
}
static int interp_handler_844(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[844] TSRMLS_DC);
}
static int interp_handler_845(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[845] TSRMLS_DC);
}
static int interp_handler_846(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[846] TSRMLS_DC);
}
static int interp_handler_847(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[847] TSRMLS_DC);
}
static int interp_handler_848(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[848] TSRMLS_DC);
}
static int interp_handler_849(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[849] TSRMLS_DC);
}
static int interp_handler_850(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[850] TSRMLS_DC);
}
static int interp_handler_851(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[851] TSRMLS_DC);
}
static int interp_handler_852(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[852] TSRMLS_DC);
}
static int interp_handler_853(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[853] TSRMLS_DC);
}
static int interp_handler_854(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[854] TSRMLS_DC);
}
static int interp_handler_855(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[855] TSRMLS_DC);
}
static int interp_handler_856(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[856] TSRMLS_DC);
}
static int interp_handler_857(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[857] TSRMLS_DC);
}
static int interp_handler_858(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[858] TSRMLS_DC);
}
static int interp_handler_859(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[859] TSRMLS_DC);
}
static int interp_handler_860(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[860] TSRMLS_DC);
}
static int interp_handler_861(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[861] TSRMLS_DC);
}
static int interp_handler_862(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[862] TSRMLS_DC);
}
static int interp_handler_863(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[863] TSRMLS_DC);
}
static int interp_handler_864(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[864] TSRMLS_DC);
}
static int interp_handler_865(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[865] TSRMLS_DC);
}
static int interp_handler_866(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[866] TSRMLS_DC);
}
static int interp_handler_867(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[867] TSRMLS_DC);
}
static int interp_handler_868(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[868] TSRMLS_DC);
}
static int interp_handler_869(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[869] TSRMLS_DC);
}
static int interp_handler_870(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[870] TSRMLS_DC);
}
static int interp_handler_871(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[871] TSRMLS_DC);
}
static int interp_handler_872(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[872] TSRMLS_DC);
}
static int interp_handler_873(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[873] TSRMLS_DC);
}
static int interp_handler_874(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[874] TSRMLS_DC);
}
static int interp_handler_875(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[875] TSRMLS_DC);
}
static int interp_handler_876(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[876] TSRMLS_DC);
}
static int interp_handler_877(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[877] TSRMLS_DC);
}
static int interp_handler_878(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[878] TSRMLS_DC);
}
static int interp_handler_879(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[879] TSRMLS_DC);
}
static int interp_handler_880(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[880] TSRMLS_DC);
}
static int interp_handler_881(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[881] TSRMLS_DC);
}
static int interp_handler_882(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[882] TSRMLS_DC);
}
static int interp_handler_883(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[883] TSRMLS_DC);
}
static int interp_handler_884(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[884] TSRMLS_DC);
}
static int interp_handler_885(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[885] TSRMLS_DC);
}
static int interp_handler_886(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[886] TSRMLS_DC);
}
static int interp_handler_887(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[887] TSRMLS_DC);
}
static int interp_handler_888(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[888] TSRMLS_DC);
}
static int interp_handler_889(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[889] TSRMLS_DC);
}
static int interp_handler_890(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[890] TSRMLS_DC);
}
static int interp_handler_891(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[891] TSRMLS_DC);
}
static int interp_handler_892(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[892] TSRMLS_DC);
}
static int interp_handler_893(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[893] TSRMLS_DC);
}
static int interp_handler_894(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[894] TSRMLS_DC);
}
static int interp_handler_895(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[895] TSRMLS_DC);
}
static int interp_handler_896(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[896] TSRMLS_DC);
}
static int interp_handler_897(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[897] TSRMLS_DC);
}
static int interp_handler_898(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[898] TSRMLS_DC);
}
static int interp_handler_899(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[899] TSRMLS_DC);
}
static int interp_handler_900(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[900] TSRMLS_DC);
}
static int interp_handler_901(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[901] TSRMLS_DC);
}
static int interp_handler_902(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[902] TSRMLS_DC);
}
static int interp_handler_903(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[903] TSRMLS_DC);
}
static int interp_handler_904(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[904] TSRMLS_DC);
}
static int interp_handler_905(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[905] TSRMLS_DC);
}
static int interp_handler_906(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[906] TSRMLS_DC);
}
static int interp_handler_907(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[907] TSRMLS_DC);
}
static int interp_handler_908(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[908] TSRMLS_DC);
}
static int interp_handler_909(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[909] TSRMLS_DC);
}
static int interp_handler_910(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[910] TSRMLS_DC);
}
static int interp_handler_911(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[911] TSRMLS_DC);
}
static int interp_handler_912(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[912] TSRMLS_DC);
}
static int interp_handler_913(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[913] TSRMLS_DC);
}
static int interp_handler_914(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[914] TSRMLS_DC);
}
static int interp_handler_915(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[915] TSRMLS_DC);
}
static int interp_handler_916(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[916] TSRMLS_DC);
}
static int interp_handler_917(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[917] TSRMLS_DC);
}
static int interp_handler_918(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[918] TSRMLS_DC);
}
static int interp_handler_919(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[919] TSRMLS_DC);
}
static int interp_handler_920(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[920] TSRMLS_DC);
}
static int interp_handler_921(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[921] TSRMLS_DC);
}
static int interp_handler_922(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[922] TSRMLS_DC);
}
static int interp_handler_923(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[923] TSRMLS_DC);
}
static int interp_handler_924(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[924] TSRMLS_DC);
}
static int interp_handler_925(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[925] TSRMLS_DC);
}
static int interp_handler_926(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[926] TSRMLS_DC);
}
static int interp_handler_927(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[927] TSRMLS_DC);
}
static int interp_handler_928(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[928] TSRMLS_DC);
}
static int interp_handler_929(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[929] TSRMLS_DC);
}
static int interp_handler_930(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[930] TSRMLS_DC);
}
static int interp_handler_931(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[931] TSRMLS_DC);
}
static int interp_handler_932(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[932] TSRMLS_DC);
}
static int interp_handler_933(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[933] TSRMLS_DC);
}
static int interp_handler_934(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[934] TSRMLS_DC);
}
static int interp_handler_935(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[935] TSRMLS_DC);
}
static int interp_handler_936(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[936] TSRMLS_DC);
}
static int interp_handler_937(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[937] TSRMLS_DC);
}
static int interp_handler_938(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[938] TSRMLS_DC);
}
static int interp_handler_939(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[939] TSRMLS_DC);
}
static int interp_handler_940(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[940] TSRMLS_DC);
}
static int interp_handler_941(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[941] TSRMLS_DC);
}
static int interp_handler_942(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[942] TSRMLS_DC);
}
static int interp_handler_943(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[943] TSRMLS_DC);
}
static int interp_handler_944(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[944] TSRMLS_DC);
}
static int interp_handler_945(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[945] TSRMLS_DC);
}
static int interp_handler_946(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[946] TSRMLS_DC);
}
static int interp_handler_947(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[947] TSRMLS_DC);
}
static int interp_handler_948(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[948] TSRMLS_DC);
}
static int interp_handler_949(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[949] TSRMLS_DC);
}
static int interp_handler_950(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[950] TSRMLS_DC);
}
static int interp_handler_951(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[951] TSRMLS_DC);
}
static int interp_handler_952(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[952] TSRMLS_DC);
}
static int interp_handler_953(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[953] TSRMLS_DC);
}
static int interp_handler_954(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[954] TSRMLS_DC);
}
static int interp_handler_955(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[955] TSRMLS_DC);
}
static int interp_handler_956(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[956] TSRMLS_DC);
}
static int interp_handler_957(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[957] TSRMLS_DC);
}
static int interp_handler_958(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[958] TSRMLS_DC);
}
static int interp_handler_959(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[959] TSRMLS_DC);
}
static int interp_handler_960(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[960] TSRMLS_DC);
}
static int interp_handler_961(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[961] TSRMLS_DC);
}
static int interp_handler_962(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[962] TSRMLS_DC);
}
static int interp_handler_963(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[963] TSRMLS_DC);
}
static int interp_handler_964(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[964] TSRMLS_DC);
}
static int interp_handler_965(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[965] TSRMLS_DC);
}
static int interp_handler_966(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[966] TSRMLS_DC);
}
static int interp_handler_967(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[967] TSRMLS_DC);
}
static int interp_handler_968(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[968] TSRMLS_DC);
}
static int interp_handler_969(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[969] TSRMLS_DC);
}
static int interp_handler_970(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[970] TSRMLS_DC);
}
static int interp_handler_971(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[971] TSRMLS_DC);
}
static int interp_handler_972(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[972] TSRMLS_DC);
}
static int interp_handler_973(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[973] TSRMLS_DC);
}
static int interp_handler_974(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[974] TSRMLS_DC);
}
static int interp_handler_975(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[975] TSRMLS_DC);
}
static int interp_handler_976(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[976] TSRMLS_DC);
}
static int interp_handler_977(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[977] TSRMLS_DC);
}
static int interp_handler_978(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[978] TSRMLS_DC);
}
static int interp_handler_979(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[979] TSRMLS_DC);
}
static int interp_handler_980(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[980] TSRMLS_DC);
}
static int interp_handler_981(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[981] TSRMLS_DC);
}
static int interp_handler_982(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[982] TSRMLS_DC);
}
static int interp_handler_983(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[983] TSRMLS_DC);
}
static int interp_handler_984(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[984] TSRMLS_DC);
}
static int interp_handler_985(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[985] TSRMLS_DC);
}
static int interp_handler_986(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[986] TSRMLS_DC);
}
static int interp_handler_987(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[987] TSRMLS_DC);
}
static int interp_handler_988(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[988] TSRMLS_DC);
}
static int interp_handler_989(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[989] TSRMLS_DC);
}
static int interp_handler_990(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[990] TSRMLS_DC);
}
static int interp_handler_991(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[991] TSRMLS_DC);
}
static int interp_handler_992(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[992] TSRMLS_DC);
}
static int interp_handler_993(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[993] TSRMLS_DC);
}
static int interp_handler_994(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[994] TSRMLS_DC);
}
static int interp_handler_995(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[995] TSRMLS_DC);
}
static int interp_handler_996(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[996] TSRMLS_DC);
}
static int interp_handler_997(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[997] TSRMLS_DC);
}
static int interp_handler_998(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[998] TSRMLS_DC);
}
static int interp_handler_999(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[999] TSRMLS_DC);
}
static int interp_handler_1000(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1000] TSRMLS_DC);
}
static int interp_handler_1001(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1001] TSRMLS_DC);
}
static int interp_handler_1002(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1002] TSRMLS_DC);
}
static int interp_handler_1003(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1003] TSRMLS_DC);
}
static int interp_handler_1004(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1004] TSRMLS_DC);
}
static int interp_handler_1005(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1005] TSRMLS_DC);
}
static int interp_handler_1006(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1006] TSRMLS_DC);
}
static int interp_handler_1007(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1007] TSRMLS_DC);
}
static int interp_handler_1008(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1008] TSRMLS_DC);
}
static int interp_handler_1009(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1009] TSRMLS_DC);
}
static int interp_handler_1010(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1010] TSRMLS_DC);
}
static int interp_handler_1011(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1011] TSRMLS_DC);
}
static int interp_handler_1012(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1012] TSRMLS_DC);
}
static int interp_handler_1013(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1013] TSRMLS_DC);
}
static int interp_handler_1014(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1014] TSRMLS_DC);
}
static int interp_handler_1015(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1015] TSRMLS_DC);
}
static int interp_handler_1016(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1016] TSRMLS_DC);
}
static int interp_handler_1017(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1017] TSRMLS_DC);
}
static int interp_handler_1018(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1018] TSRMLS_DC);
}
static int interp_handler_1019(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1019] TSRMLS_DC);
}
static int interp_handler_1020(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1020] TSRMLS_DC);
}
static int interp_handler_1021(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1021] TSRMLS_DC);
}
static int interp_handler_1022(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1022] TSRMLS_DC);
}
static int interp_handler_1023(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1023] TSRMLS_DC);
}
static int interp_handler_1024(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1024] TSRMLS_DC);
}
static int interp_handler_1025(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1025] TSRMLS_DC);
}
static int interp_handler_1026(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1026] TSRMLS_DC);
}
static int interp_handler_1027(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1027] TSRMLS_DC);
}
static int interp_handler_1028(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1028] TSRMLS_DC);
}
static int interp_handler_1029(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1029] TSRMLS_DC);
}
static int interp_handler_1030(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1030] TSRMLS_DC);
}
static int interp_handler_1031(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1031] TSRMLS_DC);
}
static int interp_handler_1032(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1032] TSRMLS_DC);
}
static int interp_handler_1033(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1033] TSRMLS_DC);
}
static int interp_handler_1034(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1034] TSRMLS_DC);
}
static int interp_handler_1035(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1035] TSRMLS_DC);
}
static int interp_handler_1036(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1036] TSRMLS_DC);
}
static int interp_handler_1037(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1037] TSRMLS_DC);
}
static int interp_handler_1038(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1038] TSRMLS_DC);
}
static int interp_handler_1039(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1039] TSRMLS_DC);
}
static int interp_handler_1040(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1040] TSRMLS_DC);
}
static int interp_handler_1041(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1041] TSRMLS_DC);
}
static int interp_handler_1042(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1042] TSRMLS_DC);
}
static int interp_handler_1043(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1043] TSRMLS_DC);
}
static int interp_handler_1044(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1044] TSRMLS_DC);
}
static int interp_handler_1045(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1045] TSRMLS_DC);
}
static int interp_handler_1046(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1046] TSRMLS_DC);
}
static int interp_handler_1047(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1047] TSRMLS_DC);
}
static int interp_handler_1048(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1048] TSRMLS_DC);
}
static int interp_handler_1049(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1049] TSRMLS_DC);
}
static int interp_handler_1050(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1050] TSRMLS_DC);
}
static int interp_handler_1051(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1051] TSRMLS_DC);
}
static int interp_handler_1052(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1052] TSRMLS_DC);
}
static int interp_handler_1053(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1053] TSRMLS_DC);
}
static int interp_handler_1054(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1054] TSRMLS_DC);
}
static int interp_handler_1055(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1055] TSRMLS_DC);
}
static int interp_handler_1056(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1056] TSRMLS_DC);
}
static int interp_handler_1057(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1057] TSRMLS_DC);
}
static int interp_handler_1058(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1058] TSRMLS_DC);
}
static int interp_handler_1059(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1059] TSRMLS_DC);
}
static int interp_handler_1060(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1060] TSRMLS_DC);
}
static int interp_handler_1061(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1061] TSRMLS_DC);
}
static int interp_handler_1062(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1062] TSRMLS_DC);
}
static int interp_handler_1063(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1063] TSRMLS_DC);
}
static int interp_handler_1064(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1064] TSRMLS_DC);
}
static int interp_handler_1065(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1065] TSRMLS_DC);
}
static int interp_handler_1066(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1066] TSRMLS_DC);
}
static int interp_handler_1067(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1067] TSRMLS_DC);
}
static int interp_handler_1068(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1068] TSRMLS_DC);
}
static int interp_handler_1069(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1069] TSRMLS_DC);
}
static int interp_handler_1070(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1070] TSRMLS_DC);
}
static int interp_handler_1071(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1071] TSRMLS_DC);
}
static int interp_handler_1072(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1072] TSRMLS_DC);
}
static int interp_handler_1073(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1073] TSRMLS_DC);
}
static int interp_handler_1074(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1074] TSRMLS_DC);
}
static int interp_handler_1075(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1075] TSRMLS_DC);
}
static int interp_handler_1076(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1076] TSRMLS_DC);
}
static int interp_handler_1077(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1077] TSRMLS_DC);
}
static int interp_handler_1078(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1078] TSRMLS_DC);
}
static int interp_handler_1079(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1079] TSRMLS_DC);
}
static int interp_handler_1080(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1080] TSRMLS_DC);
}
static int interp_handler_1081(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1081] TSRMLS_DC);
}
static int interp_handler_1082(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1082] TSRMLS_DC);
}
static int interp_handler_1083(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1083] TSRMLS_DC);
}
static int interp_handler_1084(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1084] TSRMLS_DC);
}
static int interp_handler_1085(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1085] TSRMLS_DC);
}
static int interp_handler_1086(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1086] TSRMLS_DC);
}
static int interp_handler_1087(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1087] TSRMLS_DC);
}
static int interp_handler_1088(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1088] TSRMLS_DC);
}
static int interp_handler_1089(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1089] TSRMLS_DC);
}
static int interp_handler_1090(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1090] TSRMLS_DC);
}
static int interp_handler_1091(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1091] TSRMLS_DC);
}
static int interp_handler_1092(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1092] TSRMLS_DC);
}
static int interp_handler_1093(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1093] TSRMLS_DC);
}
static int interp_handler_1094(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1094] TSRMLS_DC);
}
static int interp_handler_1095(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1095] TSRMLS_DC);
}
static int interp_handler_1096(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1096] TSRMLS_DC);
}
static int interp_handler_1097(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1097] TSRMLS_DC);
}
static int interp_handler_1098(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1098] TSRMLS_DC);
}
static int interp_handler_1099(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1099] TSRMLS_DC);
}
static int interp_handler_1100(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1100] TSRMLS_DC);
}
static int interp_handler_1101(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1101] TSRMLS_DC);
}
static int interp_handler_1102(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1102] TSRMLS_DC);
}
static int interp_handler_1103(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1103] TSRMLS_DC);
}
static int interp_handler_1104(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1104] TSRMLS_DC);
}
static int interp_handler_1105(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1105] TSRMLS_DC);
}
static int interp_handler_1106(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1106] TSRMLS_DC);
}
static int interp_handler_1107(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1107] TSRMLS_DC);
}
static int interp_handler_1108(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1108] TSRMLS_DC);
}
static int interp_handler_1109(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1109] TSRMLS_DC);
}
static int interp_handler_1110(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1110] TSRMLS_DC);
}
static int interp_handler_1111(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1111] TSRMLS_DC);
}
static int interp_handler_1112(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1112] TSRMLS_DC);
}
static int interp_handler_1113(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1113] TSRMLS_DC);
}
static int interp_handler_1114(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1114] TSRMLS_DC);
}
static int interp_handler_1115(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1115] TSRMLS_DC);
}
static int interp_handler_1116(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1116] TSRMLS_DC);
}
static int interp_handler_1117(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1117] TSRMLS_DC);
}
static int interp_handler_1118(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1118] TSRMLS_DC);
}
static int interp_handler_1119(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1119] TSRMLS_DC);
}
static int interp_handler_1120(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1120] TSRMLS_DC);
}
static int interp_handler_1121(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1121] TSRMLS_DC);
}
static int interp_handler_1122(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1122] TSRMLS_DC);
}
static int interp_handler_1123(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1123] TSRMLS_DC);
}
static int interp_handler_1124(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1124] TSRMLS_DC);
}
static int interp_handler_1125(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1125] TSRMLS_DC);
}
static int interp_handler_1126(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1126] TSRMLS_DC);
}
static int interp_handler_1127(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1127] TSRMLS_DC);
}
static int interp_handler_1128(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1128] TSRMLS_DC);
}
static int interp_handler_1129(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1129] TSRMLS_DC);
}
static int interp_handler_1130(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1130] TSRMLS_DC);
}
static int interp_handler_1131(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1131] TSRMLS_DC);
}
static int interp_handler_1132(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1132] TSRMLS_DC);
}
static int interp_handler_1133(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1133] TSRMLS_DC);
}
static int interp_handler_1134(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1134] TSRMLS_DC);
}
static int interp_handler_1135(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1135] TSRMLS_DC);
}
static int interp_handler_1136(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1136] TSRMLS_DC);
}
static int interp_handler_1137(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1137] TSRMLS_DC);
}
static int interp_handler_1138(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1138] TSRMLS_DC);
}
static int interp_handler_1139(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1139] TSRMLS_DC);
}
static int interp_handler_1140(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1140] TSRMLS_DC);
}
static int interp_handler_1141(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1141] TSRMLS_DC);
}
static int interp_handler_1142(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1142] TSRMLS_DC);
}
static int interp_handler_1143(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1143] TSRMLS_DC);
}
static int interp_handler_1144(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1144] TSRMLS_DC);
}
static int interp_handler_1145(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1145] TSRMLS_DC);
}
static int interp_handler_1146(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1146] TSRMLS_DC);
}
static int interp_handler_1147(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1147] TSRMLS_DC);
}
static int interp_handler_1148(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1148] TSRMLS_DC);
}
static int interp_handler_1149(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1149] TSRMLS_DC);
}
static int interp_handler_1150(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1150] TSRMLS_DC);
}
static int interp_handler_1151(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1151] TSRMLS_DC);
}
static int interp_handler_1152(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1152] TSRMLS_DC);
}
static int interp_handler_1153(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1153] TSRMLS_DC);
}
static int interp_handler_1154(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1154] TSRMLS_DC);
}
static int interp_handler_1155(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1155] TSRMLS_DC);
}
static int interp_handler_1156(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1156] TSRMLS_DC);
}
static int interp_handler_1157(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1157] TSRMLS_DC);
}
static int interp_handler_1158(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1158] TSRMLS_DC);
}
static int interp_handler_1159(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1159] TSRMLS_DC);
}
static int interp_handler_1160(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1160] TSRMLS_DC);
}
static int interp_handler_1161(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1161] TSRMLS_DC);
}
static int interp_handler_1162(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1162] TSRMLS_DC);
}
static int interp_handler_1163(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1163] TSRMLS_DC);
}
static int interp_handler_1164(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1164] TSRMLS_DC);
}
static int interp_handler_1165(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1165] TSRMLS_DC);
}
static int interp_handler_1166(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1166] TSRMLS_DC);
}
static int interp_handler_1167(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1167] TSRMLS_DC);
}
static int interp_handler_1168(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1168] TSRMLS_DC);
}
static int interp_handler_1169(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1169] TSRMLS_DC);
}
static int interp_handler_1170(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1170] TSRMLS_DC);
}
static int interp_handler_1171(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1171] TSRMLS_DC);
}
static int interp_handler_1172(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1172] TSRMLS_DC);
}
static int interp_handler_1173(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1173] TSRMLS_DC);
}
static int interp_handler_1174(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1174] TSRMLS_DC);
}
static int interp_handler_1175(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1175] TSRMLS_DC);
}
static int interp_handler_1176(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1176] TSRMLS_DC);
}
static int interp_handler_1177(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1177] TSRMLS_DC);
}
static int interp_handler_1178(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1178] TSRMLS_DC);
}
static int interp_handler_1179(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1179] TSRMLS_DC);
}
static int interp_handler_1180(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1180] TSRMLS_DC);
}
static int interp_handler_1181(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1181] TSRMLS_DC);
}
static int interp_handler_1182(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1182] TSRMLS_DC);
}
static int interp_handler_1183(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1183] TSRMLS_DC);
}
static int interp_handler_1184(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1184] TSRMLS_DC);
}
static int interp_handler_1185(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1185] TSRMLS_DC);
}
static int interp_handler_1186(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1186] TSRMLS_DC);
}
static int interp_handler_1187(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1187] TSRMLS_DC);
}
static int interp_handler_1188(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1188] TSRMLS_DC);
}
static int interp_handler_1189(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1189] TSRMLS_DC);
}
static int interp_handler_1190(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1190] TSRMLS_DC);
}
static int interp_handler_1191(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1191] TSRMLS_DC);
}
static int interp_handler_1192(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1192] TSRMLS_DC);
}
static int interp_handler_1193(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1193] TSRMLS_DC);
}
static int interp_handler_1194(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1194] TSRMLS_DC);
}
static int interp_handler_1195(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1195] TSRMLS_DC);
}
static int interp_handler_1196(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1196] TSRMLS_DC);
}
static int interp_handler_1197(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1197] TSRMLS_DC);
}
static int interp_handler_1198(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1198] TSRMLS_DC);
}
static int interp_handler_1199(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1199] TSRMLS_DC);
}
static int interp_handler_1200(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1200] TSRMLS_DC);
}
static int interp_handler_1201(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1201] TSRMLS_DC);
}
static int interp_handler_1202(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1202] TSRMLS_DC);
}
static int interp_handler_1203(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1203] TSRMLS_DC);
}
static int interp_handler_1204(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1204] TSRMLS_DC);
}
static int interp_handler_1205(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1205] TSRMLS_DC);
}
static int interp_handler_1206(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1206] TSRMLS_DC);
}
static int interp_handler_1207(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1207] TSRMLS_DC);
}
static int interp_handler_1208(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1208] TSRMLS_DC);
}
static int interp_handler_1209(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1209] TSRMLS_DC);
}
static int interp_handler_1210(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1210] TSRMLS_DC);
}
static int interp_handler_1211(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1211] TSRMLS_DC);
}
static int interp_handler_1212(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1212] TSRMLS_DC);
}
static int interp_handler_1213(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1213] TSRMLS_DC);
}
static int interp_handler_1214(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1214] TSRMLS_DC);
}
static int interp_handler_1215(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1215] TSRMLS_DC);
}
static int interp_handler_1216(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1216] TSRMLS_DC);
}
static int interp_handler_1217(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1217] TSRMLS_DC);
}
static int interp_handler_1218(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1218] TSRMLS_DC);
}
static int interp_handler_1219(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1219] TSRMLS_DC);
}
static int interp_handler_1220(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1220] TSRMLS_DC);
}
static int interp_handler_1221(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1221] TSRMLS_DC);
}
static int interp_handler_1222(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1222] TSRMLS_DC);
}
static int interp_handler_1223(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1223] TSRMLS_DC);
}
static int interp_handler_1224(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1224] TSRMLS_DC);
}
static int interp_handler_1225(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1225] TSRMLS_DC);
}
static int interp_handler_1226(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1226] TSRMLS_DC);
}
static int interp_handler_1227(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1227] TSRMLS_DC);
}
static int interp_handler_1228(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1228] TSRMLS_DC);
}
static int interp_handler_1229(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1229] TSRMLS_DC);
}
static int interp_handler_1230(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1230] TSRMLS_DC);
}
static int interp_handler_1231(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1231] TSRMLS_DC);
}
static int interp_handler_1232(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1232] TSRMLS_DC);
}
static int interp_handler_1233(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1233] TSRMLS_DC);
}
static int interp_handler_1234(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1234] TSRMLS_DC);
}
static int interp_handler_1235(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1235] TSRMLS_DC);
}
static int interp_handler_1236(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1236] TSRMLS_DC);
}
static int interp_handler_1237(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1237] TSRMLS_DC);
}
static int interp_handler_1238(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1238] TSRMLS_DC);
}
static int interp_handler_1239(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1239] TSRMLS_DC);
}
static int interp_handler_1240(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1240] TSRMLS_DC);
}
static int interp_handler_1241(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1241] TSRMLS_DC);
}
static int interp_handler_1242(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1242] TSRMLS_DC);
}
static int interp_handler_1243(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1243] TSRMLS_DC);
}
static int interp_handler_1244(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1244] TSRMLS_DC);
}
static int interp_handler_1245(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1245] TSRMLS_DC);
}
static int interp_handler_1246(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1246] TSRMLS_DC);
}
static int interp_handler_1247(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1247] TSRMLS_DC);
}
static int interp_handler_1248(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1248] TSRMLS_DC);
}
static int interp_handler_1249(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1249] TSRMLS_DC);
}
static int interp_handler_1250(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1250] TSRMLS_DC);
}
static int interp_handler_1251(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1251] TSRMLS_DC);
}
static int interp_handler_1252(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1252] TSRMLS_DC);
}
static int interp_handler_1253(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1253] TSRMLS_DC);
}
static int interp_handler_1254(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1254] TSRMLS_DC);
}
static int interp_handler_1255(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1255] TSRMLS_DC);
}
static int interp_handler_1256(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1256] TSRMLS_DC);
}
static int interp_handler_1257(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1257] TSRMLS_DC);
}
static int interp_handler_1258(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1258] TSRMLS_DC);
}
static int interp_handler_1259(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1259] TSRMLS_DC);
}
static int interp_handler_1260(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1260] TSRMLS_DC);
}
static int interp_handler_1261(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1261] TSRMLS_DC);
}
static int interp_handler_1262(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1262] TSRMLS_DC);
}
static int interp_handler_1263(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1263] TSRMLS_DC);
}
static int interp_handler_1264(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1264] TSRMLS_DC);
}
static int interp_handler_1265(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1265] TSRMLS_DC);
}
static int interp_handler_1266(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1266] TSRMLS_DC);
}
static int interp_handler_1267(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1267] TSRMLS_DC);
}
static int interp_handler_1268(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1268] TSRMLS_DC);
}
static int interp_handler_1269(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1269] TSRMLS_DC);
}
static int interp_handler_1270(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1270] TSRMLS_DC);
}
static int interp_handler_1271(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1271] TSRMLS_DC);
}
static int interp_handler_1272(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1272] TSRMLS_DC);
}
static int interp_handler_1273(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1273] TSRMLS_DC);
}
static int interp_handler_1274(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1274] TSRMLS_DC);
}
static int interp_handler_1275(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1275] TSRMLS_DC);
}
static int interp_handler_1276(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1276] TSRMLS_DC);
}
static int interp_handler_1277(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1277] TSRMLS_DC);
}
static int interp_handler_1278(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1278] TSRMLS_DC);
}
static int interp_handler_1279(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1279] TSRMLS_DC);
}
static int interp_handler_1280(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1280] TSRMLS_DC);
}
static int interp_handler_1281(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1281] TSRMLS_DC);
}
static int interp_handler_1282(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1282] TSRMLS_DC);
}
static int interp_handler_1283(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1283] TSRMLS_DC);
}
static int interp_handler_1284(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1284] TSRMLS_DC);
}
static int interp_handler_1285(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1285] TSRMLS_DC);
}
static int interp_handler_1286(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1286] TSRMLS_DC);
}
static int interp_handler_1287(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1287] TSRMLS_DC);
}
static int interp_handler_1288(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1288] TSRMLS_DC);
}
static int interp_handler_1289(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1289] TSRMLS_DC);
}
static int interp_handler_1290(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1290] TSRMLS_DC);
}
static int interp_handler_1291(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1291] TSRMLS_DC);
}
static int interp_handler_1292(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1292] TSRMLS_DC);
}
static int interp_handler_1293(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1293] TSRMLS_DC);
}
static int interp_handler_1294(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1294] TSRMLS_DC);
}
static int interp_handler_1295(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1295] TSRMLS_DC);
}
static int interp_handler_1296(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1296] TSRMLS_DC);
}
static int interp_handler_1297(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1297] TSRMLS_DC);
}
static int interp_handler_1298(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1298] TSRMLS_DC);
}
static int interp_handler_1299(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1299] TSRMLS_DC);
}
static int interp_handler_1300(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1300] TSRMLS_DC);
}
static int interp_handler_1301(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1301] TSRMLS_DC);
}
static int interp_handler_1302(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1302] TSRMLS_DC);
}
static int interp_handler_1303(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1303] TSRMLS_DC);
}
static int interp_handler_1304(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1304] TSRMLS_DC);
}
static int interp_handler_1305(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1305] TSRMLS_DC);
}
static int interp_handler_1306(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1306] TSRMLS_DC);
}
static int interp_handler_1307(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1307] TSRMLS_DC);
}
static int interp_handler_1308(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1308] TSRMLS_DC);
}
static int interp_handler_1309(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1309] TSRMLS_DC);
}
static int interp_handler_1310(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1310] TSRMLS_DC);
}
static int interp_handler_1311(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1311] TSRMLS_DC);
}
static int interp_handler_1312(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1312] TSRMLS_DC);
}
static int interp_handler_1313(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1313] TSRMLS_DC);
}
static int interp_handler_1314(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1314] TSRMLS_DC);
}
static int interp_handler_1315(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1315] TSRMLS_DC);
}
static int interp_handler_1316(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1316] TSRMLS_DC);
}
static int interp_handler_1317(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1317] TSRMLS_DC);
}
static int interp_handler_1318(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1318] TSRMLS_DC);
}
static int interp_handler_1319(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1319] TSRMLS_DC);
}
static int interp_handler_1320(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1320] TSRMLS_DC);
}
static int interp_handler_1321(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1321] TSRMLS_DC);
}
static int interp_handler_1322(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1322] TSRMLS_DC);
}
static int interp_handler_1323(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1323] TSRMLS_DC);
}
static int interp_handler_1324(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1324] TSRMLS_DC);
}
static int interp_handler_1325(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1325] TSRMLS_DC);
}
static int interp_handler_1326(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1326] TSRMLS_DC);
}
static int interp_handler_1327(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1327] TSRMLS_DC);
}
static int interp_handler_1328(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1328] TSRMLS_DC);
}
static int interp_handler_1329(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1329] TSRMLS_DC);
}
static int interp_handler_1330(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1330] TSRMLS_DC);
}
static int interp_handler_1331(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1331] TSRMLS_DC);
}
static int interp_handler_1332(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1332] TSRMLS_DC);
}
static int interp_handler_1333(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1333] TSRMLS_DC);
}
static int interp_handler_1334(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1334] TSRMLS_DC);
}
static int interp_handler_1335(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1335] TSRMLS_DC);
}
static int interp_handler_1336(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1336] TSRMLS_DC);
}
static int interp_handler_1337(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1337] TSRMLS_DC);
}
static int interp_handler_1338(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1338] TSRMLS_DC);
}
static int interp_handler_1339(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1339] TSRMLS_DC);
}
static int interp_handler_1340(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1340] TSRMLS_DC);
}
static int interp_handler_1341(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1341] TSRMLS_DC);
}
static int interp_handler_1342(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1342] TSRMLS_DC);
}
static int interp_handler_1343(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1343] TSRMLS_DC);
}
static int interp_handler_1344(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1344] TSRMLS_DC);
}
static int interp_handler_1345(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1345] TSRMLS_DC);
}
static int interp_handler_1346(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1346] TSRMLS_DC);
}
static int interp_handler_1347(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1347] TSRMLS_DC);
}
static int interp_handler_1348(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1348] TSRMLS_DC);
}
static int interp_handler_1349(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1349] TSRMLS_DC);
}
static int interp_handler_1350(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1350] TSRMLS_DC);
}
static int interp_handler_1351(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1351] TSRMLS_DC);
}
static int interp_handler_1352(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1352] TSRMLS_DC);
}
static int interp_handler_1353(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1353] TSRMLS_DC);
}
static int interp_handler_1354(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1354] TSRMLS_DC);
}
static int interp_handler_1355(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1355] TSRMLS_DC);
}
static int interp_handler_1356(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1356] TSRMLS_DC);
}
static int interp_handler_1357(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1357] TSRMLS_DC);
}
static int interp_handler_1358(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1358] TSRMLS_DC);
}
static int interp_handler_1359(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1359] TSRMLS_DC);
}
static int interp_handler_1360(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1360] TSRMLS_DC);
}
static int interp_handler_1361(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1361] TSRMLS_DC);
}
static int interp_handler_1362(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1362] TSRMLS_DC);
}
static int interp_handler_1363(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1363] TSRMLS_DC);
}
static int interp_handler_1364(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1364] TSRMLS_DC);
}
static int interp_handler_1365(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1365] TSRMLS_DC);
}
static int interp_handler_1366(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1366] TSRMLS_DC);
}
static int interp_handler_1367(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1367] TSRMLS_DC);
}
static int interp_handler_1368(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1368] TSRMLS_DC);
}
static int interp_handler_1369(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1369] TSRMLS_DC);
}
static int interp_handler_1370(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1370] TSRMLS_DC);
}
static int interp_handler_1371(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1371] TSRMLS_DC);
}
static int interp_handler_1372(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1372] TSRMLS_DC);
}
static int interp_handler_1373(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1373] TSRMLS_DC);
}
static int interp_handler_1374(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1374] TSRMLS_DC);
}
static int interp_handler_1375(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1375] TSRMLS_DC);
}
static int interp_handler_1376(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1376] TSRMLS_DC);
}
static int interp_handler_1377(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1377] TSRMLS_DC);
}
static int interp_handler_1378(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1378] TSRMLS_DC);
}
static int interp_handler_1379(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1379] TSRMLS_DC);
}
static int interp_handler_1380(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1380] TSRMLS_DC);
}
static int interp_handler_1381(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1381] TSRMLS_DC);
}
static int interp_handler_1382(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1382] TSRMLS_DC);
}
static int interp_handler_1383(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1383] TSRMLS_DC);
}
static int interp_handler_1384(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1384] TSRMLS_DC);
}
static int interp_handler_1385(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1385] TSRMLS_DC);
}
static int interp_handler_1386(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1386] TSRMLS_DC);
}
static int interp_handler_1387(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1387] TSRMLS_DC);
}
static int interp_handler_1388(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1388] TSRMLS_DC);
}
static int interp_handler_1389(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1389] TSRMLS_DC);
}
static int interp_handler_1390(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1390] TSRMLS_DC);
}
static int interp_handler_1391(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1391] TSRMLS_DC);
}
static int interp_handler_1392(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1392] TSRMLS_DC);
}
static int interp_handler_1393(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1393] TSRMLS_DC);
}
static int interp_handler_1394(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1394] TSRMLS_DC);
}
static int interp_handler_1395(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1395] TSRMLS_DC);
}
static int interp_handler_1396(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1396] TSRMLS_DC);
}
static int interp_handler_1397(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1397] TSRMLS_DC);
}
static int interp_handler_1398(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1398] TSRMLS_DC);
}
static int interp_handler_1399(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1399] TSRMLS_DC);
}
static int interp_handler_1400(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1400] TSRMLS_DC);
}
static int interp_handler_1401(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1401] TSRMLS_DC);
}
static int interp_handler_1402(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1402] TSRMLS_DC);
}
static int interp_handler_1403(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1403] TSRMLS_DC);
}
static int interp_handler_1404(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1404] TSRMLS_DC);
}
static int interp_handler_1405(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1405] TSRMLS_DC);
}
static int interp_handler_1406(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1406] TSRMLS_DC);
}
static int interp_handler_1407(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1407] TSRMLS_DC);
}
static int interp_handler_1408(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1408] TSRMLS_DC);
}
static int interp_handler_1409(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1409] TSRMLS_DC);
}
static int interp_handler_1410(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1410] TSRMLS_DC);
}
static int interp_handler_1411(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1411] TSRMLS_DC);
}
static int interp_handler_1412(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1412] TSRMLS_DC);
}
static int interp_handler_1413(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1413] TSRMLS_DC);
}
static int interp_handler_1414(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1414] TSRMLS_DC);
}
static int interp_handler_1415(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1415] TSRMLS_DC);
}
static int interp_handler_1416(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1416] TSRMLS_DC);
}
static int interp_handler_1417(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1417] TSRMLS_DC);
}
static int interp_handler_1418(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1418] TSRMLS_DC);
}
static int interp_handler_1419(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1419] TSRMLS_DC);
}
static int interp_handler_1420(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1420] TSRMLS_DC);
}
static int interp_handler_1421(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1421] TSRMLS_DC);
}
static int interp_handler_1422(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1422] TSRMLS_DC);
}
static int interp_handler_1423(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1423] TSRMLS_DC);
}
static int interp_handler_1424(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1424] TSRMLS_DC);
}
static int interp_handler_1425(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1425] TSRMLS_DC);
}
static int interp_handler_1426(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1426] TSRMLS_DC);
}
static int interp_handler_1427(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1427] TSRMLS_DC);
}
static int interp_handler_1428(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1428] TSRMLS_DC);
}
static int interp_handler_1429(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1429] TSRMLS_DC);
}
static int interp_handler_1430(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1430] TSRMLS_DC);
}
static int interp_handler_1431(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1431] TSRMLS_DC);
}
static int interp_handler_1432(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1432] TSRMLS_DC);
}
static int interp_handler_1433(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1433] TSRMLS_DC);
}
static int interp_handler_1434(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1434] TSRMLS_DC);
}
static int interp_handler_1435(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1435] TSRMLS_DC);
}
static int interp_handler_1436(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1436] TSRMLS_DC);
}
static int interp_handler_1437(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1437] TSRMLS_DC);
}
static int interp_handler_1438(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1438] TSRMLS_DC);
}
static int interp_handler_1439(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1439] TSRMLS_DC);
}
static int interp_handler_1440(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1440] TSRMLS_DC);
}
static int interp_handler_1441(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1441] TSRMLS_DC);
}
static int interp_handler_1442(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1442] TSRMLS_DC);
}
static int interp_handler_1443(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1443] TSRMLS_DC);
}
static int interp_handler_1444(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1444] TSRMLS_DC);
}
static int interp_handler_1445(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1445] TSRMLS_DC);
}
static int interp_handler_1446(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1446] TSRMLS_DC);
}
static int interp_handler_1447(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1447] TSRMLS_DC);
}
static int interp_handler_1448(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1448] TSRMLS_DC);
}
static int interp_handler_1449(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1449] TSRMLS_DC);
}
static int interp_handler_1450(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1450] TSRMLS_DC);
}
static int interp_handler_1451(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1451] TSRMLS_DC);
}
static int interp_handler_1452(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1452] TSRMLS_DC);
}
static int interp_handler_1453(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1453] TSRMLS_DC);
}
static int interp_handler_1454(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1454] TSRMLS_DC);
}
static int interp_handler_1455(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1455] TSRMLS_DC);
}
static int interp_handler_1456(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1456] TSRMLS_DC);
}
static int interp_handler_1457(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1457] TSRMLS_DC);
}
static int interp_handler_1458(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1458] TSRMLS_DC);
}
static int interp_handler_1459(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1459] TSRMLS_DC);
}
static int interp_handler_1460(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1460] TSRMLS_DC);
}
static int interp_handler_1461(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1461] TSRMLS_DC);
}
static int interp_handler_1462(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1462] TSRMLS_DC);
}
static int interp_handler_1463(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1463] TSRMLS_DC);
}
static int interp_handler_1464(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1464] TSRMLS_DC);
}
static int interp_handler_1465(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1465] TSRMLS_DC);
}
static int interp_handler_1466(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1466] TSRMLS_DC);
}
static int interp_handler_1467(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1467] TSRMLS_DC);
}
static int interp_handler_1468(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1468] TSRMLS_DC);
}
static int interp_handler_1469(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1469] TSRMLS_DC);
}
static int interp_handler_1470(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1470] TSRMLS_DC);
}
static int interp_handler_1471(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1471] TSRMLS_DC);
}
static int interp_handler_1472(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1472] TSRMLS_DC);
}
static int interp_handler_1473(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1473] TSRMLS_DC);
}
static int interp_handler_1474(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1474] TSRMLS_DC);
}
static int interp_handler_1475(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1475] TSRMLS_DC);
}
static int interp_handler_1476(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1476] TSRMLS_DC);
}
static int interp_handler_1477(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1477] TSRMLS_DC);
}
static int interp_handler_1478(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1478] TSRMLS_DC);
}
static int interp_handler_1479(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1479] TSRMLS_DC);
}
static int interp_handler_1480(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1480] TSRMLS_DC);
}
static int interp_handler_1481(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1481] TSRMLS_DC);
}
static int interp_handler_1482(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1482] TSRMLS_DC);
}
static int interp_handler_1483(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1483] TSRMLS_DC);
}
static int interp_handler_1484(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1484] TSRMLS_DC);
}
static int interp_handler_1485(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1485] TSRMLS_DC);
}
static int interp_handler_1486(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1486] TSRMLS_DC);
}
static int interp_handler_1487(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1487] TSRMLS_DC);
}
static int interp_handler_1488(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1488] TSRMLS_DC);
}
static int interp_handler_1489(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1489] TSRMLS_DC);
}
static int interp_handler_1490(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1490] TSRMLS_DC);
}
static int interp_handler_1491(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1491] TSRMLS_DC);
}
static int interp_handler_1492(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1492] TSRMLS_DC);
}
static int interp_handler_1493(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1493] TSRMLS_DC);
}
static int interp_handler_1494(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1494] TSRMLS_DC);
}
static int interp_handler_1495(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1495] TSRMLS_DC);
}
static int interp_handler_1496(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1496] TSRMLS_DC);
}
static int interp_handler_1497(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1497] TSRMLS_DC);
}
static int interp_handler_1498(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1498] TSRMLS_DC);
}
static int interp_handler_1499(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1499] TSRMLS_DC);
}
static int interp_handler_1500(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1500] TSRMLS_DC);
}
static int interp_handler_1501(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1501] TSRMLS_DC);
}
static int interp_handler_1502(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1502] TSRMLS_DC);
}
static int interp_handler_1503(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1503] TSRMLS_DC);
}
static int interp_handler_1504(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1504] TSRMLS_DC);
}
static int interp_handler_1505(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1505] TSRMLS_DC);
}
static int interp_handler_1506(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1506] TSRMLS_DC);
}
static int interp_handler_1507(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1507] TSRMLS_DC);
}
static int interp_handler_1508(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1508] TSRMLS_DC);
}
static int interp_handler_1509(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1509] TSRMLS_DC);
}
static int interp_handler_1510(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1510] TSRMLS_DC);
}
static int interp_handler_1511(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1511] TSRMLS_DC);
}
static int interp_handler_1512(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1512] TSRMLS_DC);
}
static int interp_handler_1513(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1513] TSRMLS_DC);
}
static int interp_handler_1514(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1514] TSRMLS_DC);
}
static int interp_handler_1515(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1515] TSRMLS_DC);
}
static int interp_handler_1516(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1516] TSRMLS_DC);
}
static int interp_handler_1517(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1517] TSRMLS_DC);
}
static int interp_handler_1518(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1518] TSRMLS_DC);
}
static int interp_handler_1519(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1519] TSRMLS_DC);
}
static int interp_handler_1520(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1520] TSRMLS_DC);
}
static int interp_handler_1521(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1521] TSRMLS_DC);
}
static int interp_handler_1522(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1522] TSRMLS_DC);
}
static int interp_handler_1523(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1523] TSRMLS_DC);
}
static int interp_handler_1524(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1524] TSRMLS_DC);
}
static int interp_handler_1525(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1525] TSRMLS_DC);
}
static int interp_handler_1526(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1526] TSRMLS_DC);
}
static int interp_handler_1527(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1527] TSRMLS_DC);
}
static int interp_handler_1528(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1528] TSRMLS_DC);
}
static int interp_handler_1529(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1529] TSRMLS_DC);
}
static int interp_handler_1530(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1530] TSRMLS_DC);
}
static int interp_handler_1531(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1531] TSRMLS_DC);
}
static int interp_handler_1532(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1532] TSRMLS_DC);
}
static int interp_handler_1533(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1533] TSRMLS_DC);
}
static int interp_handler_1534(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1534] TSRMLS_DC);
}
static int interp_handler_1535(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1535] TSRMLS_DC);
}
static int interp_handler_1536(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1536] TSRMLS_DC);
}
static int interp_handler_1537(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1537] TSRMLS_DC);
}
static int interp_handler_1538(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1538] TSRMLS_DC);
}
static int interp_handler_1539(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1539] TSRMLS_DC);
}
static int interp_handler_1540(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1540] TSRMLS_DC);
}
static int interp_handler_1541(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1541] TSRMLS_DC);
}
static int interp_handler_1542(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1542] TSRMLS_DC);
}
static int interp_handler_1543(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1543] TSRMLS_DC);
}
static int interp_handler_1544(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1544] TSRMLS_DC);
}
static int interp_handler_1545(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1545] TSRMLS_DC);
}
static int interp_handler_1546(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1546] TSRMLS_DC);
}
static int interp_handler_1547(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1547] TSRMLS_DC);
}
static int interp_handler_1548(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1548] TSRMLS_DC);
}
static int interp_handler_1549(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1549] TSRMLS_DC);
}
static int interp_handler_1550(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1550] TSRMLS_DC);
}
static int interp_handler_1551(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1551] TSRMLS_DC);
}
static int interp_handler_1552(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1552] TSRMLS_DC);
}
static int interp_handler_1553(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1553] TSRMLS_DC);
}
static int interp_handler_1554(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1554] TSRMLS_DC);
}
static int interp_handler_1555(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1555] TSRMLS_DC);
}
static int interp_handler_1556(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1556] TSRMLS_DC);
}
static int interp_handler_1557(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1557] TSRMLS_DC);
}
static int interp_handler_1558(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1558] TSRMLS_DC);
}
static int interp_handler_1559(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1559] TSRMLS_DC);
}
static int interp_handler_1560(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1560] TSRMLS_DC);
}
static int interp_handler_1561(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1561] TSRMLS_DC);
}
static int interp_handler_1562(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1562] TSRMLS_DC);
}
static int interp_handler_1563(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1563] TSRMLS_DC);
}
static int interp_handler_1564(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1564] TSRMLS_DC);
}
static int interp_handler_1565(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1565] TSRMLS_DC);
}
static int interp_handler_1566(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1566] TSRMLS_DC);
}
static int interp_handler_1567(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1567] TSRMLS_DC);
}
static int interp_handler_1568(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1568] TSRMLS_DC);
}
static int interp_handler_1569(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1569] TSRMLS_DC);
}
static int interp_handler_1570(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1570] TSRMLS_DC);
}
static int interp_handler_1571(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1571] TSRMLS_DC);
}
static int interp_handler_1572(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1572] TSRMLS_DC);
}
static int interp_handler_1573(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1573] TSRMLS_DC);
}
static int interp_handler_1574(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1574] TSRMLS_DC);
}
static int interp_handler_1575(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1575] TSRMLS_DC);
}
static int interp_handler_1576(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1576] TSRMLS_DC);
}
static int interp_handler_1577(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1577] TSRMLS_DC);
}
static int interp_handler_1578(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1578] TSRMLS_DC);
}
static int interp_handler_1579(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1579] TSRMLS_DC);
}
static int interp_handler_1580(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1580] TSRMLS_DC);
}
static int interp_handler_1581(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1581] TSRMLS_DC);
}
static int interp_handler_1582(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1582] TSRMLS_DC);
}
static int interp_handler_1583(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1583] TSRMLS_DC);
}
static int interp_handler_1584(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1584] TSRMLS_DC);
}
static int interp_handler_1585(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1585] TSRMLS_DC);
}
static int interp_handler_1586(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1586] TSRMLS_DC);
}
static int interp_handler_1587(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1587] TSRMLS_DC);
}
static int interp_handler_1588(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1588] TSRMLS_DC);
}
static int interp_handler_1589(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1589] TSRMLS_DC);
}
static int interp_handler_1590(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1590] TSRMLS_DC);
}
static int interp_handler_1591(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1591] TSRMLS_DC);
}
static int interp_handler_1592(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1592] TSRMLS_DC);
}
static int interp_handler_1593(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1593] TSRMLS_DC);
}
static int interp_handler_1594(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1594] TSRMLS_DC);
}
static int interp_handler_1595(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1595] TSRMLS_DC);
}
static int interp_handler_1596(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1596] TSRMLS_DC);
}
static int interp_handler_1597(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1597] TSRMLS_DC);
}
static int interp_handler_1598(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1598] TSRMLS_DC);
}
static int interp_handler_1599(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1599] TSRMLS_DC);
}
static int interp_handler_1600(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1600] TSRMLS_DC);
}
static int interp_handler_1601(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1601] TSRMLS_DC);
}
static int interp_handler_1602(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1602] TSRMLS_DC);
}
static int interp_handler_1603(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1603] TSRMLS_DC);
}
static int interp_handler_1604(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1604] TSRMLS_DC);
}
static int interp_handler_1605(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1605] TSRMLS_DC);
}
static int interp_handler_1606(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1606] TSRMLS_DC);
}
static int interp_handler_1607(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1607] TSRMLS_DC);
}
static int interp_handler_1608(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1608] TSRMLS_DC);
}
static int interp_handler_1609(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1609] TSRMLS_DC);
}
static int interp_handler_1610(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1610] TSRMLS_DC);
}
static int interp_handler_1611(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1611] TSRMLS_DC);
}
static int interp_handler_1612(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1612] TSRMLS_DC);
}
static int interp_handler_1613(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1613] TSRMLS_DC);
}
static int interp_handler_1614(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1614] TSRMLS_DC);
}
static int interp_handler_1615(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1615] TSRMLS_DC);
}
static int interp_handler_1616(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1616] TSRMLS_DC);
}
static int interp_handler_1617(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1617] TSRMLS_DC);
}
static int interp_handler_1618(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1618] TSRMLS_DC);
}
static int interp_handler_1619(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1619] TSRMLS_DC);
}
static int interp_handler_1620(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1620] TSRMLS_DC);
}
static int interp_handler_1621(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1621] TSRMLS_DC);
}
static int interp_handler_1622(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1622] TSRMLS_DC);
}
static int interp_handler_1623(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1623] TSRMLS_DC);
}
static int interp_handler_1624(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1624] TSRMLS_DC);
}
static int interp_handler_1625(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1625] TSRMLS_DC);
}
static int interp_handler_1626(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1626] TSRMLS_DC);
}
static int interp_handler_1627(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1627] TSRMLS_DC);
}
static int interp_handler_1628(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1628] TSRMLS_DC);
}
static int interp_handler_1629(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1629] TSRMLS_DC);
}
static int interp_handler_1630(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1630] TSRMLS_DC);
}
static int interp_handler_1631(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1631] TSRMLS_DC);
}
static int interp_handler_1632(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1632] TSRMLS_DC);
}
static int interp_handler_1633(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1633] TSRMLS_DC);
}
static int interp_handler_1634(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1634] TSRMLS_DC);
}
static int interp_handler_1635(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1635] TSRMLS_DC);
}
static int interp_handler_1636(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1636] TSRMLS_DC);
}
static int interp_handler_1637(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1637] TSRMLS_DC);
}
static int interp_handler_1638(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1638] TSRMLS_DC);
}
static int interp_handler_1639(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1639] TSRMLS_DC);
}
static int interp_handler_1640(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1640] TSRMLS_DC);
}
static int interp_handler_1641(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1641] TSRMLS_DC);
}
static int interp_handler_1642(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1642] TSRMLS_DC);
}
static int interp_handler_1643(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1643] TSRMLS_DC);
}
static int interp_handler_1644(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1644] TSRMLS_DC);
}
static int interp_handler_1645(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1645] TSRMLS_DC);
}
static int interp_handler_1646(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1646] TSRMLS_DC);
}
static int interp_handler_1647(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1647] TSRMLS_DC);
}
static int interp_handler_1648(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1648] TSRMLS_DC);
}
static int interp_handler_1649(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1649] TSRMLS_DC);
}
static int interp_handler_1650(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1650] TSRMLS_DC);
}
static int interp_handler_1651(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1651] TSRMLS_DC);
}
static int interp_handler_1652(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1652] TSRMLS_DC);
}
static int interp_handler_1653(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1653] TSRMLS_DC);
}
static int interp_handler_1654(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1654] TSRMLS_DC);
}
static int interp_handler_1655(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1655] TSRMLS_DC);
}
static int interp_handler_1656(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1656] TSRMLS_DC);
}
static int interp_handler_1657(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1657] TSRMLS_DC);
}
static int interp_handler_1658(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1658] TSRMLS_DC);
}
static int interp_handler_1659(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1659] TSRMLS_DC);
}
static int interp_handler_1660(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1660] TSRMLS_DC);
}
static int interp_handler_1661(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1661] TSRMLS_DC);
}
static int interp_handler_1662(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1662] TSRMLS_DC);
}
static int interp_handler_1663(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1663] TSRMLS_DC);
}
static int interp_handler_1664(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1664] TSRMLS_DC);
}
static int interp_handler_1665(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1665] TSRMLS_DC);
}
static int interp_handler_1666(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1666] TSRMLS_DC);
}
static int interp_handler_1667(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1667] TSRMLS_DC);
}
static int interp_handler_1668(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1668] TSRMLS_DC);
}
static int interp_handler_1669(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1669] TSRMLS_DC);
}
static int interp_handler_1670(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1670] TSRMLS_DC);
}
static int interp_handler_1671(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1671] TSRMLS_DC);
}
static int interp_handler_1672(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1672] TSRMLS_DC);
}
static int interp_handler_1673(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1673] TSRMLS_DC);
}
static int interp_handler_1674(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1674] TSRMLS_DC);
}
static int interp_handler_1675(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1675] TSRMLS_DC);
}
static int interp_handler_1676(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1676] TSRMLS_DC);
}
static int interp_handler_1677(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1677] TSRMLS_DC);
}
static int interp_handler_1678(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1678] TSRMLS_DC);
}
static int interp_handler_1679(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1679] TSRMLS_DC);
}
static int interp_handler_1680(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1680] TSRMLS_DC);
}
static int interp_handler_1681(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1681] TSRMLS_DC);
}
static int interp_handler_1682(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1682] TSRMLS_DC);
}
static int interp_handler_1683(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1683] TSRMLS_DC);
}
static int interp_handler_1684(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1684] TSRMLS_DC);
}
static int interp_handler_1685(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1685] TSRMLS_DC);
}
static int interp_handler_1686(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1686] TSRMLS_DC);
}
static int interp_handler_1687(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1687] TSRMLS_DC);
}
static int interp_handler_1688(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1688] TSRMLS_DC);
}
static int interp_handler_1689(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1689] TSRMLS_DC);
}
static int interp_handler_1690(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1690] TSRMLS_DC);
}
static int interp_handler_1691(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1691] TSRMLS_DC);
}
static int interp_handler_1692(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1692] TSRMLS_DC);
}
static int interp_handler_1693(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1693] TSRMLS_DC);
}
static int interp_handler_1694(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1694] TSRMLS_DC);
}
static int interp_handler_1695(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1695] TSRMLS_DC);
}
static int interp_handler_1696(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1696] TSRMLS_DC);
}
static int interp_handler_1697(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1697] TSRMLS_DC);
}
static int interp_handler_1698(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1698] TSRMLS_DC);
}
static int interp_handler_1699(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1699] TSRMLS_DC);
}
static int interp_handler_1700(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1700] TSRMLS_DC);
}
static int interp_handler_1701(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1701] TSRMLS_DC);
}
static int interp_handler_1702(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1702] TSRMLS_DC);
}
static int interp_handler_1703(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1703] TSRMLS_DC);
}
static int interp_handler_1704(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1704] TSRMLS_DC);
}
static int interp_handler_1705(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1705] TSRMLS_DC);
}
static int interp_handler_1706(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1706] TSRMLS_DC);
}
static int interp_handler_1707(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1707] TSRMLS_DC);
}
static int interp_handler_1708(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1708] TSRMLS_DC);
}
static int interp_handler_1709(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1709] TSRMLS_DC);
}
static int interp_handler_1710(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1710] TSRMLS_DC);
}
static int interp_handler_1711(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1711] TSRMLS_DC);
}
static int interp_handler_1712(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1712] TSRMLS_DC);
}
static int interp_handler_1713(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1713] TSRMLS_DC);
}
static int interp_handler_1714(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1714] TSRMLS_DC);
}
static int interp_handler_1715(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1715] TSRMLS_DC);
}
static int interp_handler_1716(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1716] TSRMLS_DC);
}
static int interp_handler_1717(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1717] TSRMLS_DC);
}
static int interp_handler_1718(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1718] TSRMLS_DC);
}
static int interp_handler_1719(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1719] TSRMLS_DC);
}
static int interp_handler_1720(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1720] TSRMLS_DC);
}
static int interp_handler_1721(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1721] TSRMLS_DC);
}
static int interp_handler_1722(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1722] TSRMLS_DC);
}
static int interp_handler_1723(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1723] TSRMLS_DC);
}
static int interp_handler_1724(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1724] TSRMLS_DC);
}
static int interp_handler_1725(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1725] TSRMLS_DC);
}
static int interp_handler_1726(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1726] TSRMLS_DC);
}
static int interp_handler_1727(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1727] TSRMLS_DC);
}
static int interp_handler_1728(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1728] TSRMLS_DC);
}
static int interp_handler_1729(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1729] TSRMLS_DC);
}
static int interp_handler_1730(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1730] TSRMLS_DC);
}
static int interp_handler_1731(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1731] TSRMLS_DC);
}
static int interp_handler_1732(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1732] TSRMLS_DC);
}
static int interp_handler_1733(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1733] TSRMLS_DC);
}
static int interp_handler_1734(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1734] TSRMLS_DC);
}
static int interp_handler_1735(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1735] TSRMLS_DC);
}
static int interp_handler_1736(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1736] TSRMLS_DC);
}
static int interp_handler_1737(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1737] TSRMLS_DC);
}
static int interp_handler_1738(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1738] TSRMLS_DC);
}
static int interp_handler_1739(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1739] TSRMLS_DC);
}
static int interp_handler_1740(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1740] TSRMLS_DC);
}
static int interp_handler_1741(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1741] TSRMLS_DC);
}
static int interp_handler_1742(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1742] TSRMLS_DC);
}
static int interp_handler_1743(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1743] TSRMLS_DC);
}
static int interp_handler_1744(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1744] TSRMLS_DC);
}
static int interp_handler_1745(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1745] TSRMLS_DC);
}
static int interp_handler_1746(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1746] TSRMLS_DC);
}
static int interp_handler_1747(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1747] TSRMLS_DC);
}
static int interp_handler_1748(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1748] TSRMLS_DC);
}
static int interp_handler_1749(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1749] TSRMLS_DC);
}
static int interp_handler_1750(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1750] TSRMLS_DC);
}
static int interp_handler_1751(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1751] TSRMLS_DC);
}
static int interp_handler_1752(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1752] TSRMLS_DC);
}
static int interp_handler_1753(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1753] TSRMLS_DC);
}
static int interp_handler_1754(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1754] TSRMLS_DC);
}
static int interp_handler_1755(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1755] TSRMLS_DC);
}
static int interp_handler_1756(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1756] TSRMLS_DC);
}
static int interp_handler_1757(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1757] TSRMLS_DC);
}
static int interp_handler_1758(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1758] TSRMLS_DC);
}
static int interp_handler_1759(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1759] TSRMLS_DC);
}
static int interp_handler_1760(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1760] TSRMLS_DC);
}
static int interp_handler_1761(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1761] TSRMLS_DC);
}
static int interp_handler_1762(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1762] TSRMLS_DC);
}
static int interp_handler_1763(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1763] TSRMLS_DC);
}
static int interp_handler_1764(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1764] TSRMLS_DC);
}
static int interp_handler_1765(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1765] TSRMLS_DC);
}
static int interp_handler_1766(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1766] TSRMLS_DC);
}
static int interp_handler_1767(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1767] TSRMLS_DC);
}
static int interp_handler_1768(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1768] TSRMLS_DC);
}
static int interp_handler_1769(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1769] TSRMLS_DC);
}
static int interp_handler_1770(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1770] TSRMLS_DC);
}
static int interp_handler_1771(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1771] TSRMLS_DC);
}
static int interp_handler_1772(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1772] TSRMLS_DC);
}
static int interp_handler_1773(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1773] TSRMLS_DC);
}
static int interp_handler_1774(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1774] TSRMLS_DC);
}
static int interp_handler_1775(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1775] TSRMLS_DC);
}
static int interp_handler_1776(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1776] TSRMLS_DC);
}
static int interp_handler_1777(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1777] TSRMLS_DC);
}
static int interp_handler_1778(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1778] TSRMLS_DC);
}
static int interp_handler_1779(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1779] TSRMLS_DC);
}
static int interp_handler_1780(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1780] TSRMLS_DC);
}
static int interp_handler_1781(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1781] TSRMLS_DC);
}
static int interp_handler_1782(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1782] TSRMLS_DC);
}
static int interp_handler_1783(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1783] TSRMLS_DC);
}
static int interp_handler_1784(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1784] TSRMLS_DC);
}
static int interp_handler_1785(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1785] TSRMLS_DC);
}
static int interp_handler_1786(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1786] TSRMLS_DC);
}
static int interp_handler_1787(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1787] TSRMLS_DC);
}
static int interp_handler_1788(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1788] TSRMLS_DC);
}
static int interp_handler_1789(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1789] TSRMLS_DC);
}
static int interp_handler_1790(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1790] TSRMLS_DC);
}
static int interp_handler_1791(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1791] TSRMLS_DC);
}
static int interp_handler_1792(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1792] TSRMLS_DC);
}
static int interp_handler_1793(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1793] TSRMLS_DC);
}
static int interp_handler_1794(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1794] TSRMLS_DC);
}
static int interp_handler_1795(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1795] TSRMLS_DC);
}
static int interp_handler_1796(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1796] TSRMLS_DC);
}
static int interp_handler_1797(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1797] TSRMLS_DC);
}
static int interp_handler_1798(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1798] TSRMLS_DC);
}
static int interp_handler_1799(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1799] TSRMLS_DC);
}
static int interp_handler_1800(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1800] TSRMLS_DC);
}
static int interp_handler_1801(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1801] TSRMLS_DC);
}
static int interp_handler_1802(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1802] TSRMLS_DC);
}
static int interp_handler_1803(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1803] TSRMLS_DC);
}
static int interp_handler_1804(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1804] TSRMLS_DC);
}
static int interp_handler_1805(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1805] TSRMLS_DC);
}
static int interp_handler_1806(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1806] TSRMLS_DC);
}
static int interp_handler_1807(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1807] TSRMLS_DC);
}
static int interp_handler_1808(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1808] TSRMLS_DC);
}
static int interp_handler_1809(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1809] TSRMLS_DC);
}
static int interp_handler_1810(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1810] TSRMLS_DC);
}
static int interp_handler_1811(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1811] TSRMLS_DC);
}
static int interp_handler_1812(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1812] TSRMLS_DC);
}
static int interp_handler_1813(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1813] TSRMLS_DC);
}
static int interp_handler_1814(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1814] TSRMLS_DC);
}
static int interp_handler_1815(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1815] TSRMLS_DC);
}
static int interp_handler_1816(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1816] TSRMLS_DC);
}
static int interp_handler_1817(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1817] TSRMLS_DC);
}
static int interp_handler_1818(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1818] TSRMLS_DC);
}
static int interp_handler_1819(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1819] TSRMLS_DC);
}
static int interp_handler_1820(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1820] TSRMLS_DC);
}
static int interp_handler_1821(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1821] TSRMLS_DC);
}
static int interp_handler_1822(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1822] TSRMLS_DC);
}
static int interp_handler_1823(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1823] TSRMLS_DC);
}
static int interp_handler_1824(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1824] TSRMLS_DC);
}
static int interp_handler_1825(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1825] TSRMLS_DC);
}
static int interp_handler_1826(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1826] TSRMLS_DC);
}
static int interp_handler_1827(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1827] TSRMLS_DC);
}
static int interp_handler_1828(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1828] TSRMLS_DC);
}
static int interp_handler_1829(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1829] TSRMLS_DC);
}
static int interp_handler_1830(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1830] TSRMLS_DC);
}
static int interp_handler_1831(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1831] TSRMLS_DC);
}
static int interp_handler_1832(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1832] TSRMLS_DC);
}
static int interp_handler_1833(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1833] TSRMLS_DC);
}
static int interp_handler_1834(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1834] TSRMLS_DC);
}
static int interp_handler_1835(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1835] TSRMLS_DC);
}
static int interp_handler_1836(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1836] TSRMLS_DC);
}
static int interp_handler_1837(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1837] TSRMLS_DC);
}
static int interp_handler_1838(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1838] TSRMLS_DC);
}
static int interp_handler_1839(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1839] TSRMLS_DC);
}
static int interp_handler_1840(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1840] TSRMLS_DC);
}
static int interp_handler_1841(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1841] TSRMLS_DC);
}
static int interp_handler_1842(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1842] TSRMLS_DC);
}
static int interp_handler_1843(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1843] TSRMLS_DC);
}
static int interp_handler_1844(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1844] TSRMLS_DC);
}
static int interp_handler_1845(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1845] TSRMLS_DC);
}
static int interp_handler_1846(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1846] TSRMLS_DC);
}
static int interp_handler_1847(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1847] TSRMLS_DC);
}
static int interp_handler_1848(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1848] TSRMLS_DC);
}
static int interp_handler_1849(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1849] TSRMLS_DC);
}
static int interp_handler_1850(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1850] TSRMLS_DC);
}
static int interp_handler_1851(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1851] TSRMLS_DC);
}
static int interp_handler_1852(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1852] TSRMLS_DC);
}
static int interp_handler_1853(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1853] TSRMLS_DC);
}
static int interp_handler_1854(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1854] TSRMLS_DC);
}
static int interp_handler_1855(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1855] TSRMLS_DC);
}
static int interp_handler_1856(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1856] TSRMLS_DC);
}
static int interp_handler_1857(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1857] TSRMLS_DC);
}
static int interp_handler_1858(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1858] TSRMLS_DC);
}
static int interp_handler_1859(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1859] TSRMLS_DC);
}
static int interp_handler_1860(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1860] TSRMLS_DC);
}
static int interp_handler_1861(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1861] TSRMLS_DC);
}
static int interp_handler_1862(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1862] TSRMLS_DC);
}
static int interp_handler_1863(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1863] TSRMLS_DC);
}
static int interp_handler_1864(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1864] TSRMLS_DC);
}
static int interp_handler_1865(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1865] TSRMLS_DC);
}
static int interp_handler_1866(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1866] TSRMLS_DC);
}
static int interp_handler_1867(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1867] TSRMLS_DC);
}
static int interp_handler_1868(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1868] TSRMLS_DC);
}
static int interp_handler_1869(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1869] TSRMLS_DC);
}
static int interp_handler_1870(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1870] TSRMLS_DC);
}
static int interp_handler_1871(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1871] TSRMLS_DC);
}
static int interp_handler_1872(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1872] TSRMLS_DC);
}
static int interp_handler_1873(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1873] TSRMLS_DC);
}
static int interp_handler_1874(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1874] TSRMLS_DC);
}
static int interp_handler_1875(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1875] TSRMLS_DC);
}
static int interp_handler_1876(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1876] TSRMLS_DC);
}
static int interp_handler_1877(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1877] TSRMLS_DC);
}
static int interp_handler_1878(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1878] TSRMLS_DC);
}
static int interp_handler_1879(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1879] TSRMLS_DC);
}
static int interp_handler_1880(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1880] TSRMLS_DC);
}
static int interp_handler_1881(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1881] TSRMLS_DC);
}
static int interp_handler_1882(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1882] TSRMLS_DC);
}
static int interp_handler_1883(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1883] TSRMLS_DC);
}
static int interp_handler_1884(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1884] TSRMLS_DC);
}
static int interp_handler_1885(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1885] TSRMLS_DC);
}
static int interp_handler_1886(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1886] TSRMLS_DC);
}
static int interp_handler_1887(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1887] TSRMLS_DC);
}
static int interp_handler_1888(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1888] TSRMLS_DC);
}
static int interp_handler_1889(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1889] TSRMLS_DC);
}
static int interp_handler_1890(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1890] TSRMLS_DC);
}
static int interp_handler_1891(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1891] TSRMLS_DC);
}
static int interp_handler_1892(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1892] TSRMLS_DC);
}
static int interp_handler_1893(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1893] TSRMLS_DC);
}
static int interp_handler_1894(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1894] TSRMLS_DC);
}
static int interp_handler_1895(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1895] TSRMLS_DC);
}
static int interp_handler_1896(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1896] TSRMLS_DC);
}
static int interp_handler_1897(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1897] TSRMLS_DC);
}
static int interp_handler_1898(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1898] TSRMLS_DC);
}
static int interp_handler_1899(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1899] TSRMLS_DC);
}
static int interp_handler_1900(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1900] TSRMLS_DC);
}
static int interp_handler_1901(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1901] TSRMLS_DC);
}
static int interp_handler_1902(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1902] TSRMLS_DC);
}
static int interp_handler_1903(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1903] TSRMLS_DC);
}
static int interp_handler_1904(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1904] TSRMLS_DC);
}
static int interp_handler_1905(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1905] TSRMLS_DC);
}
static int interp_handler_1906(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1906] TSRMLS_DC);
}
static int interp_handler_1907(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1907] TSRMLS_DC);
}
static int interp_handler_1908(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1908] TSRMLS_DC);
}
static int interp_handler_1909(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1909] TSRMLS_DC);
}
static int interp_handler_1910(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1910] TSRMLS_DC);
}
static int interp_handler_1911(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1911] TSRMLS_DC);
}
static int interp_handler_1912(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1912] TSRMLS_DC);
}
static int interp_handler_1913(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1913] TSRMLS_DC);
}
static int interp_handler_1914(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1914] TSRMLS_DC);
}
static int interp_handler_1915(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1915] TSRMLS_DC);
}
static int interp_handler_1916(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1916] TSRMLS_DC);
}
static int interp_handler_1917(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1917] TSRMLS_DC);
}
static int interp_handler_1918(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1918] TSRMLS_DC);
}
static int interp_handler_1919(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1919] TSRMLS_DC);
}
static int interp_handler_1920(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1920] TSRMLS_DC);
}
static int interp_handler_1921(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1921] TSRMLS_DC);
}
static int interp_handler_1922(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1922] TSRMLS_DC);
}
static int interp_handler_1923(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1923] TSRMLS_DC);
}
static int interp_handler_1924(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1924] TSRMLS_DC);
}
static int interp_handler_1925(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1925] TSRMLS_DC);
}
static int interp_handler_1926(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1926] TSRMLS_DC);
}
static int interp_handler_1927(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1927] TSRMLS_DC);
}
static int interp_handler_1928(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1928] TSRMLS_DC);
}
static int interp_handler_1929(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1929] TSRMLS_DC);
}
static int interp_handler_1930(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1930] TSRMLS_DC);
}
static int interp_handler_1931(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1931] TSRMLS_DC);
}
static int interp_handler_1932(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1932] TSRMLS_DC);
}
static int interp_handler_1933(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1933] TSRMLS_DC);
}
static int interp_handler_1934(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1934] TSRMLS_DC);
}
static int interp_handler_1935(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1935] TSRMLS_DC);
}
static int interp_handler_1936(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1936] TSRMLS_DC);
}
static int interp_handler_1937(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1937] TSRMLS_DC);
}
static int interp_handler_1938(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1938] TSRMLS_DC);
}
static int interp_handler_1939(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1939] TSRMLS_DC);
}
static int interp_handler_1940(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1940] TSRMLS_DC);
}
static int interp_handler_1941(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1941] TSRMLS_DC);
}
static int interp_handler_1942(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1942] TSRMLS_DC);
}
static int interp_handler_1943(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1943] TSRMLS_DC);
}
static int interp_handler_1944(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1944] TSRMLS_DC);
}
static int interp_handler_1945(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1945] TSRMLS_DC);
}
static int interp_handler_1946(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1946] TSRMLS_DC);
}
static int interp_handler_1947(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1947] TSRMLS_DC);
}
static int interp_handler_1948(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1948] TSRMLS_DC);
}
static int interp_handler_1949(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1949] TSRMLS_DC);
}
static int interp_handler_1950(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1950] TSRMLS_DC);
}
static int interp_handler_1951(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1951] TSRMLS_DC);
}
static int interp_handler_1952(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1952] TSRMLS_DC);
}
static int interp_handler_1953(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1953] TSRMLS_DC);
}
static int interp_handler_1954(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1954] TSRMLS_DC);
}
static int interp_handler_1955(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1955] TSRMLS_DC);
}
static int interp_handler_1956(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1956] TSRMLS_DC);
}
static int interp_handler_1957(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1957] TSRMLS_DC);
}
static int interp_handler_1958(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1958] TSRMLS_DC);
}
static int interp_handler_1959(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1959] TSRMLS_DC);
}
static int interp_handler_1960(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1960] TSRMLS_DC);
}
static int interp_handler_1961(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1961] TSRMLS_DC);
}
static int interp_handler_1962(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1962] TSRMLS_DC);
}
static int interp_handler_1963(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1963] TSRMLS_DC);
}
static int interp_handler_1964(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1964] TSRMLS_DC);
}
static int interp_handler_1965(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1965] TSRMLS_DC);
}
static int interp_handler_1966(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1966] TSRMLS_DC);
}
static int interp_handler_1967(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1967] TSRMLS_DC);
}
static int interp_handler_1968(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1968] TSRMLS_DC);
}
static int interp_handler_1969(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1969] TSRMLS_DC);
}
static int interp_handler_1970(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1970] TSRMLS_DC);
}
static int interp_handler_1971(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1971] TSRMLS_DC);
}
static int interp_handler_1972(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1972] TSRMLS_DC);
}
static int interp_handler_1973(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1973] TSRMLS_DC);
}
static int interp_handler_1974(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1974] TSRMLS_DC);
}
static int interp_handler_1975(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1975] TSRMLS_DC);
}
static int interp_handler_1976(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1976] TSRMLS_DC);
}
static int interp_handler_1977(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1977] TSRMLS_DC);
}
static int interp_handler_1978(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1978] TSRMLS_DC);
}
static int interp_handler_1979(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1979] TSRMLS_DC);
}
static int interp_handler_1980(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1980] TSRMLS_DC);
}
static int interp_handler_1981(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1981] TSRMLS_DC);
}
static int interp_handler_1982(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1982] TSRMLS_DC);
}
static int interp_handler_1983(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1983] TSRMLS_DC);
}
static int interp_handler_1984(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1984] TSRMLS_DC);
}
static int interp_handler_1985(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1985] TSRMLS_DC);
}
static int interp_handler_1986(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1986] TSRMLS_DC);
}
static int interp_handler_1987(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1987] TSRMLS_DC);
}
static int interp_handler_1988(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1988] TSRMLS_DC);
}
static int interp_handler_1989(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1989] TSRMLS_DC);
}
static int interp_handler_1990(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1990] TSRMLS_DC);
}
static int interp_handler_1991(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1991] TSRMLS_DC);
}
static int interp_handler_1992(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1992] TSRMLS_DC);
}
static int interp_handler_1993(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1993] TSRMLS_DC);
}
static int interp_handler_1994(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1994] TSRMLS_DC);
}
static int interp_handler_1995(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1995] TSRMLS_DC);
}
static int interp_handler_1996(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1996] TSRMLS_DC);
}
static int interp_handler_1997(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1997] TSRMLS_DC);
}
static int interp_handler_1998(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1998] TSRMLS_DC);
}
static int interp_handler_1999(ZEND_OPCODE_HANDLER_ARGS)
{
  return execute_opcode(execute_data, original_handler[1999] TSRMLS_DC);
}
static opcode_handler_t interp_handlers[] = {
  interp_handler_0,
  interp_handler_1,
  interp_handler_2,
  interp_handler_3,
  interp_handler_4,
  interp_handler_5,
  interp_handler_6,
  interp_handler_7,
  interp_handler_8,
  interp_handler_9,
  interp_handler_10,
  interp_handler_11,
  interp_handler_12,
  interp_handler_13,
  interp_handler_14,
  interp_handler_15,
  interp_handler_16,
  interp_handler_17,
  interp_handler_18,
  interp_handler_19,
  interp_handler_20,
  interp_handler_21,
  interp_handler_22,
  interp_handler_23,
  interp_handler_24,
  interp_handler_25,
  interp_handler_26,
  interp_handler_27,
  interp_handler_28,
  interp_handler_29,
  interp_handler_30,
  interp_handler_31,
  interp_handler_32,
  interp_handler_33,
  interp_handler_34,
  interp_handler_35,
  interp_handler_36,
  interp_handler_37,
  interp_handler_38,
  interp_handler_39,
  interp_handler_40,
  interp_handler_41,
  interp_handler_42,
  interp_handler_43,
  interp_handler_44,
  interp_handler_45,
  interp_handler_46,
  interp_handler_47,
  interp_handler_48,
  interp_handler_49,
  interp_handler_50,
  interp_handler_51,
  interp_handler_52,
  interp_handler_53,
  interp_handler_54,
  interp_handler_55,
  interp_handler_56,
  interp_handler_57,
  interp_handler_58,
  interp_handler_59,
  interp_handler_60,
  interp_handler_61,
  interp_handler_62,
  interp_handler_63,
  interp_handler_64,
  interp_handler_65,
  interp_handler_66,
  interp_handler_67,
  interp_handler_68,
  interp_handler_69,
  interp_handler_70,
  interp_handler_71,
  interp_handler_72,
  interp_handler_73,
  interp_handler_74,
  interp_handler_75,
  interp_handler_76,
  interp_handler_77,
  interp_handler_78,
  interp_handler_79,
  interp_handler_80,
  interp_handler_81,
  interp_handler_82,
  interp_handler_83,
  interp_handler_84,
  interp_handler_85,
  interp_handler_86,
  interp_handler_87,
  interp_handler_88,
  interp_handler_89,
  interp_handler_90,
  interp_handler_91,
  interp_handler_92,
  interp_handler_93,
  interp_handler_94,
  interp_handler_95,
  interp_handler_96,
  interp_handler_97,
  interp_handler_98,
  interp_handler_99,
  interp_handler_100,
  interp_handler_101,
  interp_handler_102,
  interp_handler_103,
  interp_handler_104,
  interp_handler_105,
  interp_handler_106,
  interp_handler_107,
  interp_handler_108,
  interp_handler_109,
  interp_handler_110,
  interp_handler_111,
  interp_handler_112,
  interp_handler_113,
  interp_handler_114,
  interp_handler_115,
  interp_handler_116,
  interp_handler_117,
  interp_handler_118,
  interp_handler_119,
  interp_handler_120,
  interp_handler_121,
  interp_handler_122,
  interp_handler_123,
  interp_handler_124,
  interp_handler_125,
  interp_handler_126,
  interp_handler_127,
  interp_handler_128,
  interp_handler_129,
  interp_handler_130,
  interp_handler_131,
  interp_handler_132,
  interp_handler_133,
  interp_handler_134,
  interp_handler_135,
  interp_handler_136,
  interp_handler_137,
  interp_handler_138,
  interp_handler_139,
  interp_handler_140,
  interp_handler_141,
  interp_handler_142,
  interp_handler_143,
  interp_handler_144,
  interp_handler_145,
  interp_handler_146,
  interp_handler_147,
  interp_handler_148,
  interp_handler_149,
  interp_handler_150,
  interp_handler_151,
  interp_handler_152,
  interp_handler_153,
  interp_handler_154,
  interp_handler_155,
  interp_handler_156,
  interp_handler_157,
  interp_handler_158,
  interp_handler_159,
  interp_handler_160,
  interp_handler_161,
  interp_handler_162,
  interp_handler_163,
  interp_handler_164,
  interp_handler_165,
  interp_handler_166,
  interp_handler_167,
  interp_handler_168,
  interp_handler_169,
  interp_handler_170,
  interp_handler_171,
  interp_handler_172,
  interp_handler_173,
  interp_handler_174,
  interp_handler_175,
  interp_handler_176,
  interp_handler_177,
  interp_handler_178,
  interp_handler_179,
  interp_handler_180,
  interp_handler_181,
  interp_handler_182,
  interp_handler_183,
  interp_handler_184,
  interp_handler_185,
  interp_handler_186,
  interp_handler_187,
  interp_handler_188,
  interp_handler_189,
  interp_handler_190,
  interp_handler_191,
  interp_handler_192,
  interp_handler_193,
  interp_handler_194,
  interp_handler_195,
  interp_handler_196,
  interp_handler_197,
  interp_handler_198,
  interp_handler_199,
  interp_handler_200,
  interp_handler_201,
  interp_handler_202,
  interp_handler_203,
  interp_handler_204,
  interp_handler_205,
  interp_handler_206,
  interp_handler_207,
  interp_handler_208,
  interp_handler_209,
  interp_handler_210,
  interp_handler_211,
  interp_handler_212,
  interp_handler_213,
  interp_handler_214,
  interp_handler_215,
  interp_handler_216,
  interp_handler_217,
  interp_handler_218,
  interp_handler_219,
  interp_handler_220,
  interp_handler_221,
  interp_handler_222,
  interp_handler_223,
  interp_handler_224,
  interp_handler_225,
  interp_handler_226,
  interp_handler_227,
  interp_handler_228,
  interp_handler_229,
  interp_handler_230,
  interp_handler_231,
  interp_handler_232,
  interp_handler_233,
  interp_handler_234,
  interp_handler_235,
  interp_handler_236,
  interp_handler_237,
  interp_handler_238,
  interp_handler_239,
  interp_handler_240,
  interp_handler_241,
  interp_handler_242,
  interp_handler_243,
  interp_handler_244,
  interp_handler_245,
  interp_handler_246,
  interp_handler_247,
  interp_handler_248,
  interp_handler_249,
  interp_handler_250,
  interp_handler_251,
  interp_handler_252,
  interp_handler_253,
  interp_handler_254,
  interp_handler_255,
  interp_handler_256,
  interp_handler_257,
  interp_handler_258,
  interp_handler_259,
  interp_handler_260,
  interp_handler_261,
  interp_handler_262,
  interp_handler_263,
  interp_handler_264,
  interp_handler_265,
  interp_handler_266,
  interp_handler_267,
  interp_handler_268,
  interp_handler_269,
  interp_handler_270,
  interp_handler_271,
  interp_handler_272,
  interp_handler_273,
  interp_handler_274,
  interp_handler_275,
  interp_handler_276,
  interp_handler_277,
  interp_handler_278,
  interp_handler_279,
  interp_handler_280,
  interp_handler_281,
  interp_handler_282,
  interp_handler_283,
  interp_handler_284,
  interp_handler_285,
  interp_handler_286,
  interp_handler_287,
  interp_handler_288,
  interp_handler_289,
  interp_handler_290,
  interp_handler_291,
  interp_handler_292,
  interp_handler_293,
  interp_handler_294,
  interp_handler_295,
  interp_handler_296,
  interp_handler_297,
  interp_handler_298,
  interp_handler_299,
  interp_handler_300,
  interp_handler_301,
  interp_handler_302,
  interp_handler_303,
  interp_handler_304,
  interp_handler_305,
  interp_handler_306,
  interp_handler_307,
  interp_handler_308,
  interp_handler_309,
  interp_handler_310,
  interp_handler_311,
  interp_handler_312,
  interp_handler_313,
  interp_handler_314,
  interp_handler_315,
  interp_handler_316,
  interp_handler_317,
  interp_handler_318,
  interp_handler_319,
  interp_handler_320,
  interp_handler_321,
  interp_handler_322,
  interp_handler_323,
  interp_handler_324,
  interp_handler_325,
  interp_handler_326,
  interp_handler_327,
  interp_handler_328,
  interp_handler_329,
  interp_handler_330,
  interp_handler_331,
  interp_handler_332,
  interp_handler_333,
  interp_handler_334,
  interp_handler_335,
  interp_handler_336,
  interp_handler_337,
  interp_handler_338,
  interp_handler_339,
  interp_handler_340,
  interp_handler_341,
  interp_handler_342,
  interp_handler_343,
  interp_handler_344,
  interp_handler_345,
  interp_handler_346,
  interp_handler_347,
  interp_handler_348,
  interp_handler_349,
  interp_handler_350,
  interp_handler_351,
  interp_handler_352,
  interp_handler_353,
  interp_handler_354,
  interp_handler_355,
  interp_handler_356,
  interp_handler_357,
  interp_handler_358,
  interp_handler_359,
  interp_handler_360,
  interp_handler_361,
  interp_handler_362,
  interp_handler_363,
  interp_handler_364,
  interp_handler_365,
  interp_handler_366,
  interp_handler_367,
  interp_handler_368,
  interp_handler_369,
  interp_handler_370,
  interp_handler_371,
  interp_handler_372,
  interp_handler_373,
  interp_handler_374,
  interp_handler_375,
  interp_handler_376,
  interp_handler_377,
  interp_handler_378,
  interp_handler_379,
  interp_handler_380,
  interp_handler_381,
  interp_handler_382,
  interp_handler_383,
  interp_handler_384,
  interp_handler_385,
  interp_handler_386,
  interp_handler_387,
  interp_handler_388,
  interp_handler_389,
  interp_handler_390,
  interp_handler_391,
  interp_handler_392,
  interp_handler_393,
  interp_handler_394,
  interp_handler_395,
  interp_handler_396,
  interp_handler_397,
  interp_handler_398,
  interp_handler_399,
  interp_handler_400,
  interp_handler_401,
  interp_handler_402,
  interp_handler_403,
  interp_handler_404,
  interp_handler_405,
  interp_handler_406,
  interp_handler_407,
  interp_handler_408,
  interp_handler_409,
  interp_handler_410,
  interp_handler_411,
  interp_handler_412,
  interp_handler_413,
  interp_handler_414,
  interp_handler_415,
  interp_handler_416,
  interp_handler_417,
  interp_handler_418,
  interp_handler_419,
  interp_handler_420,
  interp_handler_421,
  interp_handler_422,
  interp_handler_423,
  interp_handler_424,
  interp_handler_425,
  interp_handler_426,
  interp_handler_427,
  interp_handler_428,
  interp_handler_429,
  interp_handler_430,
  interp_handler_431,
  interp_handler_432,
  interp_handler_433,
  interp_handler_434,
  interp_handler_435,
  interp_handler_436,
  interp_handler_437,
  interp_handler_438,
  interp_handler_439,
  interp_handler_440,
  interp_handler_441,
  interp_handler_442,
  interp_handler_443,
  interp_handler_444,
  interp_handler_445,
  interp_handler_446,
  interp_handler_447,
  interp_handler_448,
  interp_handler_449,
  interp_handler_450,
  interp_handler_451,
  interp_handler_452,
  interp_handler_453,
  interp_handler_454,
  interp_handler_455,
  interp_handler_456,
  interp_handler_457,
  interp_handler_458,
  interp_handler_459,
  interp_handler_460,
  interp_handler_461,
  interp_handler_462,
  interp_handler_463,
  interp_handler_464,
  interp_handler_465,
  interp_handler_466,
  interp_handler_467,
  interp_handler_468,
  interp_handler_469,
  interp_handler_470,
  interp_handler_471,
  interp_handler_472,
  interp_handler_473,
  interp_handler_474,
  interp_handler_475,
  interp_handler_476,
  interp_handler_477,
  interp_handler_478,
  interp_handler_479,
  interp_handler_480,
  interp_handler_481,
  interp_handler_482,
  interp_handler_483,
  interp_handler_484,
  interp_handler_485,
  interp_handler_486,
  interp_handler_487,
  interp_handler_488,
  interp_handler_489,
  interp_handler_490,
  interp_handler_491,
  interp_handler_492,
  interp_handler_493,
  interp_handler_494,
  interp_handler_495,
  interp_handler_496,
  interp_handler_497,
  interp_handler_498,
  interp_handler_499,
  interp_handler_500,
  interp_handler_501,
  interp_handler_502,
  interp_handler_503,
  interp_handler_504,
  interp_handler_505,
  interp_handler_506,
  interp_handler_507,
  interp_handler_508,
  interp_handler_509,
  interp_handler_510,
  interp_handler_511,
  interp_handler_512,
  interp_handler_513,
  interp_handler_514,
  interp_handler_515,
  interp_handler_516,
  interp_handler_517,
  interp_handler_518,
  interp_handler_519,
  interp_handler_520,
  interp_handler_521,
  interp_handler_522,
  interp_handler_523,
  interp_handler_524,
  interp_handler_525,
  interp_handler_526,
  interp_handler_527,
  interp_handler_528,
  interp_handler_529,
  interp_handler_530,
  interp_handler_531,
  interp_handler_532,
  interp_handler_533,
  interp_handler_534,
  interp_handler_535,
  interp_handler_536,
  interp_handler_537,
  interp_handler_538,
  interp_handler_539,
  interp_handler_540,
  interp_handler_541,
  interp_handler_542,
  interp_handler_543,
  interp_handler_544,
  interp_handler_545,
  interp_handler_546,
  interp_handler_547,
  interp_handler_548,
  interp_handler_549,
  interp_handler_550,
  interp_handler_551,
  interp_handler_552,
  interp_handler_553,
  interp_handler_554,
  interp_handler_555,
  interp_handler_556,
  interp_handler_557,
  interp_handler_558,
  interp_handler_559,
  interp_handler_560,
  interp_handler_561,
  interp_handler_562,
  interp_handler_563,
  interp_handler_564,
  interp_handler_565,
  interp_handler_566,
  interp_handler_567,
  interp_handler_568,
  interp_handler_569,
  interp_handler_570,
  interp_handler_571,
  interp_handler_572,
  interp_handler_573,
  interp_handler_574,
  interp_handler_575,
  interp_handler_576,
  interp_handler_577,
  interp_handler_578,
  interp_handler_579,
  interp_handler_580,
  interp_handler_581,
  interp_handler_582,
  interp_handler_583,
  interp_handler_584,
  interp_handler_585,
  interp_handler_586,
  interp_handler_587,
  interp_handler_588,
  interp_handler_589,
  interp_handler_590,
  interp_handler_591,
  interp_handler_592,
  interp_handler_593,
  interp_handler_594,
  interp_handler_595,
  interp_handler_596,
  interp_handler_597,
  interp_handler_598,
  interp_handler_599,
  interp_handler_600,
  interp_handler_601,
  interp_handler_602,
  interp_handler_603,
  interp_handler_604,
  interp_handler_605,
  interp_handler_606,
  interp_handler_607,
  interp_handler_608,
  interp_handler_609,
  interp_handler_610,
  interp_handler_611,
  interp_handler_612,
  interp_handler_613,
  interp_handler_614,
  interp_handler_615,
  interp_handler_616,
  interp_handler_617,
  interp_handler_618,
  interp_handler_619,
  interp_handler_620,
  interp_handler_621,
  interp_handler_622,
  interp_handler_623,
  interp_handler_624,
  interp_handler_625,
  interp_handler_626,
  interp_handler_627,
  interp_handler_628,
  interp_handler_629,
  interp_handler_630,
  interp_handler_631,
  interp_handler_632,
  interp_handler_633,
  interp_handler_634,
  interp_handler_635,
  interp_handler_636,
  interp_handler_637,
  interp_handler_638,
  interp_handler_639,
  interp_handler_640,
  interp_handler_641,
  interp_handler_642,
  interp_handler_643,
  interp_handler_644,
  interp_handler_645,
  interp_handler_646,
  interp_handler_647,
  interp_handler_648,
  interp_handler_649,
  interp_handler_650,
  interp_handler_651,
  interp_handler_652,
  interp_handler_653,
  interp_handler_654,
  interp_handler_655,
  interp_handler_656,
  interp_handler_657,
  interp_handler_658,
  interp_handler_659,
  interp_handler_660,
  interp_handler_661,
  interp_handler_662,
  interp_handler_663,
  interp_handler_664,
  interp_handler_665,
  interp_handler_666,
  interp_handler_667,
  interp_handler_668,
  interp_handler_669,
  interp_handler_670,
  interp_handler_671,
  interp_handler_672,
  interp_handler_673,
  interp_handler_674,
  interp_handler_675,
  interp_handler_676,
  interp_handler_677,
  interp_handler_678,
  interp_handler_679,
  interp_handler_680,
  interp_handler_681,
  interp_handler_682,
  interp_handler_683,
  interp_handler_684,
  interp_handler_685,
  interp_handler_686,
  interp_handler_687,
  interp_handler_688,
  interp_handler_689,
  interp_handler_690,
  interp_handler_691,
  interp_handler_692,
  interp_handler_693,
  interp_handler_694,
  interp_handler_695,
  interp_handler_696,
  interp_handler_697,
  interp_handler_698,
  interp_handler_699,
  interp_handler_700,
  interp_handler_701,
  interp_handler_702,
  interp_handler_703,
  interp_handler_704,
  interp_handler_705,
  interp_handler_706,
  interp_handler_707,
  interp_handler_708,
  interp_handler_709,
  interp_handler_710,
  interp_handler_711,
  interp_handler_712,
  interp_handler_713,
  interp_handler_714,
  interp_handler_715,
  interp_handler_716,
  interp_handler_717,
  interp_handler_718,
  interp_handler_719,
  interp_handler_720,
  interp_handler_721,
  interp_handler_722,
  interp_handler_723,
  interp_handler_724,
  interp_handler_725,
  interp_handler_726,
  interp_handler_727,
  interp_handler_728,
  interp_handler_729,
  interp_handler_730,
  interp_handler_731,
  interp_handler_732,
  interp_handler_733,
  interp_handler_734,
  interp_handler_735,
  interp_handler_736,
  interp_handler_737,
  interp_handler_738,
  interp_handler_739,
  interp_handler_740,
  interp_handler_741,
  interp_handler_742,
  interp_handler_743,
  interp_handler_744,
  interp_handler_745,
  interp_handler_746,
  interp_handler_747,
  interp_handler_748,
  interp_handler_749,
  interp_handler_750,
  interp_handler_751,
  interp_handler_752,
  interp_handler_753,
  interp_handler_754,
  interp_handler_755,
  interp_handler_756,
  interp_handler_757,
  interp_handler_758,
  interp_handler_759,
  interp_handler_760,
  interp_handler_761,
  interp_handler_762,
  interp_handler_763,
  interp_handler_764,
  interp_handler_765,
  interp_handler_766,
  interp_handler_767,
  interp_handler_768,
  interp_handler_769,
  interp_handler_770,
  interp_handler_771,
  interp_handler_772,
  interp_handler_773,
  interp_handler_774,
  interp_handler_775,
  interp_handler_776,
  interp_handler_777,
  interp_handler_778,
  interp_handler_779,
  interp_handler_780,
  interp_handler_781,
  interp_handler_782,
  interp_handler_783,
  interp_handler_784,
  interp_handler_785,
  interp_handler_786,
  interp_handler_787,
  interp_handler_788,
  interp_handler_789,
  interp_handler_790,
  interp_handler_791,
  interp_handler_792,
  interp_handler_793,
  interp_handler_794,
  interp_handler_795,
  interp_handler_796,
  interp_handler_797,
  interp_handler_798,
  interp_handler_799,
  interp_handler_800,
  interp_handler_801,
  interp_handler_802,
  interp_handler_803,
  interp_handler_804,
  interp_handler_805,
  interp_handler_806,
  interp_handler_807,
  interp_handler_808,
  interp_handler_809,
  interp_handler_810,
  interp_handler_811,
  interp_handler_812,
  interp_handler_813,
  interp_handler_814,
  interp_handler_815,
  interp_handler_816,
  interp_handler_817,
  interp_handler_818,
  interp_handler_819,
  interp_handler_820,
  interp_handler_821,
  interp_handler_822,
  interp_handler_823,
  interp_handler_824,
  interp_handler_825,
  interp_handler_826,
  interp_handler_827,
  interp_handler_828,
  interp_handler_829,
  interp_handler_830,
  interp_handler_831,
  interp_handler_832,
  interp_handler_833,
  interp_handler_834,
  interp_handler_835,
  interp_handler_836,
  interp_handler_837,
  interp_handler_838,
  interp_handler_839,
  interp_handler_840,
  interp_handler_841,
  interp_handler_842,
  interp_handler_843,
  interp_handler_844,
  interp_handler_845,
  interp_handler_846,
  interp_handler_847,
  interp_handler_848,
  interp_handler_849,
  interp_handler_850,
  interp_handler_851,
  interp_handler_852,
  interp_handler_853,
  interp_handler_854,
  interp_handler_855,
  interp_handler_856,
  interp_handler_857,
  interp_handler_858,
  interp_handler_859,
  interp_handler_860,
  interp_handler_861,
  interp_handler_862,
  interp_handler_863,
  interp_handler_864,
  interp_handler_865,
  interp_handler_866,
  interp_handler_867,
  interp_handler_868,
  interp_handler_869,
  interp_handler_870,
  interp_handler_871,
  interp_handler_872,
  interp_handler_873,
  interp_handler_874,
  interp_handler_875,
  interp_handler_876,
  interp_handler_877,
  interp_handler_878,
  interp_handler_879,
  interp_handler_880,
  interp_handler_881,
  interp_handler_882,
  interp_handler_883,
  interp_handler_884,
  interp_handler_885,
  interp_handler_886,
  interp_handler_887,
  interp_handler_888,
  interp_handler_889,
  interp_handler_890,
  interp_handler_891,
  interp_handler_892,
  interp_handler_893,
  interp_handler_894,
  interp_handler_895,
  interp_handler_896,
  interp_handler_897,
  interp_handler_898,
  interp_handler_899,
  interp_handler_900,
  interp_handler_901,
  interp_handler_902,
  interp_handler_903,
  interp_handler_904,
  interp_handler_905,
  interp_handler_906,
  interp_handler_907,
  interp_handler_908,
  interp_handler_909,
  interp_handler_910,
  interp_handler_911,
  interp_handler_912,
  interp_handler_913,
  interp_handler_914,
  interp_handler_915,
  interp_handler_916,
  interp_handler_917,
  interp_handler_918,
  interp_handler_919,
  interp_handler_920,
  interp_handler_921,
  interp_handler_922,
  interp_handler_923,
  interp_handler_924,
  interp_handler_925,
  interp_handler_926,
  interp_handler_927,
  interp_handler_928,
  interp_handler_929,
  interp_handler_930,
  interp_handler_931,
  interp_handler_932,
  interp_handler_933,
  interp_handler_934,
  interp_handler_935,
  interp_handler_936,
  interp_handler_937,
  interp_handler_938,
  interp_handler_939,
  interp_handler_940,
  interp_handler_941,
  interp_handler_942,
  interp_handler_943,
  interp_handler_944,
  interp_handler_945,
  interp_handler_946,
  interp_handler_947,
  interp_handler_948,
  interp_handler_949,
  interp_handler_950,
  interp_handler_951,
  interp_handler_952,
  interp_handler_953,
  interp_handler_954,
  interp_handler_955,
  interp_handler_956,
  interp_handler_957,
  interp_handler_958,
  interp_handler_959,
  interp_handler_960,
  interp_handler_961,
  interp_handler_962,
  interp_handler_963,
  interp_handler_964,
  interp_handler_965,
  interp_handler_966,
  interp_handler_967,
  interp_handler_968,
  interp_handler_969,
  interp_handler_970,
  interp_handler_971,
  interp_handler_972,
  interp_handler_973,
  interp_handler_974,
  interp_handler_975,
  interp_handler_976,
  interp_handler_977,
  interp_handler_978,
  interp_handler_979,
  interp_handler_980,
  interp_handler_981,
  interp_handler_982,
  interp_handler_983,
  interp_handler_984,
  interp_handler_985,
  interp_handler_986,
  interp_handler_987,
  interp_handler_988,
  interp_handler_989,
  interp_handler_990,
  interp_handler_991,
  interp_handler_992,
  interp_handler_993,
  interp_handler_994,
  interp_handler_995,
  interp_handler_996,
  interp_handler_997,
  interp_handler_998,
  interp_handler_999,
  interp_handler_1000,
  interp_handler_1001,
  interp_handler_1002,
  interp_handler_1003,
  interp_handler_1004,
  interp_handler_1005,
  interp_handler_1006,
  interp_handler_1007,
  interp_handler_1008,
  interp_handler_1009,
  interp_handler_1010,
  interp_handler_1011,
  interp_handler_1012,
  interp_handler_1013,
  interp_handler_1014,
  interp_handler_1015,
  interp_handler_1016,
  interp_handler_1017,
  interp_handler_1018,
  interp_handler_1019,
  interp_handler_1020,
  interp_handler_1021,
  interp_handler_1022,
  interp_handler_1023,
  interp_handler_1024,
  interp_handler_1025,
  interp_handler_1026,
  interp_handler_1027,
  interp_handler_1028,
  interp_handler_1029,
  interp_handler_1030,
  interp_handler_1031,
  interp_handler_1032,
  interp_handler_1033,
  interp_handler_1034,
  interp_handler_1035,
  interp_handler_1036,
  interp_handler_1037,
  interp_handler_1038,
  interp_handler_1039,
  interp_handler_1040,
  interp_handler_1041,
  interp_handler_1042,
  interp_handler_1043,
  interp_handler_1044,
  interp_handler_1045,
  interp_handler_1046,
  interp_handler_1047,
  interp_handler_1048,
  interp_handler_1049,
  interp_handler_1050,
  interp_handler_1051,
  interp_handler_1052,
  interp_handler_1053,
  interp_handler_1054,
  interp_handler_1055,
  interp_handler_1056,
  interp_handler_1057,
  interp_handler_1058,
  interp_handler_1059,
  interp_handler_1060,
  interp_handler_1061,
  interp_handler_1062,
  interp_handler_1063,
  interp_handler_1064,
  interp_handler_1065,
  interp_handler_1066,
  interp_handler_1067,
  interp_handler_1068,
  interp_handler_1069,
  interp_handler_1070,
  interp_handler_1071,
  interp_handler_1072,
  interp_handler_1073,
  interp_handler_1074,
  interp_handler_1075,
  interp_handler_1076,
  interp_handler_1077,
  interp_handler_1078,
  interp_handler_1079,
  interp_handler_1080,
  interp_handler_1081,
  interp_handler_1082,
  interp_handler_1083,
  interp_handler_1084,
  interp_handler_1085,
  interp_handler_1086,
  interp_handler_1087,
  interp_handler_1088,
  interp_handler_1089,
  interp_handler_1090,
  interp_handler_1091,
  interp_handler_1092,
  interp_handler_1093,
  interp_handler_1094,
  interp_handler_1095,
  interp_handler_1096,
  interp_handler_1097,
  interp_handler_1098,
  interp_handler_1099,
  interp_handler_1100,
  interp_handler_1101,
  interp_handler_1102,
  interp_handler_1103,
  interp_handler_1104,
  interp_handler_1105,
  interp_handler_1106,
  interp_handler_1107,
  interp_handler_1108,
  interp_handler_1109,
  interp_handler_1110,
  interp_handler_1111,
  interp_handler_1112,
  interp_handler_1113,
  interp_handler_1114,
  interp_handler_1115,
  interp_handler_1116,
  interp_handler_1117,
  interp_handler_1118,
  interp_handler_1119,
  interp_handler_1120,
  interp_handler_1121,
  interp_handler_1122,
  interp_handler_1123,
  interp_handler_1124,
  interp_handler_1125,
  interp_handler_1126,
  interp_handler_1127,
  interp_handler_1128,
  interp_handler_1129,
  interp_handler_1130,
  interp_handler_1131,
  interp_handler_1132,
  interp_handler_1133,
  interp_handler_1134,
  interp_handler_1135,
  interp_handler_1136,
  interp_handler_1137,
  interp_handler_1138,
  interp_handler_1139,
  interp_handler_1140,
  interp_handler_1141,
  interp_handler_1142,
  interp_handler_1143,
  interp_handler_1144,
  interp_handler_1145,
  interp_handler_1146,
  interp_handler_1147,
  interp_handler_1148,
  interp_handler_1149,
  interp_handler_1150,
  interp_handler_1151,
  interp_handler_1152,
  interp_handler_1153,
  interp_handler_1154,
  interp_handler_1155,
  interp_handler_1156,
  interp_handler_1157,
  interp_handler_1158,
  interp_handler_1159,
  interp_handler_1160,
  interp_handler_1161,
  interp_handler_1162,
  interp_handler_1163,
  interp_handler_1164,
  interp_handler_1165,
  interp_handler_1166,
  interp_handler_1167,
  interp_handler_1168,
  interp_handler_1169,
  interp_handler_1170,
  interp_handler_1171,
  interp_handler_1172,
  interp_handler_1173,
  interp_handler_1174,
  interp_handler_1175,
  interp_handler_1176,
  interp_handler_1177,
  interp_handler_1178,
  interp_handler_1179,
  interp_handler_1180,
  interp_handler_1181,
  interp_handler_1182,
  interp_handler_1183,
  interp_handler_1184,
  interp_handler_1185,
  interp_handler_1186,
  interp_handler_1187,
  interp_handler_1188,
  interp_handler_1189,
  interp_handler_1190,
  interp_handler_1191,
  interp_handler_1192,
  interp_handler_1193,
  interp_handler_1194,
  interp_handler_1195,
  interp_handler_1196,
  interp_handler_1197,
  interp_handler_1198,
  interp_handler_1199,
  interp_handler_1200,
  interp_handler_1201,
  interp_handler_1202,
  interp_handler_1203,
  interp_handler_1204,
  interp_handler_1205,
  interp_handler_1206,
  interp_handler_1207,
  interp_handler_1208,
  interp_handler_1209,
  interp_handler_1210,
  interp_handler_1211,
  interp_handler_1212,
  interp_handler_1213,
  interp_handler_1214,
  interp_handler_1215,
  interp_handler_1216,
  interp_handler_1217,
  interp_handler_1218,
  interp_handler_1219,
  interp_handler_1220,
  interp_handler_1221,
  interp_handler_1222,
  interp_handler_1223,
  interp_handler_1224,
  interp_handler_1225,
  interp_handler_1226,
  interp_handler_1227,
  interp_handler_1228,
  interp_handler_1229,
  interp_handler_1230,
  interp_handler_1231,
  interp_handler_1232,
  interp_handler_1233,
  interp_handler_1234,
  interp_handler_1235,
  interp_handler_1236,
  interp_handler_1237,
  interp_handler_1238,
  interp_handler_1239,
  interp_handler_1240,
  interp_handler_1241,
  interp_handler_1242,
  interp_handler_1243,
  interp_handler_1244,
  interp_handler_1245,
  interp_handler_1246,
  interp_handler_1247,
  interp_handler_1248,
  interp_handler_1249,
  interp_handler_1250,
  interp_handler_1251,
  interp_handler_1252,
  interp_handler_1253,
  interp_handler_1254,
  interp_handler_1255,
  interp_handler_1256,
  interp_handler_1257,
  interp_handler_1258,
  interp_handler_1259,
  interp_handler_1260,
  interp_handler_1261,
  interp_handler_1262,
  interp_handler_1263,
  interp_handler_1264,
  interp_handler_1265,
  interp_handler_1266,
  interp_handler_1267,
  interp_handler_1268,
  interp_handler_1269,
  interp_handler_1270,
  interp_handler_1271,
  interp_handler_1272,
  interp_handler_1273,
  interp_handler_1274,
  interp_handler_1275,
  interp_handler_1276,
  interp_handler_1277,
  interp_handler_1278,
  interp_handler_1279,
  interp_handler_1280,
  interp_handler_1281,
  interp_handler_1282,
  interp_handler_1283,
  interp_handler_1284,
  interp_handler_1285,
  interp_handler_1286,
  interp_handler_1287,
  interp_handler_1288,
  interp_handler_1289,
  interp_handler_1290,
  interp_handler_1291,
  interp_handler_1292,
  interp_handler_1293,
  interp_handler_1294,
  interp_handler_1295,
  interp_handler_1296,
  interp_handler_1297,
  interp_handler_1298,
  interp_handler_1299,
  interp_handler_1300,
  interp_handler_1301,
  interp_handler_1302,
  interp_handler_1303,
  interp_handler_1304,
  interp_handler_1305,
  interp_handler_1306,
  interp_handler_1307,
  interp_handler_1308,
  interp_handler_1309,
  interp_handler_1310,
  interp_handler_1311,
  interp_handler_1312,
  interp_handler_1313,
  interp_handler_1314,
  interp_handler_1315,
  interp_handler_1316,
  interp_handler_1317,
  interp_handler_1318,
  interp_handler_1319,
  interp_handler_1320,
  interp_handler_1321,
  interp_handler_1322,
  interp_handler_1323,
  interp_handler_1324,
  interp_handler_1325,
  interp_handler_1326,
  interp_handler_1327,
  interp_handler_1328,
  interp_handler_1329,
  interp_handler_1330,
  interp_handler_1331,
  interp_handler_1332,
  interp_handler_1333,
  interp_handler_1334,
  interp_handler_1335,
  interp_handler_1336,
  interp_handler_1337,
  interp_handler_1338,
  interp_handler_1339,
  interp_handler_1340,
  interp_handler_1341,
  interp_handler_1342,
  interp_handler_1343,
  interp_handler_1344,
  interp_handler_1345,
  interp_handler_1346,
  interp_handler_1347,
  interp_handler_1348,
  interp_handler_1349,
  interp_handler_1350,
  interp_handler_1351,
  interp_handler_1352,
  interp_handler_1353,
  interp_handler_1354,
  interp_handler_1355,
  interp_handler_1356,
  interp_handler_1357,
  interp_handler_1358,
  interp_handler_1359,
  interp_handler_1360,
  interp_handler_1361,
  interp_handler_1362,
  interp_handler_1363,
  interp_handler_1364,
  interp_handler_1365,
  interp_handler_1366,
  interp_handler_1367,
  interp_handler_1368,
  interp_handler_1369,
  interp_handler_1370,
  interp_handler_1371,
  interp_handler_1372,
  interp_handler_1373,
  interp_handler_1374,
  interp_handler_1375,
  interp_handler_1376,
  interp_handler_1377,
  interp_handler_1378,
  interp_handler_1379,
  interp_handler_1380,
  interp_handler_1381,
  interp_handler_1382,
  interp_handler_1383,
  interp_handler_1384,
  interp_handler_1385,
  interp_handler_1386,
  interp_handler_1387,
  interp_handler_1388,
  interp_handler_1389,
  interp_handler_1390,
  interp_handler_1391,
  interp_handler_1392,
  interp_handler_1393,
  interp_handler_1394,
  interp_handler_1395,
  interp_handler_1396,
  interp_handler_1397,
  interp_handler_1398,
  interp_handler_1399,
  interp_handler_1400,
  interp_handler_1401,
  interp_handler_1402,
  interp_handler_1403,
  interp_handler_1404,
  interp_handler_1405,
  interp_handler_1406,
  interp_handler_1407,
  interp_handler_1408,
  interp_handler_1409,
  interp_handler_1410,
  interp_handler_1411,
  interp_handler_1412,
  interp_handler_1413,
  interp_handler_1414,
  interp_handler_1415,
  interp_handler_1416,
  interp_handler_1417,
  interp_handler_1418,
  interp_handler_1419,
  interp_handler_1420,
  interp_handler_1421,
  interp_handler_1422,
  interp_handler_1423,
  interp_handler_1424,
  interp_handler_1425,
  interp_handler_1426,
  interp_handler_1427,
  interp_handler_1428,
  interp_handler_1429,
  interp_handler_1430,
  interp_handler_1431,
  interp_handler_1432,
  interp_handler_1433,
  interp_handler_1434,
  interp_handler_1435,
  interp_handler_1436,
  interp_handler_1437,
  interp_handler_1438,
  interp_handler_1439,
  interp_handler_1440,
  interp_handler_1441,
  interp_handler_1442,
  interp_handler_1443,
  interp_handler_1444,
  interp_handler_1445,
  interp_handler_1446,
  interp_handler_1447,
  interp_handler_1448,
  interp_handler_1449,
  interp_handler_1450,
  interp_handler_1451,
  interp_handler_1452,
  interp_handler_1453,
  interp_handler_1454,
  interp_handler_1455,
  interp_handler_1456,
  interp_handler_1457,
  interp_handler_1458,
  interp_handler_1459,
  interp_handler_1460,
  interp_handler_1461,
  interp_handler_1462,
  interp_handler_1463,
  interp_handler_1464,
  interp_handler_1465,
  interp_handler_1466,
  interp_handler_1467,
  interp_handler_1468,
  interp_handler_1469,
  interp_handler_1470,
  interp_handler_1471,
  interp_handler_1472,
  interp_handler_1473,
  interp_handler_1474,
  interp_handler_1475,
  interp_handler_1476,
  interp_handler_1477,
  interp_handler_1478,
  interp_handler_1479,
  interp_handler_1480,
  interp_handler_1481,
  interp_handler_1482,
  interp_handler_1483,
  interp_handler_1484,
  interp_handler_1485,
  interp_handler_1486,
  interp_handler_1487,
  interp_handler_1488,
  interp_handler_1489,
  interp_handler_1490,
  interp_handler_1491,
  interp_handler_1492,
  interp_handler_1493,
  interp_handler_1494,
  interp_handler_1495,
  interp_handler_1496,
  interp_handler_1497,
  interp_handler_1498,
  interp_handler_1499,
  interp_handler_1500,
  interp_handler_1501,
  interp_handler_1502,
  interp_handler_1503,
  interp_handler_1504,
  interp_handler_1505,
  interp_handler_1506,
  interp_handler_1507,
  interp_handler_1508,
  interp_handler_1509,
  interp_handler_1510,
  interp_handler_1511,
  interp_handler_1512,
  interp_handler_1513,
  interp_handler_1514,
  interp_handler_1515,
  interp_handler_1516,
  interp_handler_1517,
  interp_handler_1518,
  interp_handler_1519,
  interp_handler_1520,
  interp_handler_1521,
  interp_handler_1522,
  interp_handler_1523,
  interp_handler_1524,
  interp_handler_1525,
  interp_handler_1526,
  interp_handler_1527,
  interp_handler_1528,
  interp_handler_1529,
  interp_handler_1530,
  interp_handler_1531,
  interp_handler_1532,
  interp_handler_1533,
  interp_handler_1534,
  interp_handler_1535,
  interp_handler_1536,
  interp_handler_1537,
  interp_handler_1538,
  interp_handler_1539,
  interp_handler_1540,
  interp_handler_1541,
  interp_handler_1542,
  interp_handler_1543,
  interp_handler_1544,
  interp_handler_1545,
  interp_handler_1546,
  interp_handler_1547,
  interp_handler_1548,
  interp_handler_1549,
  interp_handler_1550,
  interp_handler_1551,
  interp_handler_1552,
  interp_handler_1553,
  interp_handler_1554,
  interp_handler_1555,
  interp_handler_1556,
  interp_handler_1557,
  interp_handler_1558,
  interp_handler_1559,
  interp_handler_1560,
  interp_handler_1561,
  interp_handler_1562,
  interp_handler_1563,
  interp_handler_1564,
  interp_handler_1565,
  interp_handler_1566,
  interp_handler_1567,
  interp_handler_1568,
  interp_handler_1569,
  interp_handler_1570,
  interp_handler_1571,
  interp_handler_1572,
  interp_handler_1573,
  interp_handler_1574,
  interp_handler_1575,
  interp_handler_1576,
  interp_handler_1577,
  interp_handler_1578,
  interp_handler_1579,
  interp_handler_1580,
  interp_handler_1581,
  interp_handler_1582,
  interp_handler_1583,
  interp_handler_1584,
  interp_handler_1585,
  interp_handler_1586,
  interp_handler_1587,
  interp_handler_1588,
  interp_handler_1589,
  interp_handler_1590,
  interp_handler_1591,
  interp_handler_1592,
  interp_handler_1593,
  interp_handler_1594,
  interp_handler_1595,
  interp_handler_1596,
  interp_handler_1597,
  interp_handler_1598,
  interp_handler_1599,
  interp_handler_1600,
  interp_handler_1601,
  interp_handler_1602,
  interp_handler_1603,
  interp_handler_1604,
  interp_handler_1605,
  interp_handler_1606,
  interp_handler_1607,
  interp_handler_1608,
  interp_handler_1609,
  interp_handler_1610,
  interp_handler_1611,
  interp_handler_1612,
  interp_handler_1613,
  interp_handler_1614,
  interp_handler_1615,
  interp_handler_1616,
  interp_handler_1617,
  interp_handler_1618,
  interp_handler_1619,
  interp_handler_1620,
  interp_handler_1621,
  interp_handler_1622,
  interp_handler_1623,
  interp_handler_1624,
  interp_handler_1625,
  interp_handler_1626,
  interp_handler_1627,
  interp_handler_1628,
  interp_handler_1629,
  interp_handler_1630,
  interp_handler_1631,
  interp_handler_1632,
  interp_handler_1633,
  interp_handler_1634,
  interp_handler_1635,
  interp_handler_1636,
  interp_handler_1637,
  interp_handler_1638,
  interp_handler_1639,
  interp_handler_1640,
  interp_handler_1641,
  interp_handler_1642,
  interp_handler_1643,
  interp_handler_1644,
  interp_handler_1645,
  interp_handler_1646,
  interp_handler_1647,
  interp_handler_1648,
  interp_handler_1649,
  interp_handler_1650,
  interp_handler_1651,
  interp_handler_1652,
  interp_handler_1653,
  interp_handler_1654,
  interp_handler_1655,
  interp_handler_1656,
  interp_handler_1657,
  interp_handler_1658,
  interp_handler_1659,
  interp_handler_1660,
  interp_handler_1661,
  interp_handler_1662,
  interp_handler_1663,
  interp_handler_1664,
  interp_handler_1665,
  interp_handler_1666,
  interp_handler_1667,
  interp_handler_1668,
  interp_handler_1669,
  interp_handler_1670,
  interp_handler_1671,
  interp_handler_1672,
  interp_handler_1673,
  interp_handler_1674,
  interp_handler_1675,
  interp_handler_1676,
  interp_handler_1677,
  interp_handler_1678,
  interp_handler_1679,
  interp_handler_1680,
  interp_handler_1681,
  interp_handler_1682,
  interp_handler_1683,
  interp_handler_1684,
  interp_handler_1685,
  interp_handler_1686,
  interp_handler_1687,
  interp_handler_1688,
  interp_handler_1689,
  interp_handler_1690,
  interp_handler_1691,
  interp_handler_1692,
  interp_handler_1693,
  interp_handler_1694,
  interp_handler_1695,
  interp_handler_1696,
  interp_handler_1697,
  interp_handler_1698,
  interp_handler_1699,
  interp_handler_1700,
  interp_handler_1701,
  interp_handler_1702,
  interp_handler_1703,
  interp_handler_1704,
  interp_handler_1705,
  interp_handler_1706,
  interp_handler_1707,
  interp_handler_1708,
  interp_handler_1709,
  interp_handler_1710,
  interp_handler_1711,
  interp_handler_1712,
  interp_handler_1713,
  interp_handler_1714,
  interp_handler_1715,
  interp_handler_1716,
  interp_handler_1717,
  interp_handler_1718,
  interp_handler_1719,
  interp_handler_1720,
  interp_handler_1721,
  interp_handler_1722,
  interp_handler_1723,
  interp_handler_1724,
  interp_handler_1725,
  interp_handler_1726,
  interp_handler_1727,
  interp_handler_1728,
  interp_handler_1729,
  interp_handler_1730,
  interp_handler_1731,
  interp_handler_1732,
  interp_handler_1733,
  interp_handler_1734,
  interp_handler_1735,
  interp_handler_1736,
  interp_handler_1737,
  interp_handler_1738,
  interp_handler_1739,
  interp_handler_1740,
  interp_handler_1741,
  interp_handler_1742,
  interp_handler_1743,
  interp_handler_1744,
  interp_handler_1745,
  interp_handler_1746,
  interp_handler_1747,
  interp_handler_1748,
  interp_handler_1749,
  interp_handler_1750,
  interp_handler_1751,
  interp_handler_1752,
  interp_handler_1753,
  interp_handler_1754,
  interp_handler_1755,
  interp_handler_1756,
  interp_handler_1757,
  interp_handler_1758,
  interp_handler_1759,
  interp_handler_1760,
  interp_handler_1761,
  interp_handler_1762,
  interp_handler_1763,
  interp_handler_1764,
  interp_handler_1765,
  interp_handler_1766,
  interp_handler_1767,
  interp_handler_1768,
  interp_handler_1769,
  interp_handler_1770,
  interp_handler_1771,
  interp_handler_1772,
  interp_handler_1773,
  interp_handler_1774,
  interp_handler_1775,
  interp_handler_1776,
  interp_handler_1777,
  interp_handler_1778,
  interp_handler_1779,
  interp_handler_1780,
  interp_handler_1781,
  interp_handler_1782,
  interp_handler_1783,
  interp_handler_1784,
  interp_handler_1785,
  interp_handler_1786,
  interp_handler_1787,
  interp_handler_1788,
  interp_handler_1789,
  interp_handler_1790,
  interp_handler_1791,
  interp_handler_1792,
  interp_handler_1793,
  interp_handler_1794,
  interp_handler_1795,
  interp_handler_1796,
  interp_handler_1797,
  interp_handler_1798,
  interp_handler_1799,
  interp_handler_1800,
  interp_handler_1801,
  interp_handler_1802,
  interp_handler_1803,
  interp_handler_1804,
  interp_handler_1805,
  interp_handler_1806,
  interp_handler_1807,
  interp_handler_1808,
  interp_handler_1809,
  interp_handler_1810,
  interp_handler_1811,
  interp_handler_1812,
  interp_handler_1813,
  interp_handler_1814,
  interp_handler_1815,
  interp_handler_1816,
  interp_handler_1817,
  interp_handler_1818,
  interp_handler_1819,
  interp_handler_1820,
  interp_handler_1821,
  interp_handler_1822,
  interp_handler_1823,
  interp_handler_1824,
  interp_handler_1825,
  interp_handler_1826,
  interp_handler_1827,
  interp_handler_1828,
  interp_handler_1829,
  interp_handler_1830,
  interp_handler_1831,
  interp_handler_1832,
  interp_handler_1833,
  interp_handler_1834,
  interp_handler_1835,
  interp_handler_1836,
  interp_handler_1837,
  interp_handler_1838,
  interp_handler_1839,
  interp_handler_1840,
  interp_handler_1841,
  interp_handler_1842,
  interp_handler_1843,
  interp_handler_1844,
  interp_handler_1845,
  interp_handler_1846,
  interp_handler_1847,
  interp_handler_1848,
  interp_handler_1849,
  interp_handler_1850,
  interp_handler_1851,
  interp_handler_1852,
  interp_handler_1853,
  interp_handler_1854,
  interp_handler_1855,
  interp_handler_1856,
  interp_handler_1857,
  interp_handler_1858,
  interp_handler_1859,
  interp_handler_1860,
  interp_handler_1861,
  interp_handler_1862,
  interp_handler_1863,
  interp_handler_1864,
  interp_handler_1865,
  interp_handler_1866,
  interp_handler_1867,
  interp_handler_1868,
  interp_handler_1869,
  interp_handler_1870,
  interp_handler_1871,
  interp_handler_1872,
  interp_handler_1873,
  interp_handler_1874,
  interp_handler_1875,
  interp_handler_1876,
  interp_handler_1877,
  interp_handler_1878,
  interp_handler_1879,
  interp_handler_1880,
  interp_handler_1881,
  interp_handler_1882,
  interp_handler_1883,
  interp_handler_1884,
  interp_handler_1885,
  interp_handler_1886,
  interp_handler_1887,
  interp_handler_1888,
  interp_handler_1889,
  interp_handler_1890,
  interp_handler_1891,
  interp_handler_1892,
  interp_handler_1893,
  interp_handler_1894,
  interp_handler_1895,
  interp_handler_1896,
  interp_handler_1897,
  interp_handler_1898,
  interp_handler_1899,
  interp_handler_1900,
  interp_handler_1901,
  interp_handler_1902,
  interp_handler_1903,
  interp_handler_1904,
  interp_handler_1905,
  interp_handler_1906,
  interp_handler_1907,
  interp_handler_1908,
  interp_handler_1909,
  interp_handler_1910,
  interp_handler_1911,
  interp_handler_1912,
  interp_handler_1913,
  interp_handler_1914,
  interp_handler_1915,
  interp_handler_1916,
  interp_handler_1917,
  interp_handler_1918,
  interp_handler_1919,
  interp_handler_1920,
  interp_handler_1921,
  interp_handler_1922,
  interp_handler_1923,
  interp_handler_1924,
  interp_handler_1925,
  interp_handler_1926,
  interp_handler_1927,
  interp_handler_1928,
  interp_handler_1929,
  interp_handler_1930,
  interp_handler_1931,
  interp_handler_1932,
  interp_handler_1933,
  interp_handler_1934,
  interp_handler_1935,
  interp_handler_1936,
  interp_handler_1937,
  interp_handler_1938,
  interp_handler_1939,
  interp_handler_1940,
  interp_handler_1941,
  interp_handler_1942,
  interp_handler_1943,
  interp_handler_1944,
  interp_handler_1945,
  interp_handler_1946,
  interp_handler_1947,
  interp_handler_1948,
  interp_handler_1949,
  interp_handler_1950,
  interp_handler_1951,
  interp_handler_1952,
  interp_handler_1953,
  interp_handler_1954,
  interp_handler_1955,
  interp_handler_1956,
  interp_handler_1957,
  interp_handler_1958,
  interp_handler_1959,
  interp_handler_1960,
  interp_handler_1961,
  interp_handler_1962,
  interp_handler_1963,
  interp_handler_1964,
  interp_handler_1965,
  interp_handler_1966,
  interp_handler_1967,
  interp_handler_1968,
  interp_handler_1969,
  interp_handler_1970,
  interp_handler_1971,
  interp_handler_1972,
  interp_handler_1973,
  interp_handler_1974,
  interp_handler_1975,
  interp_handler_1976,
  interp_handler_1977,
  interp_handler_1978,
  interp_handler_1979,
  interp_handler_1980,
  interp_handler_1981,
  interp_handler_1982,
  interp_handler_1983,
  interp_handler_1984,
  interp_handler_1985,
  interp_handler_1986,
  interp_handler_1987,
  interp_handler_1988,
  interp_handler_1989,
  interp_handler_1990,
  interp_handler_1991,
  interp_handler_1992,
  interp_handler_1993,
  interp_handler_1994,
  interp_handler_1995,
  interp_handler_1996,
  interp_handler_1997,
  interp_handler_1998,
  interp_handler_1999,
};
