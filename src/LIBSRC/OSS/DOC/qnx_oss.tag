<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="page">
    <filename>index</filename>
    <title></title>
    <name>index</name>
  </compound>
  <compound kind="file">
    <name>oss.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Init</name>
      <anchor>a1</anchor>
      <arglist>(char *instName, OSS_HANDLE **ossP, pid_t(*currentPid)(void))</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Exit</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE **ossP)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_DbgLevelSet</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 newLevel)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_DbgLevelGet</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss.h</name>
    <path>/home/ch/WORK/QNX/src/INCLUDE/COM/MEN/</path>
    <filename>oss_8h</filename>
    <class kind="struct">OSS_CALLBACK_SET</class>
    <class kind="struct">OSS_DL_LIST</class>
    <class kind="struct">OSS_DL_NODE</class>
    <class kind="struct">OSS_RESOURCES</class>
    <class kind="struct">OSS_SHMEM_CLEAR</class>
    <class kind="struct">OSS_SHMEM_LINK</class>
    <class kind="struct">OSS_SHMEM_SET</class>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_8</name>
      <anchor>a29</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_16</name>
      <anchor>a30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_32</name>
      <anchor>a31</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_64</name>
      <anchor>a32</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_16UD</name>
      <anchor>a33</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_24UD</name>
      <anchor>a34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_24UB</name>
      <anchor>a35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_32UD</name>
      <anchor>a36</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_32UB</name>
      <anchor>a37</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_32UB64</name>
      <anchor>a38</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_Ident</name>
      <anchor>a105</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>OSS_MemGet</name>
      <anchor>a106</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 size, u_int32 *gotsizeP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemFree</name>
      <anchor>a107</anchor>
      <arglist>(OSS_HANDLE *osHdl, void *addr, u_int32 size)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemChk</name>
      <anchor>a108</anchor>
      <arglist>(OSS_HANDLE *osHdl, void *addr, u_int32 size, int32 mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemCopy</name>
      <anchor>a109</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 size, char *src, char *dest)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemFill</name>
      <anchor>a110</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 size, char *adr, int8 value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_StrCpy</name>
      <anchor>a111</anchor>
      <arglist>(OSS_HANDLE *oss, char *from, char *to)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_StrLen</name>
      <anchor>a112</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *string)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrCmp</name>
      <anchor>a113</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *str1, char *str2)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrNcmp</name>
      <anchor>a114</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *str1, char *str2, u_int32 nbrOfBytes)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_StrTok</name>
      <anchor>a115</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *string, char *separator, char **lastP)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqMask</name>
      <anchor>a116</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqUnMask</name>
      <anchor>a117</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IrqLevelToVector</name>
      <anchor>a118</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busType, int32 level, int32 *vector)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockCreate</name>
      <anchor>a119</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRemove</name>
      <anchor>a120</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockAcquire</name>
      <anchor>a121</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRelease</name>
      <anchor>a122</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigCreate</name>
      <anchor>a123</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 value, OSS_SIG_HANDLE **sigHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigSend</name>
      <anchor>a124</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SIG_HANDLE *sigHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigRemove</name>
      <anchor>a125</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SIG_HANDLE **sigHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigInfo</name>
      <anchor>a126</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SIG_HANDLE *sigHdl, int32 *signalNbrP, int32 *processIdP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemCreate</name>
      <anchor>a127</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 semType, int32 initVal, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemRemove</name>
      <anchor>a128</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemWait</name>
      <anchor>a129</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SEM_HANDLE *semHandle, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemSignal</name>
      <anchor>a130</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SEM_HANDLE *semHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_DbgLevelSet</name>
      <anchor>a131</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 newLevel)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_DbgLevelGet</name>
      <anchor>a132</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Delay</name>
      <anchor>a133</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_TickRateGet</name>
      <anchor>a134</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_TickGet</name>
      <anchor>a135</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_GetPid</name>
      <anchor>a136</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelayInit</name>
      <anchor>a137</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelay</name>
      <anchor>a138</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 mikroSec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_BusToPhysAddr</name>
      <anchor>a139</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busType, void **physicalAddrP,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciGetConfig</name>
      <anchor>a140</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSetConfig</name>
      <anchor>a141</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSlotToPciDevice</name>
      <anchor>a142</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busNbr, int32 mechSlot, int32 *pciDevNbrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IsaGetConfig</name>
      <anchor>a143</anchor>
      <arglist>(OSS_HANDLE *osHdl, int8 *cardVendId, int16 cardProdId, int8 *devVendId, int16 devProdId, int32 devNbr, int32 resType, int32 resNbr, u_int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AssignResources</name>
      <anchor>a144</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapPhysToVirtAddr</name>
      <anchor>a145</anchor>
      <arglist>(OSS_HANDLE *osHdl, void *physAddr, u_int32 size, int32 addrSpace, int32 busType, int32 busNbr, void **virtAddrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVirtAddr</name>
      <anchor>a146</anchor>
      <arglist>(OSS_HANDLE *osHdl, void **virtAddrP, u_int32 size, int32 addrSpace)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Sprintf</name>
      <anchor>a147</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Vsprintf</name>
      <anchor>a148</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt, va_list arg)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmCreate</name>
      <anchor>a149</anchor>
      <arglist>(OSS_HANDLE *oss, void(*funct)(void *arg), void *arg, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmRemove</name>
      <anchor>a150</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmSet</name>
      <anchor>a151</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm, u_int32 msec, u_int32 cyclic, u_int32 *realMsec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmClear</name>
      <anchor>a152</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_GetSmbHdl</name>
      <anchor>a168</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 busNbr, void **smbHdlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SetSmbHdl</name>
      <anchor>a169</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 busNbr, void *smbHdl)</arglist>
    </member>
    <member kind="function">
      <type>u_int16</type>
      <name>OSS_Swap16</name>
      <anchor>a170</anchor>
      <arglist>(u_int16 word)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_Swap32</name>
      <anchor>a171</anchor>
      <arglist>(u_int32 dword)</arglist>
    </member>
    <member kind="function">
      <type>u_int64</type>
      <name>OSS_Swap64</name>
      <anchor>a172</anchor>
      <arglist>(u_int64 qword)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_LIST *</type>
      <name>OSS_DL_NewList</name>
      <anchor>a173</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_Remove</name>
      <anchor>a174</anchor>
      <arglist>(OSS_DL_NODE *n)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_RemHead</name>
      <anchor>a175</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_AddTail</name>
      <anchor>a176</anchor>
      <arglist>(OSS_DL_LIST *l, OSS_DL_NODE *n)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_alarm.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__alarm_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmCreate</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss, void(*funct)(void *arg), void *arg, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmRemove</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmSet</name>
      <anchor>a8</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm, u_int32 msec, u_int32 cyclic, u_int32 *realMsecP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmClear</name>
      <anchor>a9</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm)</arglist>
    </member>
    <member kind="function">
      <type>OSS_ALARM_STATE</type>
      <name>OSS_AlarmMask</name>
      <anchor>a10</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_AlarmRestore</name>
      <anchor>a11</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_STATE oldState)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_bustoaddr.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__bustoaddr_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_BusToPhysAddr</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, void **physicalAddrP,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciGetConfig</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSetConfig</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSlotToPciDevice</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 mechSlot, int32 *pciDevNbrP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_clib.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__clib_8c</filename>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemCopy</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, char *src, char *dest)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemFill</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, char *adr, int8 value)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_StrLen</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, char *string)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrCmp</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, char *str1, char *str2)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrNcmp</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, char *str1, char *str2, u_int32 nbrOfBytes)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_StrTok</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss, char *string, char *separator, char **lastP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Sprintf</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Vsprintf</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt, va_list arg)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_StrCpy</name>
      <anchor>a8</anchor>
      <arglist>(OSS_HANDLE *oss, char *from, char *to)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_dl_list.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__dl__list_8c</filename>
    <member kind="function">
      <type>OSS_DL_LIST *</type>
      <name>OSS_DL_NewList</name>
      <anchor>a0</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_Remove</name>
      <anchor>a1</anchor>
      <arglist>(OSS_DL_NODE *n)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_RemHead</name>
      <anchor>a2</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_AddTail</name>
      <anchor>a3</anchor>
      <arglist>(OSS_DL_LIST *l, OSS_DL_NODE *n)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_ident.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__ident_8c</filename>
    <member kind="function">
      <type>char *</type>
      <name>OSS_Ident</name>
      <anchor>a1</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_irq.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__irq_8c</filename>
    <member kind="function">
      <type>OSS_IRQ_STATE</type>
      <name>OSS_IrqMaskR</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqRestore</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle, OSS_IRQ_STATE oldState)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqMask</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqUnMask</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IrqLevelToVector</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 irqNbr, int32 *vectorP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_isa_pnp.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__isa__pnp_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IsaGetConfig</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int8 *cardVendId, int16 cardProdId, int8 *devVendId, int16 devProdId, int32 devNbr, int32 resType, int32 resNbr, u_int32 *valueP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_map.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__map_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapPhysToVirtAddr</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, void *physAddr, u_int32 size, int32 addrSpace, int32 busType, int32 busNbr, void **virtAddrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVirtAddr</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, void **virtAddrP, u_int32 size, int32 addrSpace)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_mem.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__mem_8c</filename>
    <member kind="function">
      <type>void *</type>
      <name>OSS_MemGet</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, u_int32 *gotsizeP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemFree</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemChk</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size, int32 mode)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_resource.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__resource_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AssignResources</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnAssignResources</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_sem.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__sem_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemCreate</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 semType, int32 initVal, OSS_SEM_HANDLE **semP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemRemove</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemWait</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemSignal</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_sig.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__sig_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigCreate</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 signal, OSS_SIG_HANDLE **sigP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigRemove</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE **sigP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigSend</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE *sig)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigInfo</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE *sig, int32 *signalP, int32 *pidP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_smb.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__smb_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SetSmbHdl</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 busNbr, void *smbHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_GetSmbHdl</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 busNbr, void **smbHdlP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_specification.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/COM/</path>
    <filename>oss__specification_8c</filename>
    <member kind="define">
      <type>#define</type>
      <name>OSS_SWAP16</name>
      <anchor>a0</anchor>
      <arglist>(word)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_SWAP32</name>
      <anchor>a1</anchor>
      <arglist>(dword)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_SWAP64</name>
      <anchor>a2</anchor>
      <arglist>(qword)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_Ident</name>
      <anchor>a3</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_DbgLevelSet</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 newLevel)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_DbgLevelGet</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>OSS_MemGet</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, u_int32 *gotsizeP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemFree</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemChk</name>
      <anchor>a8</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size, int32 mode)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmCreate</name>
      <anchor>a9</anchor>
      <arglist>(OSS_HANDLE *oss, void(*funct)(void *arg), void *arg, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmRemove</name>
      <anchor>a10</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmSet</name>
      <anchor>a11</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarmP, u_int32 msec, u_int32 cyclic, u_int32 *realMsecP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmClear</name>
      <anchor>a12</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarmP)</arglist>
    </member>
    <member kind="function">
      <type>OSS_ALARM_STATE</type>
      <name>OSS_AlarmMask</name>
      <anchor>a13</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_AlarmRestore</name>
      <anchor>a14</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_STATE oldState)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_LIST *</type>
      <name>OSS_DL_NewList</name>
      <anchor>a15</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_Remove</name>
      <anchor>a16</anchor>
      <arglist>(OSS_DL_NODE *n)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_RemHead</name>
      <anchor>a17</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_AddTail</name>
      <anchor>a18</anchor>
      <arglist>(OSS_DL_LIST *l, OSS_DL_NODE *n)</arglist>
    </member>
    <member kind="function">
      <type>OSS_IRQ_STATE</type>
      <name>OSS_IrqMaskR</name>
      <anchor>a19</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqRestore</name>
      <anchor>a20</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle, OSS_IRQ_STATE oldState)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqMask</name>
      <anchor>a21</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqUnMask</name>
      <anchor>a22</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IrqLevelToVector</name>
      <anchor>a23</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 irqNbr, int32 *vectorP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockCreate</name>
      <anchor>a24</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRemove</name>
      <anchor>a25</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockAcquire</name>
      <anchor>a26</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRelease</name>
      <anchor>a27</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemCreate</name>
      <anchor>a28</anchor>
      <arglist>(OSS_HANDLE *oss, int32 semType, int32 initVal, OSS_SEM_HANDLE **semP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemRemove</name>
      <anchor>a29</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE **semP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemWait</name>
      <anchor>a30</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemSignal</name>
      <anchor>a31</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigCreate</name>
      <anchor>a32</anchor>
      <arglist>(OSS_HANDLE *oss, int32 signal, OSS_SIG_HANDLE **sigP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigRemove</name>
      <anchor>a33</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE **sigP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigSend</name>
      <anchor>a34</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE *sig)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigInfo</name>
      <anchor>a35</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE *sig, int32 *signalP, int32 *pidP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Delay</name>
      <anchor>a36</anchor>
      <arglist>(OSS_HANDLE *oss, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelayInit</name>
      <anchor>a37</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelay</name>
      <anchor>a38</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 microSec)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_TickGet</name>
      <anchor>a39</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_TickRateGet</name>
      <anchor>a40</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapPhysToVirtAddr</name>
      <anchor>a41</anchor>
      <arglist>(OSS_HANDLE *oss, void *physAddr, u_int32 size, int32 addrSpace, int32 busType, int32 busNbr, void **virtAddrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVirtAddr</name>
      <anchor>a42</anchor>
      <arglist>(OSS_HANDLE *oss, void **virtAddrP, u_int32 size, int32 addrSpace)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_BusToPhysAddr</name>
      <anchor>a43</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, void **physicalAddrP,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapVmeAddr</name>
      <anchor>a44</anchor>
      <arglist>(OSS_HANDLE *oss, u_int64 vmeBusAddr, u_int32 addrMode, u_int32 maxDataMode, u_int32 size, u_int32 flags, void **locAddrP, void **addrWinHdlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVmeAddr</name>
      <anchor>a45</anchor>
      <arglist>(OSS_HANDLE *oss, void *addrWinHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciGetConfig</name>
      <anchor>a46</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSetConfig</name>
      <anchor>a47</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSlotToPciDevice</name>
      <anchor>a48</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 mechSlot, int32 *pciDevNbrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IsaGetConfig</name>
      <anchor>a49</anchor>
      <arglist>(OSS_HANDLE *oss, int8 *cardVendId, int16 cardProdId, int8 *devVendId, int16 devProdId, int32 devNbr, int32 resType, int32 resNbr, u_int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AssignResources</name>
      <anchor>a50</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnAssignResources</name>
      <anchor>a51</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
    <member kind="function">
      <type>u_int16</type>
      <name>OSS_Swap16</name>
      <anchor>a52</anchor>
      <arglist>(u_int16 word)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_Swap32</name>
      <anchor>a53</anchor>
      <arglist>(u_int32 dword)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_Swap64</name>
      <anchor>a54</anchor>
      <arglist>(u_int64 qword)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_GetPid</name>
      <anchor>a55</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemCopy</name>
      <anchor>a56</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, char *src, char *dest)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemFill</name>
      <anchor>a57</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, char *adr, int8 value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_StrCpy</name>
      <anchor>a58</anchor>
      <arglist>(OSS_HANDLE *oss, char *from, char *to)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_StrLen</name>
      <anchor>a59</anchor>
      <arglist>(OSS_HANDLE *oss, char *string)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrCmp</name>
      <anchor>a60</anchor>
      <arglist>(OSS_HANDLE *oss, char *str1, char *str2)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrNcmp</name>
      <anchor>a61</anchor>
      <arglist>(OSS_HANDLE *oss, char *str1, char *str2, u_int32 nbrOfBytes)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_StrTok</name>
      <anchor>a62</anchor>
      <arglist>(OSS_HANDLE *oss, char *string, char *separator, char **lastP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Sprintf</name>
      <anchor>a63</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Vsprintf</name>
      <anchor>a64</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt, va_list arg)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_GetSmbHdl</name>
      <anchor>a65</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 busNbr, void **smbHdlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SetSmbHdl</name>
      <anchor>a66</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 busNbr, void *smbHdl)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_spinlock.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__spinlock_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockCreate</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRemove</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockAcquire</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRelease</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_swap.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__swap_8c</filename>
    <member kind="function">
      <type>u_int16</type>
      <name>OSS_Swap16</name>
      <anchor>a0</anchor>
      <arglist>(u_int16 word)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_Swap32</name>
      <anchor>a1</anchor>
      <arglist>(u_int32 dword)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_task.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__task_8c</filename>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_GetPid</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_time.c</name>
    <path>/disc/users/ch/WORK/QNX/src/LIBSRC/OSS/</path>
    <filename>oss__time_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Delay</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelayInit</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelay</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 usec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_TickRateGet</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_TickGet</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>dummy</name>
    <title></title>
    <filename>dummy</filename>
  </compound>
  <compound kind="page">
    <name>qnxossalarmusage</name>
    <title></title>
    <filename>qnxossalarmusage</filename>
  </compound>
  <compound kind="page">
    <name>qnxsemusage</name>
    <title></title>
    <filename>qnxsemusage</filename>
  </compound>
  <compound kind="page">
    <name>qnxsigusage</name>
    <title></title>
    <filename>qnxsigusage</filename>
  </compound>
  <compound kind="page">
    <name>osscommonspec</name>
    <title>Common OSS specificiation &quot;MDIS4/2003-0&quot;</title>
    <filename>osscommonspec</filename>
  </compound>
  <compound kind="page">
    <name>ossalarmusage</name>
    <title>Gernal Usage of OSS Alarms</title>
    <filename>ossalarmusage</filename>
  </compound>
  <compound kind="page">
    <name>ossdllusage</name>
    <title>Using OSS Double Linked Lists</title>
    <filename>ossdllusage</filename>
  </compound>
  <compound kind="page">
    <name>osssemusage</name>
    <title>Using OSS Semaphores</title>
    <filename>osssemusage</filename>
  </compound>
  <compound kind="page">
    <name>osssigusage</name>
    <title>Using OSS Signals</title>
    <filename>osssigusage</filename>
  </compound>
</tagfile>
